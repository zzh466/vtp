import { app, BrowserWindow, ipcMain, Notification } from 'electron'
import  { receiveData }  from '../ctp/dataStruct';
import { errorLog, infoLog} from './log';

import net from 'net';
import cppmsg, { msg } from 'cppmsg';
import { Buffer } from 'buffer';
import Trade from './trade';
import meun from  './menu';
import  request  from './request';
import '../renderer/store';
import {version } from '../renderer/utils/utils'

let COLOSEALL = false;
/**
 * Set `__static` path to static files in production
 * https://simulatedgreg.gitbooks.io/electron-vue/content/en/using-static-assets.html
 */
if (process.env.NODE_ENV !== 'development') {
  global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

let mainWindow;
let trade;
let STARTTRADE = false;
let Maincycle;
const winURL = process.env.NODE_ENV === 'development'
  ? `http://localhost:9080`
  : `file://${__dirname}/index.html`

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
  mainWindow.setMenu(meun(false));
  mainWindow.loadURL(winURL)

  mainWindow.on('closed', () => {
  
    mainWindow = null
  })
  mainWindow.on('close', () => {
   
    if(trade){
      request({
        url: 'access/logoutClient', 
      })
    }
   
    clearInterval(Maincycle);
    closeALLsubs();
  })
}

