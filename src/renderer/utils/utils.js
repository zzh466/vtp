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

export const Direction = ['买', '卖'];

export const CombOffsetFlag = ['开仓', '平仓', '', '平仓']


export const Status = [{msg: '全部成交', key: '0', type: 'success'},{msg: '部分成交', key: '1', type: 'warn'},{msg: '部分成交', key: '2', type: 'warn'},{msg: '未成交', key: '3', type: 'warn'},{msg: '未成交不在队列中', key: '4', type: 'warn'},{msg: '已撤单', key: '5', type: 'danger'},{msg: '未知', key: 'a', type: 'info'},{msg: '尚未触发', key: 'b'},{msg: '已触发', key: 'c'}]


export const version = '211110';