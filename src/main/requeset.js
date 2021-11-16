import axios from 'axios';
import {ipcMain} from'electron';
axios.defaults.withCredentials=true
axios.defaults.baseURL = 'http://192.168.0.118:8080/vtpmanagerapi';
let cookie = null;
axios.interceptors.response.use(function(config){
    if(config.headers['set-cookie']&& config.headers['set-cookie'].length){
        cookie= config.headers['set-cookie']
    }
    return config
})
axios.interceptors.request.use(function(config){
    if(cookie){
        config.headers.Cookie = cookie;
    }
    return config;
})
ipcMain.handle('request', async (event, arg) => {
    const result = await axios(arg).then(res => res.data);
    return result
})