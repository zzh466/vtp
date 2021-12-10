import {Menu,BrowserWindow, shell } from 'electron'
import {baseURL} from '../renderer/utils/utils'
const url = baseURL.split(':')[0]
let template = [{
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
      type: 'checkbox'
    }
  ]
},{
  label: '配置',
  click(){
    
    shell.openExternal(`http://${url}:8090/userConfig`);
  },
 
}];


export default function(){
  return Menu.buildFromTemplate(template)
};