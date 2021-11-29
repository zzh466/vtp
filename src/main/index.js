import { app, BrowserWindow, ipcMain, Notification } from 'electron'
import  { receiveData }  from '../ctp/dataStruct';
import { errorLog, infoLog} from './log';

import net from 'net';
import cppmsg, { msg } from 'cppmsg';
import { Buffer } from 'buffer';
import Trade from './trade';
import './menu';
import './requeset';
import '../renderer/store';
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
    title: 'Vtp',
   
    webPreferences: {
      webSecurity: false,
      nodeIntegration: true,
      contextIsolation: false
    }
  })

  mainWindow.loadURL(winURL)

  mainWindow.on('closed', () => {
  
    mainWindow = null
  })
  mainWindow.on('close', () => {
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
function closeALLsubs(){
  COLOSEALL = true;
  opedwindow.forEach(({win}) => win.close())
  opedwindow=[]
  tcp_client_list.forEach(e => e.destroy());
  tcp_client_list = []
}
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
let Maincycle
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
    const orderStatus = old.OrderStatus;

    orderMap[key] = Object.assign(old, field);
    let send = false;
    // console.log(field)
    switch(field.OrderStatus){
      case 'a':
        // event.sender.send('receive-order', orderMap[key])
        break
      case "3":
        if(orderStatus !== '3'){
          send = true;
          orderMap[key].volume = field.VolumeTotalOriginal
        }
        break
      case "5":
        send = true;
        orderMap[key].volume = -field.VolumeTotalOriginal + field.VolumeTraded;
        break;
      case "0":
      case "1":
        if(orderStatus !== 'a'){
          orderMap[key].volume =  - field.VolumeTraded
          send = true
        }
        break
      default:

    }
    if(send){
      const {InstrumentID } = field; 
      const win = findedopened(InstrumentID);
      // console.log(InstrumentID)
      if(win && win.sender){ 
        console.log(InstrumentID)
        win.sender.send('place-order', orderMap[key]);
        win.sender.send('total-order',orderMap);
      }
    }
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
    event.sender.send('receive-account', field);
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
 infoLog('撤单')
 trade.cancel(arr);
})

//行情相关
const decodeMsg = new cppmsg.msg(receiveData['SP'])
const endecodeMsg = new cppmsg.msg(receiveData['GZ'])

ipcMain.on('start-receive', (event, args) =>{
  const {host, port, instrumentIDs,  iCmdID, size = 36} = args
  let tcp_client = new net.Socket();
  
  connect();
  function connect(){
    tcp_client_list.push(tcp_client);
    tcp_client.setKeepAlive(true);
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
  
   
    const decodeKey = new cppmsg.msg([
      ['key', 'uint16'],
    ])
    const headMsg = new cppmsg.msg([
      ['size', 'int32'],
      ['CmdID', 'int32'],
    ])
    let cacheArr = [];
    function parseReceiveData(data){
      let flag = 0 
      while(flag < data.length){
        let parseData;
        
        parseData = decodeMsg.decodeMsg(data.slice(flag + 8));
        // console.log(parseData)
        const {InstrumentID} = parseData; 
        PriceData[InstrumentID] = parseData
        const win = findedopened(InstrumentID);
        // console.log(InstrumentID)
        if(win && win.sender){ 
          // console.log(InstrumentID, '11111111111111111111111111111111111')
          
            win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
        }
        flag = flag + 416;
      }
    }
    function parseEncodeData(data, size){
     
      
      const parseData = endecodeMsg.decodeMsg(data);
      // console.log(parseData)
      const {InstrumentID,BidPrice1, AskPrice1 } = parseData; 
      PriceData[InstrumentID] = [BidPrice1, AskPrice1 ]
      const win = findedopened(InstrumentID);
      // console.log(InstrumentID)
      if(win && win.sender){ 
        // console.log(InstrumentID, '11111111111111111111111111111111111')
        
          win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
      }
    }
    const HasDATA = false
    tcp_client.on('data',function(data){
      if(data.length % 416 === 0){
        parseReceiveData(data);
      }else {
        //建立缓冲区 解析分片发送数据
        const head  = headMsg.decodeMsg(data.slice(0, 8));
        // console.log(head)
        if(head.CmdID === 201){
          return
        }
        if(head.CmdID === 11 ){
          cacheArr =[];
        }
        if(head.CmdID === 12 ){
  
          const key = data[8]; 
          for(let i = 9; i<data.length -1; i++){
            data[i] = data[i] ^ key;
          }
          // console.log(data.length)
          parseEncodeData(data.slice(9), 297)
          return
        }
        
        // if(cacheArr.length !== 0 && head.CmdID !== 11){
        //   return
        // }
        cacheArr.push(data)
        // console.log.apply(console, cacheArr.map(e => e.length));
        const length = cacheArr.reduce((a,b)=> a + b.length, 0);
       
        if(length % 416 === 0){
          parseReceiveData(Buffer.concat(cacheArr, length));
          cacheArr =[]
        }
      }
      // event.sender.send('receive-tarde-data', decodeMsg.decodeMsg(data.slice(8)))
    })
  
    tcp_client.on('end',function(){
      console.log('data end!');
    })
  
    tcp_client.on('error', function (e) {
      console.log('tcp_client error!', e);
      event.sender.send('error-msg', {msg:`行情服务${host}:${port} 链接错误:${e}。正在重连…………`, code: 'connect'});
      const index = tcp_client_list.indexOf(tcp_client);
      tcp_client_list.splice(index, 1);
      // tcp_client.destroy();
      ipcMain.once('error-msg-connect', function(){
        tcp_client = new net.Socket();
        connect();
      })
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
  console.log(arg);
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
