import log from 'electron-log';
import path from 'path';
import {ipcMain} from 'electron';
const needLog = process.env.NODE_ENV !== 'development';

const logName = `${new Date().toLocaleDateString().replace(/\//g, '')}.log`;
log.transports.console.level = 'silly'
log.transports.file.fileName  = logName;
ipcMain.on('err-log', function(_, msg){
    errorLog(msg)
})
ipcMain.on('info-log', function(_, msg){
    infoLog(msg)
})
export function errorLog(err){
    if(!needLog) return
    log.error(err)
}

export function infoLog(info){
    if(!needLog) return
    log.info(info)
}