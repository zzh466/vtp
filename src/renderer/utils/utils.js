const typeMap = ['锁仓', '平今', '开仓', '平仓'];
const closeTypeMap = ['一键全平', '按指定手数平仓']
export function getWinName(instrumentID, volume = 1, type = 1, closeType=0){
    return `${instrumentID}   （手数：${volume} 平仓方式：${closeTypeMap[closeType]} 平今策略：${typeMap[type]} ）`
}