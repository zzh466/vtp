import {Menu,BrowserWindow, shell } from 'electron'
import {baseURL, winURL} from '../renderer/utils/utils'
import { logPath } from './log'
const url = baseURL.split(':')[0]

let childwin = null;

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
          
        },
      
      }])
};