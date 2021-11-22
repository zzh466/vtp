import {Menu,BrowserWindow } from 'electron'
let template = [{
    label: '系统',
    submenu: [{
      label: '置顶',
      click(event, window){
        // BrowserWindow.getAllWindows().forEach(win => win.setAlwaysOnTop(false));
        window.setAlwaysOnTop(true)
      }
    }],
}];
const menu = Menu.buildFromTemplate(template)
Menu.setApplicationMenu(menu)