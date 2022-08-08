
import events from 'events'
import ws from 'ws'
import dataFormat from 'silly-datetime'
import {baseURL} from '../renderer/utils/utils' 
import xlsx from 'xlsx';
// const baseURL = '192.168.0.18:8082/vtpmanagerapi'
const changeMap = {
    InstrumentID: 'symbol',
    ExchangeID: 'exchange',
    Volume: 'volume',
    UpperLimitPrice: 'limitUp',
    LowerLimitPrice: 'limitDown',
    LowestPrice:'lowPrice',
    HighestPrice: 'highPrice',
    LastPrice: 'lastPrice',
   
}
for(let b = 1; b < 6; b++){
    changeMap[`BidPrice${b}`] = `bidPrice${b}`
    changeMap[`BidVolume${b}`]= `bidVolume${b}`
    changeMap[`AskPrice${b}`] =`askPrice${b}`
    changeMap[`AskVolume${b}`] =`askVolume${b}`
}

function getDay(time = new Date()){
    return dataFormat.format(time,'YYYY/MM/DD')
}
function getTime(time = new Date()){
    return dataFormat.format(time,'HH:mm:ss')
}
function parseData(data){
    const item = {}
   for(let key in changeMap){
    item[key] = data[changeMap[key]] || 0; 
   }
//    console.log(data);
   item.UpdateTime = data.datetime.substr(8, 2) + ':' + data.datetime.substr(10, 2) + ':' + data.datetime.substr(12, 2)
   
   return item;
   
}

function toTradeData(_data, InstrumentID, index){
    return  {
        Direction: _data.direction,
        InstrumentID,
        Price: _data.price,
        CombOffsetFlag: '0',
        TradeDate: getDay(_data.time),
        TradeTime: getTime(_data.time),
        Volume: _data.volume,
        closeText: _data.closeText,
        openText: _data.openText,
        parseIndex: index
    }
}
const TradeDateMap = {};

export default class FakeTrader{
    constructor(id,orders =[]){
        this.emitter = new  events.EventEmitter();
        this.orders = orders;
        this.priceData = {}
        this.TradeDateMap =TradeDateMap;
        console.log(id)
        this.ws = new ws(`ws://${baseURL}/ws/${id}`);
        this.ws.onmessage = ({data}) => {
            // 
            // console.log(data)
            data =  data.split('@')
            const mess = data[0];
            if(mess === 'QuotDataHist' || mess ==='GroupQuotDataHist'){
                data = data[1];
            
                data = JSON.parse(data);
                this.priceData[data.symbol]  = [
                    data.bidPrice1,
                    data.askPrice1,
                    data.datetime,
                    data.lastPrice
                ];
                this.checktrade(data)
                this.emitter.emit('data', parseData(data));
            }
           
        }
        this.ws.onopen =  (e) =>{
            console.log('opened')
          
            
        }
    }
    on(event, fn){
        this.emitter.on(event, fn.bind(this));
    }
    trade({combOffsetFlag, direction, instrumentID, limitPrice, volumeTotalOriginal}){
        const item = {
            InstrumentID: instrumentID,
            Direction: direction,
            LimitPrice: limitPrice,
            CombOffsetFlag: combOffsetFlag,
            VolumeTotalOriginal: volumeTotalOriginal,
            OrderStatus: '3',
            StatusMsg:"未成交",
            InsertDate: getDay(),
            InsertTime: getTime(),
            VolumeTraded: 0,
            key: +new Date()
        }
        this.orders.push(item)
        this.emitter.emit('order', item);
        this.checktrade({symbol: instrumentID})
        
    }
    cancel(arr){
        console.log(arr)
        arr.forEach(item => {
            item.StatusMsg = '已撤单';
            item.OrderStatus = '5';
            this.emitter.emit('order', item);
        })
    }
    send(msg){
        console.log(msg)
        this.ws.send(msg);
    }
    getTime(instrumentID, time){
        console.log(time)
        time = +new Date(time);
        if(this.priceData[instrumentID]){
            let lasttime = +new Date(this.priceData[instrumentID][2]);
            const gap = time - lasttime
           if(gap >= 0 && gap <= 5000){
            return
           }
        }
       
        console.log(time)
        this.send('NotifyQuotDataHistCancel@');
       
        time = dataFormat.format(time -5000,'YYYYMMDDHHmmss')
         this.send(`NotifyQuotDataHist@${instrumentID}:${time}`) 
    }
    importTrade(instrumentID, path){
        console.log(instrumentID, path)
        const tradeData = [];
        TradeDateMap[instrumentID] = {data: tradeData, flag: 0};
      
        const workbook = xlsx.readFile(path)
        console.log(workbook.SheetNames[0]);
        const data = xlsx.utils.sheet_to_json(workbook.Sheets[workbook.SheetNames[0]]);
        function parse(e, type, directionType){
            const time = + new Date(e[`${type}时间`]);
            const price = e[`${type}价格`];
            const direction = directionType === e['方向']? '0' : '1';
            const volume = e['手数'];
            const openText = e['开仓说明'];
            const closeText = e['锁仓说明'];
            tradeData.push({time, price, direction, volume, openText, closeText});
        }
        data.forEach((e) => {
            parse(e, '开仓', '多')
            parse(e, '锁仓', '空')
        })
        this.emitter.emit('main-trade',tradeData.map((e,index) => toTradeData(e, instrumentID, index)));
    }
    checktrade(data){
        if(data && TradeDateMap[data.symbol]){
            const tradeData = TradeDateMap[data.symbol];
            let datetime = data.datetime;
            const arr = ['/', '/', ' ', ':', ':', '.'];
            arr.forEach((e , index) => {
                const flag = (index+2)*2 + index
                datetime = datetime.substring(0, flag) + e + datetime.substring(flag);
            })
            datetime = +new Date(datetime);
            while(tradeData.data[tradeData.flag] && tradeData.data[tradeData.flag].time <= datetime){
                const _data = tradeData.data[tradeData.flag]
                tradeData.flag ++ ;
                this.emitter.emit('sub-trade',toTradeData(_data, data.symbol));
            }
        }
        const arr = this.orders.filter(e => e.OrderStatus === '3' && e.InstrumentID === data.symbol);
        const priceData = this.priceData;
        arr.forEach(item => {
            const {CombOffsetFlag, LimitPrice, Direction, InstrumentID, VolumeTotalOriginal} = item;
            
            if((Direction === '0' && (LimitPrice >= priceData[InstrumentID][1]|| LimitPrice >= priceData[InstrumentID][3])) || (Direction  !== '0' && (LimitPrice <= priceData[InstrumentID][0] || LimitPrice <= priceData[InstrumentID][3]))){
                this.emitter.emit('trade', {
                    Direction,
                    InstrumentID,
                    Price: Direction === '0' ? priceData[InstrumentID][1]: priceData[InstrumentID][0],
                    CombOffsetFlag,
                    TradeDate: getDay(),
                    TradeTime: getTime(),
                    Volume: VolumeTotalOriginal
                });
                item.StatusMsg = '已成交';
                item.OrderStatus = '0';
                item.VolumeTraded = VolumeTotalOriginal;
                this.emitter.emit('order', item);
            }
        })
    }
    logout(){

    }
}