import axios from 'axios';
import {ipcMain} from'electron';
import {baseURL} from '../renderer/utils/utils'
axios.defaults.withCredentials=true
axios.defaults.baseURL = `http://${baseURL}`;
let cookie = null;
axios.interceptors.response.use(function(config){
    if(config.headers['set-cookie']&& config.headers['set-cookie'].length){
        cookie= config.headers['set-cookie']
    }
    console.log(config)
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

export default axios;