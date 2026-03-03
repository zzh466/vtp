import {Menu,BrowserWindow, shell, ipcMain } from 'electron'
import {baseURL, winURL} from '../renderer/utils/utils'
import { logPath } from './log';
import {getConfig, setconfig} from './config'

console.log(logPath)
let childwin = null;
let subscribeIndicatorWin = null;
let color_blindness = getConfig('color_blindness');
export { childwin, subscribeIndicatorWin};
export default function(checked,main){
  // console.log(checked)
  const config = [{
    label: '报测单笔数限制配置',
    click(){
      if(childwin){
        childwin.show();
      }else{
        childwin = new BrowserWindow({
          height: 1000,
          useContentSize: true,
          width: 1000,
          // parent: mainWindow,
          title: '单数限制配置',
          webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            webSecurity: false
          }
        })
        childwin.loadURL(`${winURL}#subscribeIndicator`)
        childwin.removeMenu()
        childwin.on('closed', function(){
          childwin = null;    
        })
      }
      
    }
  },{
    label: '快捷键及合约配置',
    click(){
      if(childwin){
        childwin.show();
      }else{
        subscribeIndicatorWin = new BrowserWindow({
          height: 1000,
          useContentSize: true,
          width: 1000,
          // parent: mainWindow,
          title: '快捷键及合约配置',
          webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            webSecurity: false
          }
        })
        subscribeIndicatorWin.loadURL(`${winURL}#Config`)
        subscribeIndicatorWin.removeMenu()
        subscribeIndicatorWin.on('closed', function(){
          subscribeIndicatorWin = null;    
        })
      }
      
    }
  }]
  if(main){
    config.push({
      label: '色盲模式',
      click(event, window){
         BrowserWindow.getAllWindows().forEach(win => win.webContents.send('change-blindness',  event.checked));
        setconfig('color_blindness', event.checked)
      
      },
      type: 'checkbox',
      color_blindness,
    })
  }
  return Menu.buildFromTemplate([{
          label: '系统',
          submenu: [{
            label: '置顶',
            click(event, window){
              // BrowserWindow.getAllWindows().forEach(win => win.setAlwaysOnTop(false));
              if(event.checked){
                window.setAlwaysOnTop(true, 'screen-saver')
              }else {
                window.setAlwaysOnTop(false)
              }
            
            },
            type: 'checkbox',
            checked,
          },
          {
            label: '日志',
            click(){
              
              shell.openPath(logPath);
            },

          }
        ]
      }
      ,{
        label: '配置',
        submenu: config
        
      
      }
      // ,{
      //   label: '订阅波动合约',
      //   click(){
      //     if(subscribeIndicatorWin){
      //       subscribeIndicatorWin.show();
      //     }else{
      //       subscribeIndicatorWin = new BrowserWindow({
      //         height: 1000,
      //         useContentSize: true,
      //         width: 1000,
      //         // parent: mainWindow,
      //         title: '订阅波动合约',
      //         webPreferences: {
      //           nodeIntegration: true,
      //           contextIsolation: false,
      //           webSecurity: false
      //         }
      //       })
      //       subscribeIndicatorWin.loadURL(`${winURL}#subscribeIndicator`)
      //       subscribeIndicatorWin.removeMenu()
      //       subscribeIndicatorWin.on('closed', function(){
      //         subscribeIndicatorWin = null;    
      //       })
      //     }
          
      //   },
      
      // }
    ])
};