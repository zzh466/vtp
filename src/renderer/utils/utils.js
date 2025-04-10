import os from 'os';
import { ipcRenderer } from 'electron';

var interfaces = os.networkInterfaces();
export function getIPAdress(){

for(var devName in interfaces){
    var iface = interfaces[devName];
    for(var i=0;i<iface.length;i++){
        var alias = iface[i];
        if(alias.family === 'IPv4' && alias.address !== '127.0.0.1' && !alias.internal){
            return alias.address;
        }
    }
}
}
console.log(getIPAdress())
export function getMac(){
    for(var i in interfaces){
        for(var j in interfaces[i]){
            if(interfaces[i][j]["family"]==="IPv4" && (interfaces[i][j]["mac"]!=="00:00:00:00:00:00" || interfaces[i][j]["mac"]!=='00:50:56:c0:00:01' )&& interfaces[i][j]["address"]!=="127.0.0.1"){
            return interfaces[i][j]["mac"]
            }
        }
    }
}
export const hostname = os.hostname();//主机名
console.log(hostname);
export const typeMap = ['锁仓', '平今', '开仓', '平仓'];
export const closeTypeMap = ['一键全平', '按指定手数平仓']
export function getWinName(instrumentID, accountIndex, volume = 1, type = 0, closeType=0){
    return `${instrumentID}-${accountIndex}   （手数：${volume} 平仓方式：${closeTypeMap[closeType]} 平今策略：${typeMap[type]} ）`
}

export function getHoldCondition(data={}){
    const {todayAsk=0, todayBuy=0, yesterdayAsk=0, yesterdayBuy=0, todayVolume=0, todayCancel=0, openvolume_limit ='无', vtp_client_cancelvolume_limit='无', big_todayCancel = 0, big_todayCancel_limit = 0} = data;
    return `今多：${todayBuy} 今空：${todayAsk} 昨多：${yesterdayBuy} 昨空：${yesterdayAsk} 今开仓：${todayVolume} 开仓限制：${openvolume_limit} 今撤单：${todayCancel} 撤单限制:${vtp_client_cancelvolume_limit} 大额撤单: ${big_todayCancel} 大额撤单限制：${big_todayCancel_limit}`
}
export const Direction = ['买', '卖'];

export const CombOffsetFlag = ['开仓', '平仓', '', '平仓']


export const Status = [{msg: '全部成交', key: '0', type: 'success'},{msg: '部分成交', key: '1', type: 'warn'},{msg: '部分成交', key: '2', type: 'warn'},{msg: '未成交', key: '3', type: 'warn'},{msg: '未成交不在队列中', key: '4', type: 'warn'},{msg: '已撤单', key: '5', type: 'danger'},{msg: '未知', key: 'a', type: 'info'},{msg: '条件单尚未触发', key: 'b'},{msg: '条件单已触发', key: 'c'}]
  

export const version = '250321a';
export function getyyyyMMdd(d){
    if(!d){
        d = new Date();
    }
   
    var curr_date = d.getDate();
    var curr_month = d.getMonth() + 1; 
    var curr_year = d.getFullYear();
    String(curr_month).length < 2 ? (curr_month = "0" + curr_month): curr_month;
    String(curr_date).length < 2 ? (curr_date = "0" + curr_date): curr_date;
    var yyyyMMdd = curr_year + "" + curr_month +""+ curr_date;
    return yyyyMMdd;
} 

// export const baseURL = process.env.NODE_ENV === 'development'?'/vtpmanagerapi': '139.196.41.155:8082/vtpmanagerapi'
export const baseIP = '139.196.41.155';
// export const baseIP = '192.168.0.18';
export const baseURL = `${baseIP}:8082/vtpmanagerapi`;
// export const baseURL = '192.168.0.18:8082/vtpmanagerapi'


export async function getClientSize(){
    
    const [width, height] = await Promise.all([ipcRenderer.invoke('get-config', 'window_width'), ipcRenderer.invoke('get-config', 'window_height')])
    
    return {
        width,
        height
    }
}

export function setClientSize(width, height){
    if(width){
        ipcRenderer.send('set-config', 'window_width', width);
    }
    if(height){
        ipcRenderer.send('set-config', 'window_height', height);
    }
}
export const tagTime = ['08:59:00', '09:29:00', '10:14:00', '10:29:00', '11:29:00', '12:59:00', '13:29:00', '14:59:00', '15:14:00','20:59:00', '22:59:00', '00:59:00', '02:29:00']
export function speak(text){
    const u = new SpeechSynthesisUtterance();
    u.text = text
    speechSynthesis.speak(u);
}
export const subscribeIndicatorKey = 'subscribeIndicator'
export const specialExchangeId = ['INE', 'SHFE']
export const winURL = process.env.NODE_ENV === 'development'
? `http://localhost:9080`
: `file://${__dirname}/index.html`
