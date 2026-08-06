import Store from 'electron-store'
import {ipcMain} from'electron';

const store = new Store()

const config = {
    window_width : 1500,
    window_height: 300,
    color_blindness: false,
    over_price: 100,
    vtp_client_openvolume_limit: 'IF:2;jm:3',
    vtp_client_openvolume_limit2: 'IF:20',
    vtp_client_cancelvolume_limit: 'IF:10;jm:2',

    login_config: {
        authCode: '3KBA9KLGMWPJN6K7',
        appId: 'client_shaob_20260717',
        tradeAddr: 'tcp://58.240.234.72:41211',
        quotAddr: 'tcp://58.240.234.72:41222',
        brokerId: '9060',
        
    },
    config: {
        barToBorder: 20,
        barVolume: 120,
        barWidth: 10,
        broadcastOpenInterest: false,
        calcBarType: 2,
        configNm: "1",
        hotkey: '6,54,3,0,0,10,,0;5,53,3,0,0,5,,0;p,80,1,0,-20,0,,0;Num 1,97,1,0,1,1,0,0;Num 3,99,1,1,1,1,0,0;Num 4,100,1,0,3,1,0,0;Num 6,102,1,1,3,1,0,0;Num 7,103,1,0,15,1,,0;Num 9,105,1,1,15,1,,0;Q,81,1,0,-1,1,0,0;A,65,1,0,-3,1,0,0;Z,90,0,0,-5,1,0,0;W,87,1,1,-1,1,0,0;S,83,1,1,-3,1,0,0;X,88,1,1,-10,1,,0;Num 5,101,2,0,0,1,0,0;1,49,3,0,0,1,0,0;2,50,3,0,0,2,0,0;3,51,3,0,0,3,0,0;4,52,3,0,0,4,,0;U,85,6,0,0,1,0,0;I,73,6,0,0,1,0,1;O,79,6,0,0,1,0,2;Num 0,96,1,0,0,1,0,0;Num Del,110,1,1,0,1,0,0',

        
        instruments: "IC2609,IF2609,IH2609,IF2612,SM609,jm2609,a2609,j2609",
        sysCloseTStrategy: 1,
        sysCloseType: 0,
        sysOrderVolume: 1,
        topQuot: false,
        volumeScaleCount: 10,
        volumeScaleHeight: 20,
        volumeScaleTick: 10,
        volumeScaleType: 2,
        volumeXOffset: 2,
        volumeYOffset: 0,
        windowsCloseCd: false,
        windowsOpenCd: false

    },
    thrRealProfit: 20000

}
for(let key in config){
    const value = store.get(key);
     console.log(value, 121313)
    if(value === undefined){
       
        store.set(key, config[key])
    }else{
        config[key] = value
    }
}

export function getConfig(key){
    return config[key]
}
export function setconfig(key, value){
    config[key] = value
    store.set(key, value)
}
ipcMain.handle('get-config', (event, key) => {
    
    return getConfig (key)
})

ipcMain.on('set-config', (event, key, value) => {
    console.log(key,'set')
    setconfig(key, value)
})
// ipcMain.on('update-configs', function(_, arg){

  
//   mainWindow.webContents.send('update-configs', arg)
// })