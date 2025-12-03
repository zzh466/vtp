import Store from 'electron-store'
import {ipcMain} from'electron';

const store = new Store()

const config = {
    window_width : 1500,
    window_height: 300,
    color_blindness: false,
    over_price: 100,
    vtp_client_openvolume_limit: 'IF:490;IC:490;IH:490;IM:490;jm:1980;jm2509:480;jm2601:980;j:45;p:9970;pg:9970;i:1970;m:19970;pp:9970;l:7970;c:7970;y:14970;v:17970;lh:980;SM:9970;SA:9970;MA:24970;FG:24970;RM:14970;OI:9970;CF:9970;SR:9970;ZC:15;lc:9970;lc2509:480;lc2510:1970;lc2511:1970;lc2512:1970;lc2601:480;lc2602:1970;lc2603:1970;lc2604:1970;lc2605:1970;si:9970;si2509:480;si2510:1970;si2511:980;si2512:1970;si2601:1970;ps:9970;ps2509:480;ps2510:1970;ps2511:980;ps2512:1970;ps2601:1970;ec:185;sc:3170;rb:31970;fu:15970;ag:12970;hc:9970;sp:7970;ru:5970;al:3970;zn:2970;au:2770;cu:1970',
    vtp_client_cancelvolume_limit: 'ni:500;',

    login_config: {
        authCode: 'ZASEVQRCNXRITENY',
        appId: 'client_mwqcw_1.0.1',
        tradeAddr: 'tcp://124.74.248.123:41257',
        quotAddr: 'tcp://124.74.248.123:41255',
        brokerId: '3010',
        
    },
    config: {
        barToBorder: 20,
        barVolume: 120,
        barWidth: 10,
        broadcastOpenInterest: false,
        calcBarType: 2,
        configNm: "1",
        hotkey: '6,54,3,0,0,10,,0;5,53,3,0,0,5,,0;p,80,1,0,-20,0,,0;Num 1,97,1,0,1,1,0,0;Num 3,99,1,1,1,1,0,0;Num 4,100,1,0,3,1,0,0;Num 6,102,1,1,3,1,0,0;Num 7,103,1,0,15,1,,0;Num 9,105,1,1,15,1,,0;Q,81,1,0,-1,1,0,0;A,65,1,0,-3,1,0,0;Z,90,0,0,-5,1,0,0;W,87,1,1,-1,1,0,0;S,83,1,1,-3,1,0,0;X,88,1,1,-10,1,,0;Num 5,101,2,0,0,1,0,0;1,49,3,0,0,1,0,0;2,50,3,0,0,2,0,0;3,51,3,0,0,3,0,0;4,52,3,0,0,4,,0;U,85,6,0,0,1,0,0;I,73,6,0,0,1,0,1;O,79,6,0,0,1,0,2;Num 0,96,1,0,0,1,0,0;Num Del,110,1,1,0,1,0,0',

        
        instruments: "IF2601,IC2601,IC2603,IF2603,SM601,jm2601",
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