import log from 'electron-log';
import path from 'path';
import {ipcMain, app} from 'electron';
 
const needLog =process.env.NODE_ENV !== 'development';
// const needLog =true;
let homeDir =  path.dirname(app.getPath('exe'))
console.log('1111111111111111111', homeDir)
const logName = `${new Date().toLocaleDateString().replace(/\//g, '')}.log`;
log.transports.console.level = 'silly';

log.transports.file.resolvePath  = ()=> path.join(homeDir,'/log',logName);
log.transports.file.maxSize = 1024*1024 *10;
ipcMain.on('err-log', function(_, msg){
    errorLog(msg)
})
ipcMain.on('info-log', function(_, msg){
    infoLog(msg)
})
ipcMain.on('data-log', function(_, msg){
    console.log(msg, 123456)
    log.info(msg)
})
export function errorLog(err){
    if(!needLog) return
    log.error(err)
}

export function infoLog(info){
    if(!needLog) return
    log.info(info)
}

export const logPath = path.join(homeDir,'/log')