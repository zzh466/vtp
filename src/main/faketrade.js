
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

function getDay(){
    return dataFormat.format(new Date(),'YYYYMMDD')
}
function getTime(){
    return dataFormat.format(new Date(),'HH:mm:ss')
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


export default class FakeTrader{
    constructor(id,orders =[]){
        this.emitter = new  events.EventEmitter();
        this.orders = orders;
        this.priceData = {}
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
                    data.askPrice1
                ];
                this.checktrade()
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
        this.checktrade()
        
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
        this.ws.send(msg);
    }
    importTrade(instrumentID, path){
        const tradeData = xlsx.parse(path)
        console.log(tradeData);
    }
    checktrade(instrumentID){
        const arr = this.orders.filter(e => e.OrderStatus === '3' && e.InstrumentID === instrumentID);
        const priceData = this.priceData;
        arr.forEach(item => {
            const {CombOffsetFlag, LimitPrice, Direction, InstrumentID, VolumeTotalOriginal} = item;
            console.log(item)
            if(Direction === '0' && LimitPrice >= priceData[InstrumentID][1] || (Direction  !== '0' && LimitPrice <= priceData[InstrumentID][0] )){
                this.emitter.emit('trade', {
                    Direction,
                    InstrumentID,
                    Price: Direction === '0' ? priceData[InstrumentID][1]: priceData[InstrumentID][0],
                    CombOffsetFlag,
                    TradeTime: getDay(),
                    TradeDate: getTime(),
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