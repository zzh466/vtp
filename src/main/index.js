import { app, BrowserWindow, ipcMain, ipcRenderer } from 'electron'
import  { receiveData }  from '../ctp/dataStruct'
import '../renderer/store'
import path from 'path';
import net from 'net';
import cppmsg from 'cppmsg';



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
    width: 1000,
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
const opedwindow = []
ipcMain.on('open-window', (evnt, insId) => {
  const hasInsId = opedwindow.find(({id}) => id === insId)
  if(hasInsId){
    hasInsId.win.show()
  }else {
    const childwin = new BrowserWindow({
      height: 300,
      useContentSize: true,
      width: 1200,
      parent: mainWindow,
      title: insId
      // webPreferences: {
      //   preload: preloadUrl
      // }
    })
    childwin.loadURL(`${winURL}#price?id=${insId}`)
    childwin.on('close', function(){
      
      const index = opedwindow.findIndex(({id}) => id === insId);
      opedwindow.splice(index, 1);
      evnt.sender.send(`remove-window`, insId)
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


let tcp_client = null;
app.on('ready', createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
  if(tcp_client) {
    tcp_client.destroy()
  }
})

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
ipcMain.on('start-receive', (event, args) =>{
  const {host, port, instrumentIDs } = args
  tcp_client = new net.Socket();
  tcp_client.connect({host, port},function(){
    console.log('connected to Server');
    
    const subscribe = [['RequestID', 'int32'], ['InstrumentID', 'string', '32']]
    const msg = new cppmsg.msg([
      ['size', 'int32'],
      ['iCmdID', 'int32'],
      ['Stru_ReqSubscribe', 'object', subscribe]
    ])
    instrumentIDs.forEach((InstrumentID, index) => {
      console.log(instrumentIDs, InstrumentID)
      tcp_client.write(msg.encodeMsg2({
        size: 36,
        iCmdID: 101,
        Stru_ReqSubscribe: {
          RequestID: index + 2,
          InstrumentID: InstrumentID
        }
      }))
    });
    

    console.log('success')
  })

  // 接收数据

  const decodeMsg = new cppmsg.msg(receiveData)
  
  tcp_client.on('data',function(data){
   
    if(data.length % 416 === 0){
        
        let flag = 0;
       
      
        while(flag < data.length){
          const parseData = decodeMsg.decodeMsg(data.slice(flag + 8));
          const {InstrumentID, UpdateTime } = parseData; 
        
         
          const win = opedwindow.find(({id}) => InstrumentID === id);
          // console.log(data.length, InstrumentID, UpdateTime)
          if(win && win.sender){ 
            // console.log(InstrumentID, UpdateTime, data.length)  
            console.log('eee----',  InstrumentID, UpdateTime)
              win.sender.send(`receive-${parseData.InstrumentID}`, parseData)
          }
          flag = flag + 416;
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
