import Store from 'electron-store'
import {ipcMain} from'electron';

const store = new Store()

const config = {
    window_width : 1500,
    window_height: 300
}
for(let key in config){
    const value = store.get(key);
    if(value === undefined){
        store.set(key, config[key])
    }else{
        config[key] = value
    }
}


ipcMain.handle('get-config', (event, key) => {
    console.log(key,'get')
    return config[key]
})

ipcMain.on('set-config', (event, key, value) => {
    console.log(key,'set')
    config[key] = value
    store.set(key, value)
})