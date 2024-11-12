import log from 'electron-log';
import path from 'path';
import {ipcMain, app} from 'electron';
import {getyyyyMMdd} from '../renderer/utils/utils'
const needLog =process.env.NODE_ENV !== 'development';
// const needLog =true;
let homeDir =  path.dirname(app.getPath('logs'))
console.log('1111111111111111111', homeDir)
const logName = `${getyyyyMMdd()}.log`;
log.transports.console.level = 'silly';

log.transports.file.resolvePath  = ()=> path.join(homeDir,'/logs',logName);
log.transports.file.maxSize = 1024*1024 *40;
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

export function devLog(info){
    log.info(info)
}
export const logPath = path.join(homeDir,'/logs/')