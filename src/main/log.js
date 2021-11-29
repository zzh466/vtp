import log from 'electron-log';
import path from 'path';
import {app} from 'electron';
const needLog = process.env.NODE_ENV !== 'development';

const logName = `${new Date().toLocaleDateString().replace(/\//g, '')}.log`;
log.transports.console.level = 'silly'
log.transports.file.fileName  = logName;
export function errorLog(err){
    if(!needLog) return
    log.error(err)
}

export function infoLog(info){
    if(!needLog) return
    log.info(info)
}