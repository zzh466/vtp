import {Menu,BrowserWindow, shell, ipcMain } from 'electron'
import {baseURL, winURL} from '../renderer/utils/utils'
import { logPath } from './log';


let childwin = null;
let subscribeIndicatorWin = null
export { childwin, subscribeIndicatorWin};
export default function(checked){
  // console.log(checked)
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
              
              shell.showItemInFolder(logPath);
            },

          }
        ]
      },{
        label: '配置',
        submenu: [{
          label: '快捷键',
          click(){
            if(childwin){
              childwin.show();
            }else{
              childwin = new BrowserWindow({
                height: 1000,
                useContentSize: true,
                width: 1000,
                // parent: mainWindow,
                title: '修改配置快捷键',
                webPreferences: {
                  nodeIntegration: true,
                  contextIsolation: false,
                  webSecurity: false
                }
              })
              childwin.loadURL(`${winURL}#config`)
              childwin.removeMenu()
              childwin.on('closed', function(){
                childwin = null;    
              })
            }
            
          }
        }, {
          label: '提醒合约参数',
            click(){
              if(subscribeIndicatorWin){
                subscribeIndicatorWin.show();
              }else{
                subscribeIndicatorWin = new BrowserWindow({
                  height: 1000,
                  useContentSize: true,
                  width: 1000,
                  // parent: mainWindow,
                  title: '修改提醒合约参数',
                  webPreferences: {
                    nodeIntegration: true,
                    contextIsolation: false,
                    webSecurity: false
                  }
                })
                subscribeIndicatorWin.loadURL(`${winURL}#subscribeIndicator`)
                subscribeIndicatorWin.removeMenu()
                subscribeIndicatorWin.on('closed', function(){
                  subscribeIndicatorWin = null;    
                })
              }
              
            }
        }]
        
      
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