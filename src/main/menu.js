import {Menu,BrowserWindow, shell, ipcMain } from 'electron'
import {baseURL, winURL} from '../renderer/utils/utils'
import { logPath } from './log';
let account;
ipcMain.on('set-account', (_, args) => account = args);

let childwin = null;
export { childwin};
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
            childwin.loadURL(`${winURL}#config?id=${account}`)
            childwin.removeMenu()
            childwin.on('closed', function(){
              childwin = null;
            })
          }
          
        },
      
      }])
};