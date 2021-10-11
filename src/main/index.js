import { app, BrowserWindow, ipcMain, ipcRenderer } from 'electron'
import  { receiveData }  from '../ctp/dataStruct'
import '../renderer/store'
import path from 'path';
import net from 'net';
import cppmsg from 'cppmsg';
import { Buffer } from 'buffer';


/**
 * Set `__static` path to static files in production
 * https://simulatedgreg.gitbooks.io/electron-vue/content/en/using-static-assets.html
 */
if (process.env.NODE_ENV !== 'development') {
  global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

let mainWindow;
const winURL = process.env.NODE_ENV === 'development'
  ? `http://localhost:9080`
  : `file://${__dirname}/index.html`

const preloadUrl = process.env.NODE_ENV === 'development'
? path.join('D://my-project','static', 'preload.js')
: `file://${__static}/preload.js`
function createWindow () {
  /**
   * Initial window options
   */

  mainWindow = new BrowserWindow({
    height: 563,
    useContentSize: true,
    width: 1400,
    title: 'Vtp'
    // webPreferences: {
    //   preload: preloadUrl
    // }
  })

  mainWindow.loadURL(winURL)

  mainWindow.on('closed', () => {
    mainWindow = null
  })
}
let opedwindow = []
ipcMain.on('open-window', (evnt, insId) => {
  const hasInsId = opedwindow.find(({id}) => id === insId)
  if(hasInsId){
    hasInsId.win.show()
  }else {
    const childwin = new BrowserWindow({
      height: 300,
      useContentSize: true,
      width: 1300,
      parent: mainWindow,
      title: insId
      // webPreferences: {
      //   preload: preloadUrl
      // }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}`)
    childwin.on('close', function(){
      
      const index = opedwindow.findIndex(({id}) => id === insId);
      const last = opedwindow[index + 1] ||  opedwindow[index - 1] || {}
      opedwindow.splice(index, 1);
      evnt.sender.send(`remove-window`, insId)
      evnt.sender.send('change-ins', last.id)
    })
    childwin.on('focus', function(){
      evnt.sender.send('change-ins', insId)
    })
    opedwindow.push({
      id: insId,
      win: childwin
    })
  }
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
//为了给不同的页面注册sender
ipcMain.on('register-event',  (event, args) =>{
  console.log('register-event',args, opedwindow)
  const win =  opedwindow.find(({id}) => id === args);
  win.sender = event.sender;
  
})
//停止监控
ipcMain.on('stop-subscrible',  (event, args) =>{
  console.log('stop-subscrible')
  opedwindow.forEach(({win}) => win.close())
  opedwindow=[]
  closeALLsubs();
  
})
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
      console.log(parseData)
      const {InstrumentID } = parseData; 
      const win = opedwindow.find(({id}) => InstrumentID === id);
      // console.log(data.length, InstrumentID, UpdateTime)
      if(win && win.sender){ 
        // console.log(InstrumentID, UpdateTime, data.length)  
        
          win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
      }
      flag = flag + 416;
    }
  }
  tcp_client.on('data',function(data){
    if(data.length % 416 === 0){
      parseReceiveData(data);
    }else {
      //建立缓冲区 解析分片发送数据
   
      cacheArr.push(data);
      // console.log.apply(console, cacheArr.map(e => e.length));
      const length = cacheArr.reduce((a,b)=> a + b.length, 0);
      if(length === 9056){
        //删除连接成功的无用报文
        cacheArr =[];
        return;
      }
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