ipcMain.on('resize-main', (evnt, {width, height}) => {
  mainWindow.setSize(width, height)
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
ipcMain.on('open-window', (evnt, {id: insId, title, account, width, height}) => {
  COLOSEALL = false;
  const hasInsId = opedwindow.find(({id}) => id === insId)
 
  if(hasInsId){
    hasInsId.win.show()
  }else {
    const childwin = new BrowserWindow({
      height,
      useContentSize: true,
      width,
      // parent: mainWindow,
      title: title,
      webPreferences: {
        nodeIntegration: true,
        contextIsolation: false
      }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}&account=${account}`)
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
    childwin.setMenu(meun(true))
  
    childwin.setAlwaysOnTop(true, 'screen-saver')
    opedwindow.push({
      id: insId,
      win: childwin
    })
  }
})

ipcMain.on('change-title', (event, {id, title})=>{
  const win = findedopened(id);
  win.win.setTitle(title);
})

let tcp_client_list = [];
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
const orderMap = {};
const tradeMap = [];
const rateMap =[];
let InstrumetsData =[];
const PriceData ={};
const broadcast_Data = {};

function closeALLsubs(){
  COLOSEALL = true;
  tcp_client_list.forEach(e => e.destroy());
  tcp_client_list = []
  opedwindow.forEach(({win}) => win.close())
  opedwindow=[]
  
}
//为了给不同的页面注册sender
ipcMain.on('register-event',  (event, args) =>{
  const win =  findedopened(args);
  win.sender = event.sender;
  for(let key in orderMap){
    const value = orderMap[key];
    if(value.InstrumentID === args && value.OrderStatus === '3'){
      win.sender.send('place-order',value);
    }
  }
  const instrumet = InstrumetsData.find(({instrumentID}) => instrumentID===args);
  win.sender.send(`receive-${args}`, PriceData[args])
  win.sender.send('instrumet-data',instrumet);
  win.sender.send('total-order',orderMap);
  win.sender.send('receive-broadcast',broadcast_Data[args])
  tradeMap.sort((a, b)=>{
      const date1 = a.OpenDate || a.TradeDate
      const date2 = b.OpenDate || b.TradeDate
      return date1 - date2
  }).forEach(field=>{
    if(field.InstrumentID === args){
      win.sender.send('trade-order',field);
    }
  })

})
ipcMain.on('update-instrumentsData',  (event, args) =>{
  InstrumetsData = args;
  InstrumetsData.forEach(instrumet =>{
    const {instrumentID} = instrumet;
    const win =  findedopened(instrumentID);
    if(win && win.sender){
      win.sender.send('instrumet-data',instrumet);
    }
  })
})

//停止订阅
ipcMain.on('stop-subscrible',  (event, args) =>{
  console.log('stop-subscrible')
  if(args){
    errorLog(args)
  }
  closeALLsubs();
  
})
//交易相关
ipcMain.on('trade-login', (event, args) => {
  if(trade){
    event.sender.send('receive-trade', tradeMap);
    event.sender.send('finish-loading', 'trade')
    event.sender.send('receive-order', orderMap);
    event.sender.send('receive-order', orderMap);
    event.sender.send('receive-rate', rateMap)
    event.sender.send('receive-instrument', trade.getInstrumentList);
    STARTTRADE = false;
    return
  }
  trade = new Trade(args);
  if(!Maincycle){
    Maincycle=setInterval(()=>{
     
      if( trade.tasks.length < 2){
        trade.chainSend('reqQryTradingAccount', trade.m_BrokerId, trade.m_InvestorId, function (params) {
          
        })
      }
      if(Object.getOwnPropertyNames(PriceData).length!==0){
        const data = {};
        for(let key in PriceData){
          data[key] = [PriceData[key].BidPrice1, PriceData[key].AskPrice1 ]
        }
        event.sender.send('receive-price', data)
      }
    }, 1000)
  }
  function getorderKey(obj){
    const {FrontID, SessionID,  OrderRef} = obj;
    const frontId = FrontID.toString();
    const sessionId = SessionID.toString();
    const orderRef = OrderRef.trim();
    return frontId + sessionId + orderRef;
  }
  let TRADETIME = setTimeout(() => {
    event.sender.send('receive-trade', tradeMap);
    event.sender.send('finish-loading', 'trade')
    TRADETIME= null;
  }, 2000)
  trade.on('rtnTrade', function(field){
    console.log('emmit---rtnTrade');
    const { Volume, Price, InstrumentID} = field
    const win = findedopened(InstrumentID);
    tradeMap.push(field);
    if(win  && win.sender){
       win.sender.send('trade-order', field)
    }
    if(STARTTRADE){
      const not =new Notification({
        title: `${field.InstrumentID} ${Price}  成交 ${Volume}手`,
        silent: false
      })
      not.show();
      setTimeout(()=> {
        not.close();
      }, 2000)

    }else{
      clearTimeout(TRADETIME)
      TRADETIME = setTimeout(() => {
        event.sender.send('receive-trade', tradeMap);
        event.sender.send('finish-loading', 'trade')
        TRADETIME= null;
      }, 2000)
      return
    }
   
   
    event.sender.send('receive-trade', tradeMap);
    
  })
  let ORDERTIME =  setTimeout(() => {
    event.sender.send('finish-loading', 'order')
  }, 2000);
  trade.on('rtnOrder', function(field){
    const key = getorderKey(field);
    const old = orderMap[key] || {}
    // const orderStatus = old.OrderStatus;

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
        win.sender.send('total-order',orderMap);
      }
    // }
    // console.log('emmit---rtnOrder', field)
    if(!STARTTRADE){
      clearTimeout(ORDERTIME)
      ORDERTIME = setTimeout(() => {
        event.sender.send('receive-order', orderMap);
        event.sender.send('finish-loading', 'order')
      }, 3000)
      return
    }
    event.sender.send('receive-order', orderMap);
  })
 
  
  trade.chainOn('rqInvestorPositionDetail', 'reqQryInvestorPositionDetail',function (isLast,field) {
    const { LastSettlementPrice, OpenDate, TradingDay} = field;
    if(OpenDate ===TradingDay) return;  
    field.Price = LastSettlementPrice;
    field.Volume = field.Volume + field.CloseVolume;
    tradeMap.push(field);
  
    if(isLast && !TRADETIME){
      console.log('111111111111111111111111111111111111111', event.sender.send)
      event.sender.send('receive-trade', tradeMap);
    }
  })
  trade.chainOn('rqTradingAccount', 'reqQryTradingAccount',function( isLast, field){
    if(mainWindow){
      event.sender.send('receive-account', field);
    }
  })
  trade.emitterOn('instrument-finish', function (list) {
    event.sender.send('receive-instrument', list);
    event.sender.send('finish-loading', 'instrument')
    
  })
  trade.login.then(()=>{
    trade.chainSend('reqQryInstrument', '', function (field) {
      // console.log('reqQryInstrument is callback');
      // console.log(field);
    })
  })
  
  trade.chainOn('rqInstrumentCommissionRate', 'reqQryInstrumentCommissionRate',function (isLast, field) {
    console.log('rqInstrumentCommissionRate is callback');
    console.log("rqInstrumentCommissionRate: isLast", isLast);
    console.log(field)
    if(!rateMap.find(e => e.InstrumentID === field.InstrumentID)){
      rateMap.push(field)
    }
    
    if(isLast){
      event.sender.send('finish-loading', 'rate')
      event.sender.send('receive-rate', rateMap);
      
    }
  }, '');

  trade.emitterOn('error', (msg, skip) =>{
    if(skip && !STARTTRADE) return;
    event.sender.send('error-msg', {msg});
  })
  const settlementInfo = []
  trade.emitterOn('settlement-info', (islast, info) =>{
    console.log(info, islast, '22222222222222222222222222222')
    if(info){
      settlementInfo.push(info)
    }

    
    if(islast){
      console.log('111111111111111111111111', settlementInfo)
      if(settlementInfo.length){
        event.sender.send('receive-info', settlementInfo);
      }else{
        trade.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){})
      }
      
    }
  })
})

ipcMain.handle('get-pirceTick', async (event, arg) => {
  const result = await trade.getInstrument(arg)
  return result
})
ipcMain.on('confirm-settlement', (event)=>{
  trade.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){
    
  })
})
ipcMain.on('trade', (event, args) => {
  const {instrumentID } = args;
  const index = rateMap.findIndex(e => instrumentID.startsWith(e.InstrumentID));
  if(index === -1){
    trade.chainSend('reqQryInstrumentCommissionRate', trade.m_BrokerId, trade.m_InvestorId,instrumentID);
  }
  STARTTRADE = true;
  infoLog(JSON.stringify(args));
  trade.trade(args);
})

ipcMain.on('cancel-order', (event, args) => {
  const arr = [];
  function needCancel(order){
    return (order.OrderStatus === '1' || order.OrderStatus === 'a' || order.OrderStatus === '3') && (!args || args.value === order[args.key])
  }
 for(let key in orderMap){
   const item = orderMap[key];
   if(needCancel(item)){
    //  console.log(item)
     arr.push(item)
   }
 }
 if(arr.length){
  infoLog(`撤单: ${JSON.stringify(arr.map(({InstrumentID, LimitPrice, VolumeTotalOrigina}) => ({InstrumentID, LimitPrice, VolumeTotalOrigina})))}`)
 }
 console.log('cancel', +Date.now())
 trade.cancel(arr);
})

//行情相关
const decodeMsg = new cppmsg.msg(receiveData['SP'])
const endecodeMsg = new cppmsg.msg(receiveData['GZ'])
function sendParseData(parseData){
  const {InstrumentID} = parseData; 
  PriceData[InstrumentID] = parseData
  const win = findedopened(InstrumentID);
  // console.log(InstrumentID)
  if(win && win.sender){ 
    // console.log(InstrumentID, '11111111111111111111111111111111111')
    
      win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
  }
}
function parseReceiveData(data){
     
  
 
  let parseData;
  
  parseData = decodeMsg.decodeMsg(data);
  console.log(parseData)
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

ipcMain.on('start-receive', (event, args) =>{
  const {host, port, instrumentIDs,  iCmdID, size = 36} = args
  let tcp_client = new net.Socket();
  
  connect();
  function connect(){
    tcp_client_list.push(tcp_client);
    // tcp_client.setKeepAlive(true);
    tcp_client.connect({host, port},function(){
      console.log('connected to Server');
      
      const subscribe = [['RequestID', 'int32'], ['InstrumentID', 'string', '32']]
      const msg = new cppmsg.msg([
        ['size', 'int32'],
        ['iCmdID', 'int32'],
        ['Stru_ReqSubscribe', 'object', subscribe]
      ])
      instrumentIDs.forEach((InstrumentID, index) => {
       
        tcp_client.write(msg.encodeMsg2({
          size,
          iCmdID,
          Stru_ReqSubscribe: {
            RequestID: index + 2,
            InstrumentID: InstrumentID
          }
        }))
      })
  
      console.log('success')
    })
  
   
   
    const headMsg = new cppmsg.msg([
      ['size', 'int32'],
      ['CmdID', 'int32'],
    ])
    let cacheArr = [];
  
    tcp_client.on('data',function(data){
      // console.log(data)
      // console.log(data.length)
      cacheArr.push(data);
      const length = cacheArr.reduce((a,b)=> a + b.length, 0);
      // console.log(length)
      data = Buffer.concat(cacheArr, length)
    
      cacheArr = [];
      while(data.length){
        if(data.length < 8){
          cacheArr.push(data)
          return
        } 
        const _head = headMsg.decodeMsg(data.slice(0, 8));
        const {size, CmdID } = _head;
    
        if(data.length < size + 8){
          console.log('222222222222222222222222',data.length, _head)
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
      // if(head.CmdID === 12 ){
        
      //   if(data.length < 302){
      //     console.log(data.length)
      //   }
      //   while(data.length > 301){
        
      //     let _head = headMsg.decodeMsg(data.slice(0, 8));
      //     let end = 301;
      //     if(_head.size + 8 <end){
      //       console.log(_head)
      //       return
      //     } 
      //     const encodeData = data.slice(8, end)
      //     parseEncodeData(encodeData)
      //     data = data.slice(_head.size + 8)
      //     if(data.length < 302 && 0<data.length){
      //       console.log(data.length)
      //     }
         
          
      //   }
      //   // console.log(data.length)
        
      //   return
      // }
      
      // // if(cacheArr.length !== 0 && head.CmdID !== 11){
      // //   return
      // // }
      //    //建立缓冲区 解析分片发送数据
      // cacheArr.push(data)
      // // console.log.apply(console, cacheArr.map(e => e.length));
      // const length = cacheArr.reduce((a,b)=> a + b.length, 0);
      
      // if(length % 416 === 0){
      //   parseReceiveData(Buffer.concat(cacheArr, length));
      // }
      
      // event.sender.send('receive-tarde-data', decodeMsg.decodeMsg(data.slice(8)))
    })
  
    tcp_client.on('end',function(){
      console.log('data end!');
    })
    tcp_client.setTimeout( 3 * 60 * 1000);
    tcp_client.on('timeout',function(){
      
        const index = tcp_client_list.indexOf(tcp_client);
        if(index > -1){
          tcp_client_list.splice(index, 1);
        }
        tcp_client.destroy();
        tcp_client = new net.Socket();
        connect();
        console.log('timeout');
    })
    tcp_client.on('close',function(hadError ){
      if(hadError && mainWindow && !COLOSEALL){
        
        const index = tcp_client_list.indexOf(tcp_client);
        if(index > -1){
          tcp_client_list.splice(index, 1);
        }
        tcp_client = new net.Socket();
        connect();
      }
      console.log('data close', hadError);
    })
  
    tcp_client.on('error', function (e) {
      console.log('tcp_client error!', e);
      event.sender.send('error-msg', {msg:`行情服务${host}:${port} 链接错误:${e}。正在重连…………`});
      errorLog(`行情服务${host}:${port} 链接错误:${JSON.stringify(e)}`)
      // const index = tcp_client_list.indexOf(tcp_client);
      // tcp_client_list.splice(index, 1);
      // // tcp_client.destroy();
      // ipcMain.once('error-msg-connect', function(){
      //   tcp_client = new net.Socket();
      //   connect();
      // })
    })
  }
  

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
ipcMain.on('broadcast-openinterest', function(_, arg){
  // console.log(arg);
  const instrument = arg.split(':');
  const win = findedopened(instrument[0]);
  const data =  {
    direction: instrument[1],
    volume: instrument[2]
  }
  broadcast_Data[instrument[0]] = data;
  if(win && win.sender){
    win.sender.send('receive-broadcast',data)
  }
  
})

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
