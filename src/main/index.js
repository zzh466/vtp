import { app, BrowserWindow, ipcMain, dialog , Notification} from 'electron'
import  { receiveData }  from '../ctp/dataStruct';
import { errorLog, infoLog, devLog} from './log';

import net from 'net';
import udpClient from './udp';
import cppmsg, { msg } from 'cppmsg';
import { Buffer } from 'buffer';
import Trade from './trade';
import OnlineTrade from './onlineTrade';
import FakeTrade from './faketrade';
import meun, {childwin, subscribeIndicatorWin} from  './menu';
import  './export';
import  './config';
import  request  from './request';
import '../renderer/store';
import {version, winURL, specialExchangeId, tagTime } from '../renderer/utils/utils'
import  { exec } from 'child_process';
import  events  from 'events';
import { error } from 'console';


const Mainemitter = new  events.EventEmitter();
let COLOSEALL = false;
/**
 * Set `__static` path to static files in production
 * https://simulatedgreg.gitbooks.io/electron-vue/content/en/using-static-assets.html
 */
if (process.env.NODE_ENV !== 'development') {
  global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

let mainWindow;
let trade = {};
let STARTTRADE = false;
let Maincycle;
let LOCK = false;
let LOCKed = false;
let CFFEXLACK = false;
let DicatorMap = {};
let flagTime = +new Date()
let TMPCLOSE = false;
let InstrumentIDINFO = []

function checkLock(instrumentID){
 
  if(LOCK && (!TMPCLOSE || !instrumentID || InstrumentIDINFO.find(e => e.InstrumentID === instrumentID))){
      return true
  }
  return false
}
function createWindow () {
  /**
   * Initial window options
   */

  mainWindow = new BrowserWindow({
    height: 333,
    useContentSize: true,
    width: 500,
    title: `Vtp  ${version}`,
   
    webPreferences: {
      webSecurity: false,
      nodeIntegration: true,
      contextIsolation: false
    }
  })
  
  // mainWindow.webContents.openDevTools({mode:'detach'});
  console.log('start')
  mainWindow.loadURL(winURL)
  mainWindow.removeMenu()
  mainWindow.on('closed', () => {
    console.log('main-closed')
    mainWindow = null
  })
 
  mainWindow.on('close', () => {
   console.log('main-close')
    if(trade){
      request({
        url: 'access/logoutClient', 
      })
      trade.shouldReconnect = false
      if(trade.logout){
        trade.logout();
      }
     
      infoLog(`${trade.m_UserId} 登出`)

    }
    if(childwin){
      childwin.close();
    }
    if(subscribeIndicatorWin){
      subscribeIndicatorWin.close();
    }
    
    clearInterval(Maincycle);
    closeALLData();
    closeALLsubs();
    
  })
}

ipcMain.on('resize-main', (evnt, {width, height}) => {
  mainWindow.setSize(width, height)
  mainWindow.setMenu(meun(false, true));
})
ipcMain.on('close-main', (event, arg) => {
  if(arg){
    errorLog(`关闭，关闭原因${arg}未收到`)
  }
  app.quit();
})
let opedwindow = [];
function findedopened(insId){
  const win = opedwindow.find(({id}) => id === insId);
  return win;
}
function subscribeInstrument(id){
  
  for(let i =0; i< tcp_client_list.length; i++){
    const _tcp = tcp_client_list[i];
    const {instrumentIDs} = _tcp;
    if(instrumentIDs.includes(id)){
      console.log(_tcp.args.port)
      _tcp.addinstrument(id)
      break;
    }
  }
}
ipcMain.on('subscribe-instrument', (event, id) => {
  console.log('2313132312132', id)
  STARTTRADE = true
  
  subscribeInstrument(id);
})
ipcMain.on('open-window', (evnt, {id: insId, title, account, width, height, exchangeId, tick, checked, configId, accountIndex, showController = '', accountStatus, volumeMultiple}) => {
  console.log('open-window')
  COLOSEALL = false;
  const hasInsId = opedwindow.find(({id}) => id === insId)
 
  if(hasInsId){
    hasInsId.win.show()
  }else {
    subscribeInstrument(insId)
    const childwin = new BrowserWindow({
      height,
      useContentSize: true,
      width,
      // parent: mainWindow,
      title: title,
      webPreferences: {
        nodeIntegration: true,
        contextIsolation: false,
        webSecurity: false
      }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}&account=${account}&exchangeId=${exchangeId}&tick=${tick}&configId=${configId}&accountIndex=${accountIndex}&showController=${showController}&accountStatus=${accountStatus}&volumeMultiple=${volumeMultiple}`)
    childwin.on('close', function(){
      if(COLOSEALL) return;
     
      const index = opedwindow.findIndex(({id}) => id === insId);
      opedwindow.splice(index, 1);
      evnt.sender.send(`remove-window`, insId)
      
    })
    childwin.on('focus', function(){
     
      const index = opedwindow.findIndex(({id}) => id === insId);
      if(index > -1){
        evnt.sender.send('change-ins', insId)
      }
    })
    childwin.setMenu(meun(checked))
  
    childwin.setAlwaysOnTop(checked, 'screen-saver')
    opedwindow.push({
      id: insId,
      win: childwin
    })
  }
})

// let noti1, noti2, noti3
// let notiList = []
// ipcMain.on('instrument-notification', (event, ins, text)=>{
//     if(noti1 && noti2 && noti3){
//       notiList.push([event, ins, text])
//       return
//     }
//     showNotification(event, ins, text)
// })
// function showNextNotification(){
//   if(notiList.length){
//     showNotification.apply(undefined, notiList.shift())
//   }
// }
// function showNotification(event, ins, text){
//   if(!noti1){
//     console.log(111111111111111)
//     noti1 = new Notification({
//       title: '通知',
//       body:  text,  
//     })
//     noti1.on('click', function(){
//       console.log('click 11111111')
//       noti1 = null;
//       showNextNotification()
//       event.sender.send('open-noti-ins', ins)
//     })
//     noti1.on('close', function(){
//       console.log('close  noti1')
//       noti1 = null;
//       showNextNotification()
//     })
//     noti1.show();
//     return
//   }
//   if(!noti2){
//     console.log(222222222222)
//     noti2 = new Notification({
//       title: '通知',
//       body:  text,  
//     })
//     noti2.on('click', function(){
//       console.log('click 22222222222')
//       noti2 = null;
//       showNextNotification()
//       event.sender.send('open-noti-ins', ins)
//     })
//     noti2.on('close', function(){
//       console.log('close 222222222')
//       noti2 = null;
//       showNextNotification()
//     })
//     noti2.show();
//     return
//   }
//   if(!noti3){
//     console.log(33333333)
//     noti3 = new Notification({
//       title: '通知',
//       body:  text,  
//     })
//     noti3.on('click', function(){
//       console.log('click 33333333')
//       noti3 = null;
//       showNextNotification()
//       event.sender.send('open-noti-ins', ins)
//     })
//     noti3.on('close', function(){
//       console.log('close 33333333' )
//       noti3 = null;
//       showNextNotification()
//     })
//     noti3.show();
//     return
//   }
  
// }
ipcMain.on('change-title', (event, {id, title})=>{
  const win = findedopened(id);
  win.win.setTitle(title);
})

// app.on('web-contents-created', (event, contents) => {
//   console.log('web-contents-created', 11111111111111)
//   contents.on('will-navigate', (event, navigationUrl) => {
//     const parsedUrl = new URL(navigationUrl)
//     console.log(parsedUrl.origin, 11111111111111)
//     // if (parsedUrl.origin !== 'https://example.com') {
//     //   event.preventDefault()
//     // }
//   })
// })
app.on('ready', createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
 
 
})

app.on('activate', () => {
  if (mainWindow === null) {
    createWindow()
  }
})
let orderMap = {};
let tradeMap = [];
let positionMap = [];
let rateMap =[];
let catchRate  = new Set()
let InstrumetsData =[];
const PriceData ={};
const historyData = {};
const broadcast_Data = {};
let tcp_client_list = [];
let tcp_reconnct_count = 0;
let tcp_timeout= 3 *60 *1000;
function closeALLsubs(){
  COLOSEALL = true;
  
  opedwindow.forEach(({win}) => win.close())
  opedwindow=[]
  
}
function closeALLData(){
  COLOSEALL = true;
  tcp_client_list.forEach(e => e.destroy());
  tcp_client_list = []
}
//为了给不同的页面注册sender
ipcMain.on('register-event',  (event, args) =>{
  const win =  findedopened(args);
  win.sender = event.sender;
  // for(let key in orderMap){
  //   const value = orderMap[key];
  //   if(value.InstrumentID === args && value.OrderStatus === '3'){
  //     win.sender.send('place-order',value);
  //   }
  // }
  const instrumet = InstrumetsData.find(({instrumentID}) => instrumentID===args);
  win.sender.send(`receive-history`, historyData[args])
  win.sender.send('instrumet-data',instrumet);
  win.sender.send('total-order',orderMap);
  win.sender.send('receive-broadcast',broadcast_Data[args])
  if(DicatorMap[args]){
    win.sender.send('broadcast-indicator', DicatorMap[args][0], DicatorMap[args][1]);
  }
 
  positionMap.concat(tradeMap).forEach(field=>{
    if(field.InstrumentID === args){
      win.sender.send('trade-order',field, true);
    }
  })
})
ipcMain.on('update-instrumentsData',  (event, args, init) =>{
  InstrumetsData = args;
  if(init){
    InstrumetsData.forEach(instrumet =>{
      const {instrumentID} = instrumet;
      const win =  findedopened(instrumentID);
      if(win && win.sender){
        win.sender.send('instrumet-data',instrumet);
      }
    })
  }

})

//停止订阅
ipcMain.on('close-all-sub',  (event, args) =>{
  console.log('stop-subscrible')

  closeALLsubs();
  
})
ipcMain.on('stop-subscrible',  (event, args) =>{
  console.log('stop-subscrible')
 
  closeALLData();
  
})
//交易相关
function getUnCatchCommission(args){
 
  const {InstrumentID: instrumentID } = args;
  // console.log('commssion--------------', args)
  if(!catchRate.has(instrumentID)){
    catchRate.add(instrumentID)
    trade.chainSend('reqQryInstrumentCommissionRate', trade.m_BrokerId, trade.m_InvestorId,instrumentID);
  }
  
  
}

let connectcount = 0;
ipcMain.on('trade-login', (event, args) => {
  orderMap = {};
  tradeMap = [];
  positionMap = [];
  rateMap =[];
  let TRADETIME = setTimeout(() => {
    
    event.sender.send('receive-trade', tradeMap);
    event.sender.send('finish-loading', 'trade')
    TRADETIME= null;
  }, 5000)
  let ORDERTIME =  setTimeout(() => {
    event.sender.send('finish-loading', 'order')
  }, 5000);
  // if(trade){
  //   rateMap = [];
  //   catchRate = new Set();
  //   trade.init(args);
  
  //   connectcount = 0;
  //   STARTTRADE= false;
  //   trade.logout();
  //   trade.shouldReconnect = true;
  //   opedwindow.forEach(({sender}) => sender.send('clear-trader'))
  //   return

  // }
  if(args.puppet){
    trade = new OnlineTrade(args);
  }else{
    trade = new Trade(args);
  }
 

  function getorderKey(obj){
 
    const {FrontID, SessionID,  OrderRef} = obj;
    const frontId = FrontID.toString();
    const sessionId = SessionID.toString();
    const orderRef = OrderRef;
    return frontId + sessionId + orderRef;
  }

  
  trade.on('rtnTrade', function(field){

    
    // infoLog(JSON.stringify(field));
    const { Volume, Price, InstrumentID} = field
    // if(args.instruments.includes(InstrumentID)){
      infoLog(JSON.stringify(field));
    // }
    const win = findedopened(InstrumentID);
    tradeMap.push(field);
    getUnCatchCommission(field)

    if(STARTTRADE){
      // const not =new Notification({
      //   title: `${field.InstrumentID} ${Price}  成交 ${Volume}手`,
      //   silent: false
      // })
      // not.show();
      // setTimeout(()=> {
      //   not.close();
      // }, 2000)
      if(win  && win.sender){
        win.sender.send('trade-order', field)
      }
      event.sender.send('receive-trade', field);
    }else{
     
    
      clearTimeout(TRADETIME)
      TRADETIME = setTimeout(() => {
        console.log(123456)
        event.sender.send('receive-trade', tradeMap);
        event.sender.send('finish-loading', 'trade')
        console.log('connectcount', connectcount)
        //防止重连时候成交 要重新更新下
        opedwindow.forEach(({sender, id}) => {
          sender.send('clear-trader')
          positionMap.concat(tradeMap).forEach(field=>{
            if(field.InstrumentID === id){
              sender.send('trade-order',field, true);
            }
          })
        })
        if(connectcount > 1 && !ORDERTIME){
          STARTTRADE = true;
        }
        TRADETIME= null;
      }, 5000)
      return
    }
   
   
   
    
  })
  
  
  trade.on('rtnOrder', function(field){
    
    const key = getorderKey(field);
    
    const needUpdate = !!orderMap[key];
    const old = orderMap[key] || {}
    // const orderStatus = old.OrderStatus;
    // if(args.instruments.includes(field.InstrumentID)){
      // infoLog(JSON.stringify(field));
    // }
    //保证强平撤单
    if(LOCK && field.OrderStatus === '5' ){
      infoLog(`强平撤单${JSON.stringify(field)}`);
      Mainemitter.emit('closeall')
    }
    //返回可能不按时序 先返回已完成的后返回中间状态，所以一旦订单已经完成就要将中间状态舍弃
    if(old.OrderStatus && !field.OrderSysID.startsWith('TJBD_')){
      const status = old.OrderStatus;
      if(['5', '0', '2'].includes(status)){
        return
      }
    }
    orderMap[key] = Object.assign(old, field);
    // let send = false;  
    // console.log(field)
    // switch(field.OrderStatus){
    //   case 'a':
    //     // event.sender.send('receive-order', orderMap[key])
    //     break
    //   case "3":
    //     if(orderStatus !== '3'){
    //       send = true;
    //       orderMap[key].volume = field.VolumeTotalOriginal
    //     }
    //     break
    //   case "5":
    //     send = true;
    //     orderMap[key].volume = -field.VolumeTotalOriginal + field.VolumeTraded;
    //     break;
    //   case "0":
      
    //     if(orderStatus !== 'a'){
    //       orderMap[key].volume =  - field.VolumeTraded
    //       send = true
    //     }
    //     break
    //   case "1":
    //     send = true
    //     if(orderStatus !== 'a'){
    //       orderMap[key].volume =  - field.VolumeTraded
          
    //     }else {
    //       orderMap[key].volume= field.VolumeTotalOriginal -  field.VolumeTraded
    //     }
    //     break
    //   default:

    // }
    // if(send){
      const {InstrumentID } = field; 
      const win = findedopened(InstrumentID);
      // console.log(InstrumentID)
      if(win && win.sender){ 
        // console.log(InstrumentID)
        // win.sender.send('place-order', orderMap[key]);
        if(STARTTRADE && field.OrderStatus === '5' && field.OrderSubmitStatus==='4'){
          console.log(field)
          win.sender.send('order-error',field.StatusMsg);
        }
        // setTimeout(()=>
        // win.sender.send('total-order',orderMap, field), 500
        // )
        win.sender.send('total-order',orderMap, field);
      }
    // }
    
    if(!STARTTRADE){
      clearTimeout(ORDERTIME)
      ORDERTIME = setTimeout(() => {
        event.sender.send('receive-order', orderMap);
        opedwindow.forEach(({sender}) => sender.send('total-order',orderMap))
        if(connectcount > 1 && !TRADETIME){
          STARTTRADE = true;
        }
        ORDERTIME = null;
      }, 5000)
      return
    }else{
      infoLog(`${field.OrderStatus} ${field.StatusMsg} ${+new Date() - flagTime}`)
    }
   
    event.sender.send('receive-order', orderMap[key], key, needUpdate);
  })
 
  let send = false;
  trade.chainOn('rqInvestorPositionDetail', 'reqQryInvestorPositionDetail',function (isLast,field) {
    const { LastSettlementPrice, OpenDate, TradingDay} = field;
    // console.log(isLast, field,send, 'ssssssssssssssssss')
    if( !isLast && !send){
      event.sender.send('add-loading', 'position')
      send = true;
    }
  
    if(OpenDate !==TradingDay) {
   
      field.Price = LastSettlementPrice;
      field.Volume = field.Volume + field.CloseVolume;
      positionMap.push(field);
    }
    if(isLast){
      // console.log(positionMap.map(e => e.InstrumentID) , '111111111111111111111111111111111111111')
      if(connectcount > 1 && positionMap.length){
        opedwindow.forEach(({sender, id}) => {
          sender.send('clear-trader')
          positionMap.forEach(field=>{
            if(field.InstrumentID === id){
              sender.send('trade-order',field, true);
            }
          })
        })
      }
      send = false;
      console.log('position', positionMap.length)
      event.sender.send('receive-position', positionMap);
      event.sender.send('finish-loading', 'position')
    }
  })
  
  // trade.emitterOn('instrument-finish', function (list) {
  //   event.sender.send('receive-instrument', list);
  //   event.sender.send('finish-loading', 'instrument')
    
  // })
  
  trade.chainOn('rqTradingAccount', 'reqQryTradingAccount',function( isLast, field){
    // console.log(field, 'index.js', mainWindow)
    if(mainWindow){
      event.sender.send('receive-account', field);
    }
  })
  trade.emitterOn('connect', function () {
    if(!mainWindow)return;
    infoLog(`${trade.m_UserId}第${connectcount + 1}次链接`)
    console.log('ctp已连接')
    tradeMap = [];
    orderMap = {};
    trade.tasks = [];
    event.sender.send('account-connect', true)
    if(connectcount){
      clearTimeout(TRADETIME);
      clearTimeout(ORDERTIME);
      
        tcp_client_list.forEach(e => {
          if(e.type==='udp'){
            e.destroy()
          }
        });
      
      trade.tasks.push(setTimeout(()=>{
        trade.next()
      }, 1500))
      TRADETIME = setTimeout(() => {
        //隔交易日重连
        positionMap.length = 0
        trade.chainSend('reqQryInvestorPositionDetail', trade.m_BrokerId, trade.m_InvestorId, function(){})
        opedwindow.forEach(({sender}) => sender.send('clear-trader'))
        event.sender.send('finish-loading', 'trade')
        TRADETIME= null;
      }, 5000)
      ORDERTIME = setTimeout(() => {
        opedwindow.forEach(({sender}) => sender.send('total-order',orderMap))
        event.sender.send('receive-order', orderMap);
        trade.confirmTime = false;
        trade.chainSend('reqQrySettlementInfoConfirm',  trade.m_BrokerId, trade.m_InvestorId, function(){})
        ORDERTIME = null
      }, 5000)
    }
   
    STARTTRADE =false;
   
    // opedwindow.forEach(({sender}) => sender.send('clear-trader'))
    event.sender.send('add-loading', 'trade')
    event.sender.send('add-loading', 'order')
    event.sender.send('receive-trade', tradeMap);
  
    connectcount++
  })
  trade.on('disconnected', (...rest) => {
    console.log('disconnected', rest)
    STARTTRADE =false;
    if(mainWindow){
      event.sender.send('account-connect', false)
    }
    errorLog('disconnected');
  })
  // trade.login.then(()=>{
  //   trade.chainSend('reqQryInstrument', '', function (field) {
  //     // console.log('reqQryInstrument is callback');
  //     // console.log(field);
  //   })
  // })
  
  trade.chainOn('rqInstrumentCommissionRate', 'reqQryInstrumentCommissionRate',function (isLast, field) {
    console.log('rqInstrumentCommissionRate is callback');
    console.log("rqInstrumentCommissionRate: isLast", isLast);
    if(field.InstrumentID){
    
      const index = rateMap.findIndex(e => e.InstrumentID === field.InstrumentID);
      if(index === -1){
        rateMap.push(field)
      }else {
        rateMap.splice(index, 1, field)
      }
    };
   
    
    if(isLast){
    
      event.sender.send('receive-rate', rateMap);
      
    }
  }, '');

  trade.emitterOn('error', (msg = {}, skip) =>{
    if(skip && !STARTTRADE) return;
    const win = BrowserWindow.getFocusedWindow();
    const opened = opedwindow.find(e=> e.win === win)
    console.log(msg.ErrorID)
    if(LOCK && msg.ErrorID === 31){
      errorLog("资金不足")
      CFFEXLACK = true;
      console.log('ctp错误' ,msg)
    }
    if(win && opened){
      opened.sender.send('order-error',msg);
    }else {
      event.sender.send('error-msg', {msg});
    }
  })
  const settlementInfo = []
  trade.emitterOn('settlement-info', (islast, info) =>{
   
    if(info){
      settlementInfo.push(info)
    }

    
    if(islast){
      // console.log('111111111111111111111111', settlementInfo)
      if(settlementInfo.length){
        event.sender.send('receive-info', settlementInfo);
        settlementInfo.length = 0;
      }
      
    }
  })
})

// ipcMain.handle('get-pirceTick', async (event, arg) => {
//   const result = await trade.getInstrument(arg)
//   return result
// })
ipcMain.on('puppet-reconnect', ()=>{
  console.log(trade, 'relogin')
  trade.relogin()
})
ipcMain.on('query-SettlementInfo', (event, tradingDay)=>{
  trade.chainOn('rqSettlementInfo', 'reqQrySettlementInfo', function(_,info){
      this.emitter.emit('settlement-info', _,info)
  }, tradingDay);
})
ipcMain.on('confirm-settlement', (event, tradingDay)=>{
  if(trade.confirmTime) return;
  trade.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){
    trade.confirmTime = true;
  })
})


ipcMain.on('trade', (event, args) => {
  STARTTRADE= true;
  if(checkLock(args.instrumentID)) return;
  if(LOCKed){
    event.sender.send('error-msg', {msg:'当前账户已经锁定，无法报单'})
    return
  }
  flagTime = +new Date();
  infoLog(JSON.stringify(args));
  const time = PriceData[args.instrumentID].UpdateTime + `.${PriceData[args.instrumentID].UpdateMillisec}`
  trade.trade(args, time);
})
function findCancelorder(args, title){
  const arr = [];
  function needCancel(order){
    return (order.OrderStatus === '1' || order.OrderStatus === 'a' || order.OrderStatus === '3'|| order.OrderStatus === 'b') && (!args || args.value === order[args.key])
  }
 for(let key in orderMap){
   const item = orderMap[key];
   if(needCancel(item)){
    //  console.log(item, '123133')
     arr.push(item)
   }
 }
 if(arr.length){
  flagTime = +new Date();
  infoLog(`${title}: ${JSON.stringify(arr.map(({InstrumentID, LimitPrice, VolumeTotalOrigina}) => ({InstrumentID, LimitPrice, VolumeTotalOrigina})))}`)
 }
 return arr;
}
ipcMain.on('cancel-order', (event, args) => {
  let value = ''
  if(args.key && args.key === 'InstrumentID'){
    value = args.key;
  }
  if(checkLock(value)) return;
  if(LOCKed){
    event.sender.send('error-msg', {msg:'当前账户已经锁定，无法撤单'})
    return
  }
 const arr = findCancelorder(args, '撤单');
 if(arr.length){
    arr.forEach(e => {
      e.time = PriceData[arr[0].InstrumentID].UpdateTime + `.${PriceData[arr[0].InstrumentID].UpdateMillisec}`
    })
     
 }

//  console.log('cancel',arr)
 trade.cancel(arr);
})
ipcMain.handle('async-cancel-order', (event, args)=>{
  let value = ''
  if(args.key && args.key === 'InstrumentID'){
    value = args.key;
  }
  if(checkLock(value)) return;
  if(LOCKed){
    event.sender.send('error-msg', {msg:'当前账户已经锁定，无法撤单'})
    return
  }
  const arr = findCancelorder(args, '先撤后下');
  if(arr.length){
    arr.forEach(e => {
      e.time = PriceData[arr[0].InstrumentID].UpdateTime + `.${PriceData[arr[0].InstrumentID].UpdateMillisec}`
    })
 }
  return trade.cancel(arr);
})
ipcMain.on('change-lock', (event, locked)=>{
  LOCKed = locked;
})
ipcMain.on('force-close', (event, {over_price = 15, instrumentInfo}, tmpClose) => {
  console.log('触发强平1111111111111111')
  if(LOCK === true) return;
  LOCK = true;
  TMPCLOSE = tmpClose;
  InstrumentIDINFO = instrumentInfo;
  const tarderarr = positionMap.concat(tradeMap);
  let length = tradeMap.length;
  let count = 0;
  const combOffsetFlagMap = {};
  const map = {

  }
  let interval = null;
  let lag = false;
  let lowAndUPerOrderMap ={}; 
  cancel()
  function cancel(){
    lowAndUPerOrderMap = {}
    const arr = findCancelorder('', '撤单')
    .filter(e => {
      const {LimitPrice, InstrumentID, Direction, VolumeTotal, VolumeTraded} = e;
      if(tmpClose && !instrumentInfo.find(e => e.InstrumentID === InstrumentID)){
        return false
      }
      const priceData = PriceData[InstrumentID] || {};
      const {LowerLimitPrice, UpperLimitPrice} = priceData;
      if(Direction ==='0' && LimitPrice >= UpperLimitPrice || (Direction === '1' && LimitPrice <=LowerLimitPrice )){
        const volume = VolumeTotal - VolumeTraded;
        if(!lowAndUPerOrderMap[InstrumentID]){
          lowAndUPerOrderMap[InstrumentID] = [0,0]
        }
       
        lowAndUPerOrderMap[InstrumentID][Direction] += volume
        infoLog(`强平涨跌停挂单 ${JSON.stringify(e)}`)
        return false
      }else {
        return true
      }
     
    });
    
    if(arr.length){
      lag = true
      Mainemitter.once('closeall', closeALL)
    }
    trade.cancel(arr);
  }
  function closeALL(){
    let tradering = []
   
    for(let key in map){
        if(lowAndUPerOrderMap[key]){
          const cancelarr = lowAndUPerOrderMap[key]
          if(cancelarr[0] && map[key].ask.length ){
            map[key].ask.splice(0, cancelarr[0])
          }
          if(cancelarr[1] && map[key].buy.length ){
            map[key].buy.splice(0, cancelarr[1])
          }
        }
        let direction = '1',
        arr = map[key].buy;
        if(map[key].ask.length){
          direction = '0';
          arr=map[key].ask
        }
       
        if(arr.length){
          const _arr = [];
          arr.reduce((a,b) => {
            const c= a.find(e=>e.id === b);
            if(c){
              c.volume ++
            }else{
              a.push({
                instrumentID: key,
                volume: 1,
                id: b,
                direction
              })
            }
            return a
          },  _arr);
        
          tradering= tradering.concat(_arr);
        }
    }
    console.log(tradering, 'tradering')
  
    const tradeList = [];
    tradering.forEach(({instrumentID, volume, direction, id}) => {
        const priceData = PriceData[instrumentID];
        const info = instrumentInfo.find(e => e.InstrumentID === instrumentID);
      
        if(!priceData || !info) return;
        const price = direction === '0'?priceData.BidPrice1 : priceData.AskPrice1;
        let _over_price = over_price;
        if(direction === '1'){
          _over_price = -over_price;
        }
        // if(over_price < priceData.LowerLimitPrice){
        //   over_price = priceData.LowerLimitPrice;
        // }
        // if(over_price > priceData.UpperLimitPrice){
        //   over_price = priceData.UpperLimitPrice;
        // }
        const { PriceTick , ExchangeID} = info;
        let limitPrice = price + _over_price * PriceTick;
        if(limitPrice < priceData.LowerLimitPrice){
          limitPrice = priceData.LowerLimitPrice;
        }
        if(limitPrice > priceData.UpperLimitPrice){
          limitPrice = priceData.UpperLimitPrice;
        }
         const d = (PriceTick.toString().split('.')[1] || []).length;
         console.log(d, '小数点')
         if(d){
          const times = Math.pow(10, d);
          limitPrice =  Math.round(limitPrice * times) / times;
         }
          
      
        let combOffsetFlag = ( specialExchangeId.includes(ExchangeID)  && count)? '3': '1';
       //股指合约要锁仓
        if(ExchangeID === 'CFFEX'&& instrumentID.startsWith('I') && !CFFEXLACK){
          combOffsetFlag = '0';
          for(let key in orderMap){
         
            if(orderMap[key].ExchangeID + orderMap[key].OrderSysID == id){
              combOffsetFlag = orderMap[key].CombOffsetFlag;
              combOffsetFlagMap[id] = combOffsetFlag;
              break;
            }
          }
         
        }
      const item = tradeList.find(e => e.instrumentID === instrumentID && e.combOffsetFlag === combOffsetFlag);
      if(item){
        item.volumeTotalOriginal = item.volumeTotalOriginal+ volume
      }else{
        tradeList.push( {limitPrice, instrumentID, combOffsetFlag, volumeTotalOriginal: volume, direction, ExchangeID});
      }
      
    })
    infoLog(JSON.stringify(tradeList))
    //防止刚好卡主时间点 第一次进来没有需要强平的需要二次确认
    console.log(count, '数字')
    if(!tradeList.length && count){
        event.sender.send('force-close-finish');
        clearInterval(interval);
        console.log('结束')
        infoLog('强平结束')
        setTimeout(()=>  LOCK = false, 3000)
        CFFEXLACK = false
      
    }else{
      infoLog(`开始第${count+1}次强平`);
     
    
      tradeList.forEach(e => trade.trade(e))
      
      if(count> 3){
        LOCK = false;
        clearInterval(interval);
        errorLog('强平次数过多。清手动强平!!!!')
        event.sender.send('error-msg', {msg: '强平次数过多。请联系管理员手动强平!!!!'})
        event.sender.send('force-close-finish');
        CFFEXLACK = false
      }
    }
  }
  function fixMap({Direction, Volume, ExchangeID , OrderSysID, InstrumentID, key}){
    if(!map[InstrumentID]){
      map[InstrumentID] = {
        buy: [],
        ask: []
      }
    }
    let same = 'buy', other='ask';
    if(Direction === '1'){
        same = 'ask';
        other = 'buy';
    }
    while(Volume){
      if(map[InstrumentID][other].length){
        map[InstrumentID][other].shift();
      }else{
        map[InstrumentID][same].push(ExchangeID + OrderSysID)
      }
      Volume--
    }
  }
  tarderarr.forEach(fixMap);
  
  interval = setInterval(function(){
    console.log('进入循环111111111111')
    lag = false
    cancel()
    let arr = [];
    if(tradeMap.length> length){
      arr = tradeMap.slice(length);
      length = tradeMap.length;
    };
    arr.forEach(fixMap)
    count ++;
    if(!lag){
      closeALL()
    }
  }, 1500)
  if(!lag){
    closeALL()
  }
  
})
//行情相关
const decodeMsg = new cppmsg.msg(receiveData['SP'])
const endecodeMsg = new cppmsg.msg(receiveData['GZ'])

function sendParseData(parseData){

     //开盘会有错误数据进入 todo判断正无穷
    //  console.log(parseData)
  if(parseData.OpenPrice >  Number.MAX_SAFE_INTEGER  ||  parseData.LastPrice> Number.MAX_SAFE_INTEGER){
      console.log(parseData.InstrumentID, 'data')
      return
  }
  
  let {InstrumentID} = parseData; 
  //  console.log( InstrumentID, parseData.UpdateTime, new Date())                                               
  // if(InstrumentID.startsWith('LC')){
  //   console.log(InstrumentID)
  //   InstrumentID = 'lc' + InstrumentID.substring(2)
  // }
  if(!historyData[InstrumentID]){
    historyData[InstrumentID] = [parseData]
  }else{
    historyData[InstrumentID].push(parseData)
    if(historyData[InstrumentID].length > 20){
      historyData[InstrumentID].shift()
    }
  }
  PriceData[InstrumentID] = parseData

  const win = findedopened(InstrumentID);
  // console.log(InstrumentID)
  // infoLog(JSON.stringify(parseData))
  if(win && win.sender){ 
    // console.log(InstrumentID, '11111111111111111111111111111111111')
    
      win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
  }
  if(trade.priceData){
    trade.priceData[InstrumentID]  = [
        parseData.BidPrice1,
        parseData.AskPrice1,
        parseData.datetime,
        parseData.LastPrice,  
        parseData.UpdateTime
    ];
    trade.checktrade(parseData);
  }
}
function parseReceiveData(data){
     
  
 
  let parseData;
  
  parseData = decodeMsg.decodeMsg(data);
  // console.log(parseData)
  sendParseData(parseData)
  // console.log(parseData)
  
}
function parseEncodeData(data){
 
  const key = data[0]; 
  for(let i = 1; i<data.length ; i++){
    data[i] = data[i] ^ key;
  }
  
  const parseData = endecodeMsg.decodeMsg(data.slice(1));
  // console.log(parseData)
  sendParseData(parseData)
}

const ReqSubscribeMsg = new cppmsg.msg([
  ['size', 'int32'],
  ['iCmdID', 'int32'],
  ['Stru_ReqSubscribe', 'object', [['RequestID', 'int32'], ['InstrumentID', 'string', '32']]]
])
class TcpClient{
  constructor(args){
    this.args = args;
    this.index = 1;
    this.openInstruments = [];
    this.connectcount = 0

  
  }
  addinstrument(instrument){
    if(!this.openInstruments.includes(instrument)){
      this.openInstruments.push(instrument);
      this.writeinstrument(instrument)
    }
  }
  writeinstrument(instrument){
    this.index ++; 
    const size = this.size;
    const iCmdID = this.iCmdID;
    if(this.tcp_client){
      this.tcp_client.write(ReqSubscribeMsg.encodeMsg2({
        size,
        iCmdID,
        Stru_ReqSubscribe: {
          RequestID: this.index,
          InstrumentID: instrument
        }
      }))
    }
    
  }
  checktype(){
    const {url} = this.args; 
    const _url = url[0].split(':');
    let host= _url[0];
    let port= _url[1];
    let cmd;
    if(['19301', '19299'].includes(port)){
      this.type = 'udp'
    
     
    }else {
      
      this.type = 'tcp'
    }
    console.log(url)
    if(url.length >1 && this.type === 'tcp'){
      return new Promise(resolve => {
        console.log('check start')
        const tcp_client = new net.Socket();
        // tcp_client.setTimeout(2000)
        let timeout =  setTimeout(()=>{
          console.log(host, port, 'timeout')
          const _url = url[1].split(':');
          this.host= _url[0];
          this.port= _url[1];
          tcp_client.destroy()
          resolve()
        }, 3000)
        tcp_client.on('error',  (e) => {
          console.log(e, 'error')
        })
        tcp_client.connect({host, port},()=>{
          console.log(host, 'check')
          this.port = port;
          this.host = host;
          tcp_client.destroy()
          clearTimeout(timeout)
          resolve()
          
        })
      })
    }else{
      this.port = port;
      this.host = host;
      return Promise.resolve()
    }
  }
  connect(){
    let { instrumentIDs,  iCmdID, size = 36, } = this.args
    if(this.connectcount > 5){
      mainWindow.webContents.send('error-msg', {msg:`行情服务${host}:${port} 链接次数过多，请点击强制重连`});
      return;
    }
    this.connectcount ++ 
    
    let tcp_client;
    // host = '127.0.0.1';
    // port = '18899'
    // if(port === '18899'){
    //   host = '111.229.232.221'
    //   port = '18999'
    // }
    
    if(this.type === 'udp'){
      
      console.log(1111)
      tcp_client = new udpClient();
    }else {
     
      tcp_client = new net.Socket()
    }
    const { host , port} = this;
    console.log(tcp_client.bufferSize, '12122')
    this.tcp_client = tcp_client;
    this.instrumentIDs = instrumentIDs;
    this.iCmdID = iCmdID;
    this.size = size;
    tcp_client.setKeepAlive(true, 5*1000);

    tcp_client.connect({host, port},()=>{
     
      this.openInstruments.forEach(this.writeinstrument.bind(this))
    })
  
   
   
    const headMsg = new cppmsg.msg([
      ['size', 'int32'],
      ['CmdID', 'int32'],
    ])
    let cacheArr = [];
    // let time = +new Date();
    tcp_client.on('data',function(data){
      // console.log(111111111111111)
      // console.log(host, port)
      // let _time = +new Date();
      // console.log(`${_time - time}`);
      // console.log(data.length)
      // time = _time;
      // devLog(data)
      if(cacheArr.length){
        cacheArr.push(data);
        const length = cacheArr.reduce((a,b)=> a + b.length, 0);
        // console.log(length)
        data = Buffer.concat(cacheArr, length);
        cacheArr = [];
      }
    
      while(data.length){
        if(data.length < 8){
          cacheArr.push(data)
          return
        } 
        const _head = headMsg.decodeMsg(data.slice(0, 8));
        const {size, CmdID } = _head; 
        // console.log(1111, size, CmdID, data.length)
        if(data.length < size + 8){
      
          cacheArr.push(data)
          return
        }
    
        const parseData = data.slice(8, size+8);
        if(CmdID === 11){
          parseReceiveData(parseData)
        }else if(CmdID === 12){
          parseEncodeData(parseData)
        }
        data = data.slice(size + 8)
        
      }
    
    })
  
    tcp_client.on('end',function(){
      console.log('data end!');
    })
    // tcp_client.setTimeout(tcp_timeout);
    tcp_client.on('timeout',function(){
      
    
        tcp_client.destroy();
       
        console.log('timeout');
        infoLog('timeout')
    })
    tcp_client.on('close',(hadError ) =>{
      console.log('1231231')
      this.tcp_client = null;
      if( mainWindow && !COLOSEALL){
       
        setTimeout(()=> this.connect(), 1000)
      
      }
      infoLog(`data close ${JSON.stringify(hadError)}`);
    })
  
    tcp_client.on('error', function (e) {
   
      mainWindow.webContents.send('error-msg', {msg:`行情服务${host}:${port} 链接错误:${e}。正在重连…………`});
      errorLog(`行情服务${host}:${port} 链接错误:${JSON.stringify(e)}`)
      
    })
  }
  destroy(){
    if(this.tcp_client){
      this.tcp_client.destroy()
    }
   
  }
  changeIns(instruments){
   
    this.args.instrumentIDs = instruments;
    this.instrumentIDs = instruments;
    // this.destroy()
  }
}


ipcMain.on('start-receive', (event, args) =>{
  // STARTTRADE = true;
  let tcp_client = new TcpClient(args);

  if(!Maincycle){
    let taskcount =0
    Maincycle=setInterval(()=>{
     
      const time = new Date().toTimeString().substring(0,8)
      // console.log(time)
      if(tagTime.includes(time)){
        opedwindow.forEach(e => {
          // infoLog(`主进程 ${e.id} ${time}`)
          if(e.sender){
            
            e.sender.send('time-progress', time);
          }
        })
      }
      if(Object.getOwnPropertyNames(PriceData).length!==0){
        const data = {};
        for(let key in PriceData){
          data[key] = [PriceData[key].BidPrice1, PriceData[key].AskPrice1, PriceData[key].LowerLimitPrice ,PriceData[key].UpperLimitPrice, PriceData[key]]
        }
        event.sender.send('receive-price', data)
      }
      //防止网络波动导致一些请求没有返回堵塞请求队列
     console.log(trade.tasks)
      if(trade && trade.tasks.length < 2){
        taskcount = 0
        trade.chainSend('reqQryTradingAccount', trade.m_BrokerId, trade.m_InvestorId, function (params) {
          
        })
      }else{
        taskcount ++;
        if(taskcount > 1.5*60 && trade.tasks.length ){
          trade.tasks = []
          taskcount = 0
        }
      }
    }, 1000)
  }
  tcp_client.checktype().then(()=>{
    console.log('check-client finish')
    event.sender.send('check-client')
    tcp_client.connect();
  })
 
  tcp_client_list.push(tcp_client)
  // function connect(){
  //   infoLog('data open');
  //   tcp_client_list.push(tcp_client);
  //   tcp_client.setKeepAlive(true, 5*1000);
  //   tcp_client.connect({host, port},function(){
  //     console.log('connected to Server');
      
  //     const subscribe = [['RequestID', 'int32'], ['InstrumentID', 'string', '32']]
  //     const msg = new cppmsg.msg([
  //       ['size', 'int32'],
  //       ['iCmdID', 'int32'],
  //       ['Stru_ReqSubscribe', 'object', subscribe]
  //     ])
  //     instrumentIDs.forEach((InstrumentID, index) => {
       
  //       tcp_client.write(msg.encodeMsg2({
  //         size,
  //         iCmdID,
  //         Stru_ReqSubscribe: {
  //           RequestID: index + 2,
  //           InstrumentID: InstrumentID
  //         }
  //       }))
  //     })
  
  //     console.log('success')
  //   })
  
   
   
  //   const headMsg = new cppmsg.msg([
  //     ['size', 'int32'],
  //     ['CmdID', 'int32'],
  //   ])
  //   let cacheArr = [];
  
  //   tcp_client.on('data',function(data){
  //     // console.log(data)
  //     // console.log(data.length)
  //     cacheArr.push(data);
  //     const length = cacheArr.reduce((a,b)=> a + b.length, 0);
  //     // console.log(length)
  //     data = Buffer.concat(cacheArr, length)
    
  //     cacheArr = [];
  //     while(data.length){
  //       if(data.length < 8){
  //         cacheArr.push(data)
  //         return
  //       } 
  //       const _head = headMsg.decodeMsg(data.slice(0, 8));
  //       const {size, CmdID } = _head;
    
  //       if(data.length < size + 8){
      
  //         cacheArr.push(data)
  //         return
  //       }
    
  //       const parseData = data.slice(8, size+8);
  //       if(CmdID === 11){
  //         parseReceiveData(parseData)
  //       }else if(CmdID === 12){
  //         parseEncodeData(parseData)
  //       }
  //       data = data.slice(size + 8)
        
  //     }
  //     // if(head.CmdID === 12 ){
        
  //     //   if(data.length < 302){
  //     //     console.log(data.length)
  //     //   }
  //     //   while(data.length > 301){
        
  //     //     let _head = headMsg.decodeMsg(data.slice(0, 8));
  //     //     let end = 301;
  //     //     if(_head.size + 8 <end){
  //     //       console.log(_head)
  //     //       return
  //     //     } 
  //     //     const encodeData = data.slice(8, end)
  //     //     parseEncodeData(encodeData)
  //     //     data = data.slice(_head.size + 8)
  //     //     if(data.length < 302 && 0<data.length){
  //     //       console.log(data.length)
  //     //     }
         
          
  //     //   }
  //     //   // console.log(data.length)
        
  //     //   return
  //     // }
      
  //     // // if(cacheArr.length !== 0 && head.CmdID !== 11){
  //     // //   return
  //     // // }
  //     //    //建立缓冲区 解析分片发送数据
  //     // cacheArr.push(data)
  //     // // console.log.apply(console, cacheArr.map(e => e.length));
  //     // const length = cacheArr.reduce((a,b)=> a + b.length, 0);
      
  //     // if(length % 416 === 0){
  //     //   parseReceiveData(Buffer.concat(cacheArr, length));
  //     // }
      
  //     // event.sender.send('receive-tarde-data', decodeMsg.decodeMsg(data.slice(8)))
  //   })
  
  //   tcp_client.on('end',function(){
  //     console.log('data end!');
  //   })
  //   // tcp_client.setTimeout(tcp_timeout);
  //   tcp_client.on('timeout',function(){
      
  //       // const index = tcp_client_list.indexOf(tcp_client);
  //       // if(index > -1){
  //       //   tcp_client_list.splice(index, 1);
  //       // }
  //       tcp_client.destroy();
  //       // tcp_client = new net.Socket();
      
  //       // connect();
  //       console.log('timeout');
  //       infoLog('timeout')
  //   })
  //   tcp_client.on('close',function(hadError ){
  //     console.log('1231231')
  //     if( mainWindow && !COLOSEALL){
        
  //       const index = tcp_client_list.indexOf(tcp_client);
  //       if(index > -1){
  //         tcp_client_list.splice(index, 1);
  //       }
        
  //       tcp_client = new net.Socket();
  //       setTimeout(()=> connect(), 1000)
      
  //     }
  //     infoLog(`data close ${JSON.stringify(hadError)}`);
  //   })
  
  //   tcp_client.on('error', function (e) {
  //     // console.log('3333333333333')
  //     // console.log('tcp_client error!', e);
  //     event.sender.send('error-msg', {msg:`行情服务${host}:${port} 链接错误:${e}。正在重连…………`});
  //     errorLog(`行情服务${host}:${port} 链接错误:${JSON.stringify(e)}`)
  //     // const index = tcp_client_list.indexOf(tcp_client);
  //     // tcp_client_list.splice(index, 1);
  //     // // tcp_client.destroy();
  //     // ipcMain.once('error-msg-connect', function(){
  //     //   tcp_client = new net.Socket();
  //     //   connect();
  //     // })
  //   })
  // }
  

  // 接收数据

  
  // let a =0
  // setInterval(() => {
  //   console.log(a, opedwindow)
  //   opedwindow.forEach(({id, sender}) => {
  //     console.log(`receive-${id}`)
  //     if(sender) {
  //       sender.send(`receive-${id}`, a)
  //     }
      
  //   })
  //   a++
  //  }, 500)
})
//强制重连
ipcMain.on('tcp-reconnect', function(_, tcpinfo){
  tcp_reconnct_count = tcp_client_list.length;
  infoLog(`强制重连 重连${tcp_reconnct_count}个链接`)
  console.log(tcpinfo)
  tcp_client_list.forEach(e=>{
    const tcpData = tcpinfo.find( d => d.exchangeNo === e.args.exchangeNo);
    let p = Promise.resolve();
    console.log(tcpData)
    if(tcpData){
      e.args.url = tcpData.quotAddr.split(';') ;
      p = e.checktype();
    }
    p.then(()=>{
      if(e.connectcount > 5){
        e.connectcount = 0;
        e.connect()
      }else{
        e.destroy();
      }
    })
    
    
  })

})

ipcMain.on('broadcast-openinterest', function(_, arg){
  // console.log(arg);
  const instrument = arg.split(':');
  const win = findedopened(instrument[0]);
  let data = {}
  data[instrument[1]] = parseInt(instrument[2]);
  
  if(broadcast_Data[instrument[0]]){
    data = Object.assign(broadcast_Data[instrument[0]], data)
  }
  
  broadcast_Data[instrument[0]] = data;
  if(win && win.sender){
    win.sender.send('receive-broadcast',data)
  }
  
})
ipcMain.on('update-all-config', function(_, arg){
  // console.log(arg);
  mainWindow.webContents.send('update-config', arg)
  opedwindow.forEach(({sender}) => sender.send('update-config', arg))
 
  const allInstruments = arg.flatMap(e => e.instruments.split(','));

  tcp_client_list.forEach((e) => {
    const {instruments} = e.args;
   
    e.changeIns(allInstruments.filter(a => instruments.includes(a)));

  })

})
ipcMain.on('add-sub-instruments', function(_, ins){
  for(let i =0; i< tcp_client_list.length; i++){
    const args= tcp_client_list[i].args
    if(args.instruments.includes(ins)){
      const arrs = args.instrumentIDs.slice();
      arrs.push(ins)
      tcp_client_list[i].changeIns(arrs);
      break;
    }
  }
})
//模拟交易
ipcMain.on('fake-trade', function(event, {id, orderData, tradeData}){
  tradeMap = tradeData;
  orderData.forEach(e => {
    orderMap[e.key] =e;
  })
  console.log(id)
  trade = new FakeTrade(id, orderData)
  trade.on('trade', function(item){
    event.sender.send('receive-trade', item)
    const win = findedopened(item.InstrumentID);
    tradeMap.push(item)
    // console.log(InstrumentID)
    if(win && win.sender){ 
      win.sender.send('trade-order', item)
    }
  })
  trade.on('sub-trade', function(item){
    const win = findedopened(item.InstrumentID);
    // console.log(InstrumentID)
    if(win && win.sender){ 
      win.sender.send('trade-order', item)
    }
  })
  trade.on('main-trade', function(item){
    event.sender.send('receive-trade', item)
  })
  trade.on('order', function(item){
    
    orderMap[item.key] = item;
    event.sender.send('receive-order', item)
    const win = findedopened(item.InstrumentID);
    console.log(win)
    if(win && win.sender){ 
      win.sender.send('total-order',orderMap, item);
    }

  })
  trade.on('data', function(data){
    // console.log(data)
    sendParseData(data);
  })
  if(!Maincycle){
    Maincycle=setInterval(()=>{
      event.sender.send('receive-price', trade.priceData)
    },1000)
  }
 
})

ipcMain.on('send-fake-trade-msg', function(event, msg){
  console.log(msg)
  trade.send(msg)
})
ipcMain.on('import-trades', function(event,){
  dialog.showOpenDialog(mainWindow, {
    title: '导入交易信息',
    
    filters: [
      { name: 'Excel', extensions: ['xls', 'xlsx'] },
  
    ]
  }).then(result => {
    console.log(result)
    if(result.filePaths){
      const path =result.filePaths[0] 
      const instrument = path.match(/[A-Za-z]+[0-9]+/)
      if(instrument){
        const win = findedopened(instrument[0]);
        if(win && win.sender){ 
          win.sender.send('clear-trader');
        }
        trade.importTrade(instrument[0], path)
      }
   
    }
  }).catch(err => {
    console.log(err)
  })
})
// ipcMain.on('parse-trades', function(event,{instrumentID, filepath}){
 
// })
ipcMain.on('jump-next', function(event, instrumentID){
 
  if(trade.TradeDateMap[instrumentID]){
    console.log('jump', instrumentID)
    event.sender.send('clear-trader');
    const tradeData = trade.TradeDateMap[instrumentID];
    const flag = ++tradeData.flag;
    if(tradeData.data[flag]){
      trade.getTime(instrumentID ,tradeData.data[flag].time);
    }
  }
})
ipcMain.on('jump-time', function(event, {instrumentID, time, index}){
  if(trade.TradeDateMap[instrumentID]){
    console.log('jump', time)
    const tradeData = trade.TradeDateMap[instrumentID];
    
    console.log(index);
   
    tradeData.flag = index;
    
    const win = findedopened(instrumentID);
    if(win && win.sender){ 
      win.sender.send('clear-trader');
      trade.getTime(instrumentID ,time);
    }
  }
})
//指标计算
ipcMain.on('broadcast-indicator', function(event, msg){
  
    msg = msg.split('-')
    console.log(msg)
  const instrumentID = msg[0];
  DicatorMap[instrumentID] = [msg[1], msg[2]];
  const win = findedopened(instrumentID);
  if(win && win.sender){ 
    win.sender.send('broadcast-indicator', msg[1], msg[2]);
  }
})
// ipcMain.on('tarder-login-out', function(){
//   trade.logout();
//   positionMap = [];
//   tradeMap = [];
//   orderMap = [];
//   STARTTRADE = false;
//   trade = null;
// })
/**
 * Auto Updater
 *
 * Uncomment the following code below and install `electron-updater` to
 * support auto updating. Code Signing with a valid certificate is required.
 * https://simulatedgreg.gitbooks.io/electron-vue/content/en/using-electron-builder.html#auto-updating
 */

/*
import { autoUpdater } from 'electron-updater'

autoUpdater.on('update-downloaded', () => {
  autoUpdater.quitAndInstall()
})

app.on('ready', () => {
  if (process.env.NODE_ENV === 'production') autoUpdater.checkForUpdates()
})
 */
