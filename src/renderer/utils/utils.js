import os from 'os';
export function getIPAdress(){
var interfaces = os.networkInterfaces();
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

export const hostname = os.hostname();//主机名
console.log(hostname);
const typeMap = ['锁仓', '平今', '开仓', '平仓'];
const closeTypeMap = ['一键全平', '按指定手数平仓']
export function getWinName(instrumentID, volume = 1, type = 0, closeType=0){
    return `${instrumentID}   （手数：${volume} 平仓方式：${closeTypeMap[closeType]} 平今策略：${typeMap[type]} ）`
}

export function getHoldCondition(data={}){
    const {todayAsk=0, todayBuy=0, yesterdayAsk=0, yesterdayBuy=0, todayVolume=0, todayCancel=0, openvolume_limit ='无'} = data;
    return `今多：${todayBuy} 今空：${todayAsk} 昨多：${yesterdayBuy} 昨空：${yesterdayAsk} 今开仓：${todayVolume} 今撤单：${todayCancel} 开仓限制：${openvolume_limit}`
}
export const Direction = ['买', '卖'];

export const CombOffsetFlag = ['开仓', '平仓', '', '平仓']


export const Status = [{msg: '全部成交', key: '0', type: 'success'},{msg: '部分成交', key: '1', type: 'warn'},{msg: '部分成交', key: '2', type: 'warn'},{msg: '未成交', key: '3', type: 'warn'},{msg: '未成交不在队列中', key: '4', type: 'warn'},{msg: '已撤单', key: '5', type: 'danger'},{msg: '未知', key: 'a', type: 'info'},{msg: '尚未触发', key: 'b'},{msg: '已触发', key: 'c'}]


export const version = '220323a';
export function getyyyyMMdd(){
    var d = new Date();
    var curr_date = d.getDate();
    var curr_month = d.getMonth() + 1; 
    var curr_year = d.getFullYear();
    String(curr_month).length < 2 ? (curr_month = "0" + curr_month): curr_month;
    String(curr_date).length < 2 ? (curr_date = "0" + curr_date): curr_date;
    var yyyyMMdd = curr_year + "" + curr_month +""+ curr_date;
    return yyyyMMdd;
} 

// export const baseURL = process.env.NODE_ENV === 'development'?'192.168.0.108:8080/vtpmanagerapi': '139.196.41.155:8080/vtpmanagerapi'
export const baseURL = '139.196.41.155:8080/vtpmanagerapi'

const key = 'user-client';
export function getClientSize(){
    const width = parseInt(localStorage.getItem(`${key}-width`)) || 1500;
    const height =  parseInt(localStorage.getItem(`${key}-height`)) || 300;
    return {
        width,
        height
    }
}

export function setClientSize(width, height){
    if(width){
        localStorage.setItem(`${key}-width`, width);
    }
    if(height){
        localStorage.setItem(`${key}-height`, height);
    }
}

export const specialExchangeId = ['INE', 'SHFE']