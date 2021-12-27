import {Menu,BrowserWindow, shell } from 'electron'
import {baseURL} from '../renderer/utils/utils'
import { logPath } from './log'
const url = baseURL.split(':')[0]



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
          
          shell.openExternal(`http://${url}:8090/userConfig`);
        },
      
      }])
};