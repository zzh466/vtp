<template>
    <div>
    <Table :tableData='traderData' height="280" :columns='traderColumns' @row-dblclick="$emit('round-click', $event)"/>
    </div>
</template>

<script>
  import { Direction} from '../../utils/utils'
 import Vue from 'vue';
 import { ipcRenderer } from 'electron';
  Vue.component('Tag', {
        template: `<div class='trade-tag'>
            <div class='finish' :style='{width: width + "%"}'></div>
            <div  class='remain' :style='{width: 100 - width + "%"}'></div>
        </div>`,
        props: ['data'],
        computed: {
            width(){
                const {Volume, CloseVolume} = this.data;
                return CloseVolume / Volume *100;
            }
        }
    })

  export default {
    props: ['data', 'rates', 'price', 'instrumentInfo', 'positions'],
    data(){
        const _this = this;
        return {
            traderColumns: [
                {
                    label: '合约',
                    prop: 'InstrumentID',
                },{
                    label: '日期',
                    prop:'TradeDate'
                },{
                    label: '开仓时间',
                    prop: 'TradeTime'
                },{
                    label: '平仓时间',
                    prop: 'CloseTime',
                    render(item){
                        return [...new Set(item.CloseTime.map(e => e.split('-')[0]))].join('\n')
                    }
                },{
                    label: '持仓时间（秒）',
                    prop: 'HoldTime',
                    render(item){
                        let time = [];
                        
                        if(item.TradeTime && item.CloseTime.length){

                            item.CloseTime.map(e =>{
                                e = e.split('-');
                                const count = +e[1];
                                e = e[0];
                                
                              
                                if(time[time.length-1] && time[time.length-1].time ===e){
                                    time[time.length-1].count += count;
                                    return
                                }
                                const start = item.TradeTime.split(':')
                                const end =  e.split(':')
                                let time1 = end[0] - start[0];
                                let time2;
                                if(end[1] > start[1]){
                                    time2 = end[1] - start[1];
                                }else{
                                    time2 = -(start[1]-end[1]);
                                }
                                let time3;
                                if(end[2] > start[2]){
                                    time3 = end[2] - start[2];
                                }else{
                                    time3 = -(start[2]-end[2]);
                                }
                                let range= 60*60*time1 + 60*time2 + time3
                                if(range < 0){
                                    range = range + 60 *60 * 24
                                }
                                time.push({
                                    count,
                                    time: e,
                                    range
                                }) 
                            })
                           
                        }
                        return time.map(e => `${e.range} (${e.count})`).join('\n')
                    }
                },{
                    label: '方向',
                    prop: 'Direction',
                    class(item){
                        return item.Direction === '1'? 'sell-direction': '';
                    }, 
                    render(item){
                        return Direction[item.Direction];
                    }
                },
                {
                    label: '手数',
                    prop: 'Volume',
                   
                    render(item){
            
                        const arr = [item.CloseVolume ,item.Volume];
                        return arr.join('/') ;
                    }
                },
                 {
                    label: '成交状态',
                    prop: 'OrderStaus',
                    component: 'Tag'
                 },
                {
                    label: '开仓均价',
                    prop: 'Price',
                     type: 'number',
                    render(data){
                        return data.Price.toFixed(2)
                    }
                },
                {
                    label: '平仓均价',
                    prop: 'ClosePrice',
                     type: 'number',
                    render(data){
                    
                        return data.ClosePrice.toFixed(2)
                    }
                },
                {
                     label: '盈亏点',
                    prop: 'range',
                     type: 'number',
                    render(data){
;
                     
                       const {ClosePrice , Price, Volume, CloseVolume, Direction, InstrumentID} = data;
                        if(!CloseVolume && !_this.price[InstrumentID]){
                            return 0
                        }
                       let range = ClosePrice - Price;
                       if(!ClosePrice)  range = 0
                       if(_this.price[InstrumentID] &&  _this.price[InstrumentID][Direction] && Volume > CloseVolume){
                            let _price = _this.price[InstrumentID][Direction]
                            if(!_price ){
                                _price = Price
                            }
                            if(_price > Number.MAX_SAFE_INTEGER){
                                _price = _this.price[InstrumentID][Direction + 2]
                            }
                           range = (range * CloseVolume + (Volume -CloseVolume) *( _price -Price)) / Volume;
                       }
                        if(data.Direction==='1'){
                           range = -range;
                       }
                       
                       range = range.toFixed(2)
                       if(range > 0 && range > data.maxProfit){
                            
                            data.maxProfit = parseFloat(range);
                        }else if(range < 0 && range < data.minProfit){
                            
                            data.minProfit = parseFloat(range);
                        }
                       return range
                    }
                },
                {
                     label: '手续费',
                    prop: 'commission',
                     type: 'number',
                    render(data){
                        
                        
                        
                        const rate = _this.rates.find(e => !e.uncatch && (data.InstrumentID===e.InstrumentID || data.InstrumentID.startsWith(e.InstrumentID)))
                        
                        const info = _this.instrumentInfo.find(e => data.InstrumentID === e.InstrumentID );
                        if(!rate || !info){
                            data.commission = 0;
                            return 0
                        } 
                        
                        const  {Price, ClosePrice, open = 0, closeToady = 0, close = 0} = data;
                        const {OpenRatioByMoney, OpenRatioByVolume, CloseTodayRatioByMoney, CloseTodayRatioByVolume, CloseRatioByMoney, CloseRatioByVolume} = rate;
                        const {VolumeMultiple} = info;
                        let commission  = (VolumeMultiple * OpenRatioByMoney * Price + OpenRatioByVolume )* open +  (VolumeMultiple * CloseTodayRatioByMoney * ClosePrice + CloseTodayRatioByVolume )* closeToady +
                         (VolumeMultiple * CloseRatioByMoney * ClosePrice + CloseRatioByVolume )* close;
                        
                        // switch (data.closeType) {
                        //     case "0":
                        //         // 开仓手续费
                        //         
                        //         break;
                        //     case "1":
                        //         // 平仓手续费
                        //         commission = rate.CloseRatioByMoney * data.closePrice + rate.CloseRatioByVolume * data.closeVolume
                        //         break;
                        //     case "3":
                        //         // 平今手续费
                        //         commission = rate.CloseTodayRatioByMoney * data.closePrice + rate.CloseTodayRatioByVolume * data.closeVolume
                        //         break;
                        // }
                          data.commission = commission;
                        commission = commission.toFixed(2) ;
                      
                        return commission
                    }
                },
                {
                     label: '持仓盈亏',
                    prop: 'optionProfit',
                     type: 'number',
                    render(data){
                         const {  Price, Volume, CloseVolume, Direction, InstrumentID} = data;
                         let profit = 0
                         if( Volume> CloseVolume){
                              const info = _this.instrumentInfo.find(e => InstrumentID === e.InstrumentID);
                             const volume  = Volume - CloseVolume;
                             
                             if(_this.price[InstrumentID] && info){
                                 let currentPrice = _this.price[InstrumentID][Direction];
                                 if(!currentPrice){
                                     currentPrice = _this.price[InstrumentID][Direction + 2]
                                 }
                                 profit =(currentPrice - Price) *volume * info.VolumeMultiple;
                                
                             }
                              if(Direction==='1'){
                                profit = -profit;
                            }

                         }
                         data.optionProfit = profit
                        return profit.toFixed(2)
                    }
                },
                {
                     label: '平仓盈亏',
                    prop: 'closeProfit',
                     type: 'number',
                    render(data){
                           const {  Price, CloseVolume, ClosePrice , InstrumentID, Direction} = data;
                         let profit = 0
                         if(CloseVolume ){
                              const info = _this.instrumentInfo.find(e => InstrumentID === e.InstrumentID);
                              if(info){
                                   profit = (ClosePrice -Price) * CloseVolume* info.VolumeMultiple
                              }
                            
                            if(Direction==='1'){
                                profit = -profit;
                            }

                         }
                         data.closeProfit = profit
                        return profit.toFixed(2)
                    }
                },
                {
                     label: '实际盈亏',
                    prop: 'true',
                     type: 'number',
                    render(data){
                        const price = (data.closeProfit + data.optionProfit - data.commission).toFixed(2)
                       
                        return price;
                    }
                },
                {
                    label: '最大持仓盈利',
                    prop: 'maxProfit'
                },
                {
                    label: '最大持仓亏损',
                    prop: 'minProfit'
                }
                //   {
                //      label: 'pp',
                //     prop: 'true',
                //      type: 'number',
                //     render(data){ 
                //         return data.open +'/'+ data.close +'/'+ data.closeToady
                //     }
                // }
            ],
            traderData: []
        }
    },
  
    methods: {
        init(){
            let arr = []
            // const yesterday = {};
            // this.positions.forEach(({ Volume, Direction, InstrumentID}) => {
            //     if(!yesterday[InstrumentID]){
            //         yesterday[InstrumentID] = [0,0]
            //     }
            //     yesterday[InstrumentID][Direction] += Volume;
            // })
            // this.yesterday = yesterday;
           
                
            this.data.forEach(e => {
                e._volume = undefined
                this.findAnDmatch(e, arr);
            }); 
            
            
            this.traderData = arr.filter(a=> {
                //订阅未完成回合 或者 已完成但是是隔节合约的行情
                if(a.CloseVolume < a.Volume || (a.CloseTime.length && !a.OpenTime)){
                    
                    ipcRenderer.send('subscribe-instrument', a.InstrumentID);
                }
                return a.CloseTime.length || a.Volume !== a.CloseVolume 
            })
            console.log(this.traderData, '1111111111111111')
             this.$emit('history-trade', this.traderData.filter(a => a.Volume  &&!a.OpenTime))
        },
        update(data){
            this.findAnDmatch(data, this.traderData);
        },
        findAnDmatch(e, arr){
                
                // const yesterday = this.yesterday || {};
                const {InstrumentID, Volume, Direction, Price, OpenDate, TradeTime, TradeDate, ExchangeID, OrderSysID, CombOffsetFlag, open =0, close =0, closeToady=0} = e;
                
                 let _volume = e._volume;
                 if(_volume === undefined){
                     _volume = Volume
                 }// 不能修改原数据
                 let item;
                 for(let i = arr.length -1; i >=0 ;i--){
                     const trade = arr[i];
                     if(trade.InstrumentID === InstrumentID && trade.Volume > trade.CloseVolume){
                         item = trade;
                         //一笔订单多次成交会应该合并所以要是同向的要继续往后面找
                         if(item.Direction === Direction){
                            
                            continue;
                         }
                         break;
                     }
                   
                 }
               
                 const combOffsetFlag = !TradeTime || CombOffsetFlag === '0';
                //  let open = 0, close = 0, closeToady = 0;
                //  if(combOffsetFlag){
                //      open = _volume;
                //  }else {
                     
                //      const _d = Direction === '0'? '1': '0';
                //      if(yesterday[InstrumentID] &&yesterday[InstrumentID][_d] >= _volume){
                //          close = _volume
                //          yesterday[InstrumentID][_d] -= _volume
                //      }else{
                //          closeToady = _volume;
                //      }
                //  }

                 if(item){
                     
                     if( item.Direction!==Direction){
                        
                         if(_volume + item.CloseVolume > item.Volume){
                            const gap = item.Volume - item.CloseVolume;
                            item.ClosePrice = (gap * Price +  item.CloseVolume * item.ClosePrice) / item.Volume;
                            e._volume = _volume - gap;
                            //为了下面TradeTime 计算准确
                            _volume = gap;
                            if(open){
                                item.open += gap;
                                e.open = open - gap
                            }
                            if(close){
                               
                                item.close += gap;
                                e.close = close - gap;
                                
                            }
                             if(closeToady){
                                item.closeToady += gap;
                                e.closeToady = closeToady - gap;
                            }
                           
                        
                            item.CloseVolume = item.Volume;
                            this.findAnDmatch(e, arr)
                        }else{
                            if(!TradeTime){
                                 item.open -= open ;
                                item.Volume = item.Volume - _volume;
                            }else{
                                item.open += open;
                                 item.close += close;
                                item.closeToady += closeToady;
                                item.ClosePrice = (item.CloseVolume * item.ClosePrice  + Price*_volume )/ (item.CloseVolume + _volume);
                                item.CloseVolume = item.CloseVolume + _volume;
                                item.closeText = e.closeText;
                            }
                            
                        }
                        //这里
                        if(TradeTime){
                            
                            item.CloseTime.push(`${TradeTime}-${_volume}`);
                        }
                       
                     }else{
                         
                        
                         if(TradeTime && item.OrderSysID !== OrderSysID){
                             
                            arr.unshift({
                                InstrumentID,
                                Volume: _volume,
                                Direction,
                                Price,
                                TradeDate: TradeDate,
                                TradeTime,
                                CloseVolume: 0,
                                ClosePrice: 0,
                                OpenTime:TradeTime,
                                OrderSysID,
                                ExchangeID,
                                combOffsetFlag,
                                open,
                                close,
                                closeToady,
                                openText: e.openText,
                                parseIndex: e.parseIndex,
                                maxProfit: 0,
                                minProfit: 0,
                                CloseTime: []
                            }) 
                         }else {
                              item.open += open ;
                              const total = item.Volume + _volume; 
                              item.Price = (item.Volume * item.Price +  Price * _volume)/ total;
                              item.Volume = total; 
                              this.$forceUpdate()
                         }
                        
                         
                     }
                    
                   
                }else{
                    
                   
                    arr.unshift({
                        InstrumentID,
                        Volume: _volume,
                        Direction,
                        Price,
                        TradeDate: TradeDate||OpenDate,
                        TradeTime,
                        CloseVolume: 0,
                        ClosePrice: 0,
                        OpenTime:TradeTime,
                        OrderSysID,
                        ExchangeID,
                        combOffsetFlag,
                        open,
                        close,
                        closeToady,
                        openText: e.openText,
                         parseIndex: e.parseIndex,
                         maxProfit: 0,
                        minProfit: 0,
                        CloseTime: []
                    })
                }
            }
    }
  }
</script>

<style>
    .trade-tag {
        display: flex;
       
    }
    .trade-tag .finish {
        background-color: rgb(96, 235, 61);
        height: 20px;
    }
    .trade-tag .remain {
        background-color: rgb(255,200,200);
         height: 20px;
    }
</style>