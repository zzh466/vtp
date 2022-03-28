import { app, BrowserWindow, ipcMain, ipcRenderer, Notification } from 'electron'
import  { receiveData }  from '../ctp/dataStruct';
import { errorLog, infoLog} from './log';

import net from 'net';
import cppmsg, { msg } from 'cppmsg';
import { Buffer } from 'buffer';
import Trade from './trade';
import meun from  './menu';
import  './export';
import  request  from './request';
import '../renderer/store';
import {version,specialExchangeId } from '../renderer/utils/utils'
import console from 'console';


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
  // mainWindow.webContents.openDevTools({mode:'detach'});
  console.log('start')
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
    closeALLData();
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
ipcMain.on('open-window', (evnt, {id: insId, title, account, width, height, exchangeId, tick, checked}) => {
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
        contextIsolation: false,
        webSecurity: false
      }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}&account=${account}&exchangeId=${exchangeId}&tick=${tick}`)
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

ipcMain.on('change-title', (event, {id, title})=>{
  const win = findedopened(id);
  win.win.setTitle(title);
})


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
const catchRate  = new Set()
let InstrumetsData =[];
const PriceData ={};
const broadcast_Data = {};
let tcp_client_list = [];
let tcp_reconnct_count = 0;
function closeALLsubs(){
  COLOSEALL = true;
  
  opedwindow.forEach(({win}) => win.close())
  opedwindow=[]
  
}
function closeALLData(){
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
  win.sender.send(`receive-${args}`, PriceData[args])
  win.sender.send('instrumet-data',instrumet);
  win.sender.send('total-order',orderMap);
  win.sender.send('receive-broadcast',broadcast_Data[args])
  positionMap.concat(tradeMap).forEach(field=>{
    if(field.InstrumentID === args){
      win.sender.send('trade-order',field, true);
    }
  })
})
ipcMain.on('update-instrumentsData',  (event, args) =>{
  InstrumetsData = args;
  // InstrumetsData.forEach(instrumet =>{
  //   const {instrumentID} = instrumet;
  //   const win =  findedopened(instrumentID);
  //   if(win && win.sender){
  //     win.sender.send('instrumet-data',instrumet);
  //   }
  // })
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
  if(!catchRate.has(instrumentID)){
    catchRate.add(instrumentID)
    trade.chainSend('reqQryInstrumentCommissionRate', trade.m_BrokerId, trade.m_InvestorId,instrumentID);
  }
  
  
}


ipcMain.on('trade-login', (event, args) => {
  orderMap = {};
  tradeMap = [];
  positionMap = [];
  rateMap =[];
  trade = new Trade(args);

  function getorderKey(obj){
    const {FrontID, SessionID,  OrderRef} = obj;
    const frontId = FrontID.toString();
    const sessionId = SessionID.toString();
    const orderRef = OrderRef;
    return frontId + sessionId + orderRef;
  }
  let TRADETIME = setTimeout(() => {
    event.sender.send('receive-trade', tradeMap);
    event.sender.send('finish-loading', 'trade')
    TRADETIME= null;
  }, 5000)
  let connectcount = 0;
  trade.on('rtnTrade', function(field){
    console.log('emmit---rtnTrade');
    
    infoLog(JSON.stringify(field));
    const { Volume, Price, InstrumentID} = field
    if(args.instruments.includes(InstrumentID)){
      infoLog(JSON.stringify(field));
    }
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
        if(connectcount > 1 ){
          STARTTRADE = true;
        }
        TRADETIME= null;
      }, 5000)
      return
    }
   
   
   
    
  })
  let ORDERTIME =  setTimeout(() => {
    event.sender.send('finish-loading', 'order')
  }, 5000);
  
  trade.on('rtnOrder', function(field){
    const key = getorderKey(field);
    const needUpdate = !!orderMap[key];
    const old = orderMap[key] || {}
    // const orderStatus = old.OrderStatus;
    if(args.instruments.includes(field.InstrumentID)){
      infoLog(JSON.stringify(field));
    }
    //返回可能不按时序 先返回已完成的后返回中间状态，所以一旦订单已经完成就要将中间状态舍弃
    if(old.OrderStatus){
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
        win.sender.send('total-order',orderMap);
      }
    // }
    console.log('StarSTARTTRADE', STARTTRADE)
    if(!STARTTRADE){
      clearTimeout(ORDERTIME)
      ORDERTIME = setTimeout(() => {
        event.sender.send('receive-order', orderMap);
        event.sender.send('finish-loading', 'order')
        
      }, 5000)
      return
    }
    console.log(orderMap[key])
    event.sender.send('receive-order', orderMap[key], key, needUpdate);
  })
 
  
  trade.chainOn('rqInvestorPositionDetail', 'reqQryInvestorPositionDetail',function (isLast,field) {
    const { LastSettlementPrice, OpenDate, TradingDay} = field;
    event.sender.send('add-loading', 'position')
    if(OpenDate !==TradingDay) {
      field.Price = LastSettlementPrice;
      field.Volume = field.Volume + field.CloseVolume;
      positionMap.push(field);
    };  
    if(isLast){
      console.log('111111111111111111111111111111111111111')
      event.sender.send('receive-position', positionMap);
      event.sender.send('finish-loading', 'position')
    }
  })
  
  // trade.emitterOn('instrument-finish', function (list) {
  //   event.sender.send('receive-instrument', list);
  //   event.sender.send('finish-loading', 'instrument')
    
  // })
  
  trade.chainOn('rqTradingAccount', 'reqQryTradingAccount',function( isLast, field){
    // console.log(field)
    if(mainWindow){
      event.sender.send('receive-account', field);
    }
  })
  trade.emitterOn('connect', function () {
    infoLog('行情已连接')
    tradeMap = [];
    trade.tasks = [];
    if(connectcount){
   
      trade.tasks.push(setTimeout(()=>{
        trade.next()
    }, 1500))
    }
   
    STARTTRADE =false;
    event.sender.send('add-loading', 'trade')
    event.sender.send('receive-trade', tradeMap);
    connectcount++
  })
  trade.on('disconnected', (...rest) => {
    console.log('disconnected', rest)
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

  trade.emitterOn('error', (msg, skip) =>{
    if(skip && !STARTTRADE) return;
    const win = BrowserWindow.getFocusedWindow();
    const opened = opedwindow.find(e=> e.win === win)
    
    if(win && opened){
      opened.sender.send('order-error',msg);
    }else {
      event.sender.send('error-msg', {msg});
    }
  })
  const settlementInfo = []
  trade.emitterOn('settlement-info', (islast, info) =>{
    // console.log(info, islast, '22222222222222222222222222222')
    if(info){
      settlementInfo.push(info)
    }

    
    if(islast){
      // console.log('111111111111111111111111', settlementInfo)
      if(settlementInfo.length){
        event.sender.send('receive-info', settlementInfo);
      }else{
        trade.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){})
      }
      
    }
  })
})

// ipcMain.handle('get-pirceTick', async (event, arg) => {
//   const result = await trade.getInstrument(arg)
//   return result
// })
ipcMain.on('confirm-settlement', (event)=>{
  trade.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){
    
  })
})
ipcMain.on('trade', (event, args) => {
  STARTTRADE= true;
 
  infoLog(JSON.stringify(args));
  trade.trade(args);
})
function findCancelorder(args, title){
  const arr = [];
  function needCancel(order){
    return (order.OrderStatus === '1' || order.OrderStatus === 'a' || order.OrderStatus === '3') && (!args || args.value === order[args.key])
  }
 for(let key in orderMap){
   const item = orderMap[key];
   if(needCancel(item)){
     console.log(item, '123133')
     arr.push(item)
   }
 }
 if(arr.length){
  infoLog(`${title}: ${JSON.stringify(arr.map(({InstrumentID, LimitPrice, VolumeTotalOrigina}) => ({InstrumentID, LimitPrice, VolumeTotalOrigina})))}`)
 }
 return arr;
}
ipcMain.on('cancel-order', (event, args) => {
 const arr = findCancelorder(args, '撤单');

 console.log('cancel',arr)
 trade.cancel(arr);
})
ipcMain.handle('async-cancel-order', (event, args)=>{
  const arr = findCancelorder(args, '先撤后下');
  return trade.cancel(arr);
})
ipcMain.on('force-close', (event, {over_price = 15, instrumentInfo}) => {

  const tarderarr = positionMap.concat(tradeMap);
  let length = tradeMap.length;
  let count = 0;
  const combOffsetFlagMap = {};
  const map = {

  }
  let interval = null;
  cancel()
  function cancel(){
    const arr = findCancelorder('', '撤单');
    trade.cancel(arr);
  }
  function closeALL(){
    let tradering = []
    console.log(map);

    for(let key in map){
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
          }, _arr);
        
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
        let combOffsetFlag = ( specialExchangeId.includes(ExchangeID)  && combOffsetFlagMap[id]!=='1')? '3': '1';
        if(!count){
          combOffsetFlag = '0';
          for(let key in orderMap){
         
            if(orderMap[key].ExchangeID + orderMap[key].OrderSysID == id){
              combOffsetFlag = orderMap[key].CombOffsetFlag;
              combOffsetFlagMap[id] = combOffsetFlag;
              break;
            }
          }
         
        }
      tradeList.push( {limitPrice, instrumentID, combOffsetFlag, volumeTotalOriginal: volume, direction, ExchangeID});
    })
    console.log(tradeList, 'tradeList')
    if(!tradeList.length){
        event.sender.send('force-close-finish');
        clearInterval(interval);
        console.log('结束')
        infoLog('强平结束')
        
    }else{
      infoLog(`开始第${count+1}次强平`);
      if(count){
        setTimeout(()=>{
          tradeList.forEach(e => trade.trade(e))
        }, 100)
      }else{
        tradeList.forEach(e => trade.trade(e))
      }
    }
  }
  function fixMap({Direction, Volume, ExchangeID , OrderSysID, InstrumentID}){
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
  closeALL()
  interval = setInterval(function(){
    cancel()
    let arr = [];
    if(tradeMap.length> length){
      arr = tradeMap.slice(length);
      length = tradeMap.length;
    };
    arr.forEach(fixMap)
    count ++;
    closeALL();
  }, 1500)
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

ipcMain.on('start-receive', (event, args) =>{
  STARTTRADE = true;
  const {host, port, instrumentIDs,  iCmdID, size = 36} = args
  let tcp_client = new net.Socket();
  if(!Maincycle){
    Maincycle=setInterval(()=>{
      // console.log(trade.tasks)
      if( trade.tasks.length < 2){
        trade.chainSend('reqQryTradingAccount', trade.m_BrokerId, trade.m_InvestorId, function (params) {
          
        })
      }
      if(Object.getOwnPropertyNames(PriceData).length!==0){
        const data = {};
        for(let key in PriceData){
          data[key] = [PriceData[key].BidPrice1, PriceData[key].AskPrice1, PriceData[key].LowerLimitPrice ,PriceData[key].UpperLimitPrice]
        }
        event.sender.send('receive-price', data)
      }
    }, 1000)
  }
  connect();
  function connect(){
    infoLog('data open');
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
    tcp_client.setTimeout(3 *60 * 1000);
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
      console.log('1231231')
      if((hadError || tcp_reconnct_count) && mainWindow && !COLOSEALL){
        
        const index = tcp_client_list.indexOf(tcp_client);
        if(index > -1){
          tcp_client_list.splice(index, 1);
        }
        if(tcp_reconnct_count){
          tcp_reconnct_count--;
        }
        tcp_client = new net.Socket();
        setTimeout(()=> connect(), 1000)
      
      }
      infoLog('data close', hadError);
    })
  
    tcp_client.on('error', function (e) {
      // console.log('3333333333333')
      // console.log('tcp_client error!', e);
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
//强制重连
ipcMain.on('tcp-reconnect', function(){
  tcp_reconnct_count = tcp_client_list.length;
  tcp_client_list.forEach(e=>{
    e.destroy();
  })
  tcp_client_list= [];
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
