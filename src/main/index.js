import { app, BrowserWindow, ipcMain, Notification } from 'electron'
import  { receiveData }  from '../ctp/dataStruct'
import '../renderer/store'

import net from 'net';
import cppmsg, { msg } from 'cppmsg';
import { Buffer } from 'buffer';
import Trade from './trade';
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
    height: 300,
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
  if (process.env.NODE_ENV !== 'development') {
    mainWindow.removeMenu()
  }
}

ipcMain.on('resize-main', (evnt, {width, height}) => {
  mainWindow.setSize(width, height)
})
ipcMain.on('close-main', () => {
  app.quit();
})
let opedwindow = [];
function findedopened(insId){
  const win = opedwindow.find(({id}) => id === insId);
  return win;
}
ipcMain.on('open-window', (evnt, {id: insId, title}) => {
  COLOSEALL = false;
  const hasInsId = opedwindow.find(({id}) => id === insId)
 
  if(hasInsId){
    hasInsId.win.show()
  }else {
    const childwin = new BrowserWindow({
      height: 300,
      useContentSize: true,
      width: 1500,
      parent: mainWindow,
      title: title,
      webPreferences: {
        nodeIntegration: true,
        contextIsolation: false
      }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}`)
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
    if (process.env.NODE_ENV !== 'development') {
      childwin.removeMenu()
    }
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
  closeALLsubs()
 
})
function closeALLsubs(){
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
  win.sender.send('instrumet-data',instrumet);
  win.sender.send('total-order',orderMap);
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
    if(win){
      win.sender.send('instrumet-data',instrumet);
    }
  })
})

//停止订阅
ipcMain.on('stop-subscrible',  (event, args) =>{
  console.log('stop-subscrible')
  COLOSEALL = true;
  opedwindow.forEach(({win}) => win.close())
 
  opedwindow=[]
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
    STARTTRADE = false;
    return
  }
  trade = new Trade(args);
 
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
    console.log('emmit---rtnTrade', field);
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
        orderMap[key].volume = -field.VolumeTotalOriginal;
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
      }, 2000)
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
      event.sender.send('receive-trade', tradeMap);
    }
  })
  trade.chainOn('rqInstrumentCommissionRate', 'reqQryInstrumentCommissionRate',function (isLast, field) {
    console.log('rqInstrumentCommissionRate is callback');
    console.log("rqInstrumentCommissionRate: isLast", isLast);
    rateMap.push(field)
    if(isLast){
      event.sender.send('finish-loading', 'rate')
      event.sender.send('receive-rate', rateMap);
    }
  });

  trade.emitterOn('error', (msg, skip) =>{
    if(skip && !STARTTRADE) return;
    event.sender.send('error-msg', msg);
  })
  const settlementInfo = []
  trade.emitterOn('settlement-info', (islast, info) =>{
    settlementInfo.push(info)
    if(islast){
      console.log('111111111111111111111111', event.sender.send)
      event.sender.send('receive-info', settlementInfo);
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
    // trade.send('reqQryInstrumentCommissionRate', trade.m_BrokerId, trade.m_InvestorId);
  }
  STARTTRADE = true;
  
  trade.trade(args);
})

ipcMain.on('cancel-order', (event, args) => {
  const arr = [];
 for(let key in orderMap){
   const item = orderMap[key];
   if(item.OrderStatus === '3' && !args || args === item.InstrumentID){
    //  console.log(item)
     arr.push(item)
   }
 }
 trade.cancel(arr);
})

//行情相关
const PriceData ={};
let Maincycle = null
ipcMain.on('start-receive', (event, args) =>{
  const {host, port, instrumentIDs, type,  iCmdID, size = 36} = args
  const tcp_client = new net.Socket();
  tcp_client_list.push(tcp_client)
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
  const decodeMsg = new cppmsg.msg(receiveData[type])
  if(!Maincycle){
    Maincycle=setInterval(()=>{
      
      if(Object.getOwnPropertyNames(PriceData).length!==0){
        event.sender.send('receive-price', PriceData)
      }
    }, 1000)
  }
  const decodeKey = new cppmsg.msg([
    ['key', 'uint8'],
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
      const {InstrumentID,BidPrice1, AskPrice1 } = parseData; 
      PriceData[InstrumentID] = [BidPrice1, AskPrice1 ]
      const win = findedopened(InstrumentID);
      // console.log(InstrumentID)
      if(win && win.sender){ 
        // console.log(InstrumentID, '11111111111111111111111111111111111')
        
          win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
      }
      flag = flag + 416;
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

  tcp_client.on('error', function () {
    console.log('tcp_client error!');
  })

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
