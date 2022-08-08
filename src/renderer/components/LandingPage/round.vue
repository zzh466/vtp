<template>
    <div>
    <Table :tableData='traderData' height="280" :columns='traderColumns' @row-dblclick="$emit('round-click', $event)"/>
    </div>
</template>

<script>
  import { Direction} from '../../utils/utils'
 import Vue from 'vue';
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
                    label: '时间',
                    prop: 'TradeTime'
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
                      
                       let range = ClosePrice - Price;
                       if(_this.price[InstrumentID] && Volume > CloseVolume){
                           range = (range * CloseVolume + (Volume -CloseVolume) *( _this.price[InstrumentID][Direction] -Price)) / Volume;
                       }
                        if(data.Direction==='1'){
                           range = -range;
                       }
                       data.range = range;
                       return range.toFixed(2)
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
                        
                        const {OpenRatioByMoney, OpenRatioByVolume, CloseTodayRatioByMoney, CloseTodayRatioByVolume, CloseRatioByMoney, CloseRatioByVolume} = rate;
                        const {VolumeMultiple} = info;
                        let commission  = (VolumeMultiple * OpenRatioByMoney * data.Price + OpenRatioByVolume )* data.open +  (VolumeMultiple * CloseTodayRatioByMoney * data.ClosePrice + CloseTodayRatioByVolume )* data.closeToady +
                         (VolumeMultiple * CloseRatioByMoney * data.ClosePrice + CloseRatioByVolume )* data.close;
                        
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
                        commission = commission.toFixed(2) ;
                        data.commission = commission;
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
                        return (data.closeProfit + data.optionProfit - data.commission).toFixed(2);
                    }
                },
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
            const yesterday = {};
            this.positions.forEach(({ Volume, Direction, InstrumentID}) => {
                if(!yesterday[InstrumentID]){
                    yesterday[InstrumentID] = [0,0]
                }
                yesterday[InstrumentID][Direction] += Volume;
            })
            this.yesterday = yesterday;
           
                
            this.data.forEach(e => {
                e._volume = undefined
                this.findAnDmatch(e, arr);
            }); 
            
            this.$emit('history-trade', arr.filter(a => a.Volume &&!a.OpenTime))
            this.traderData = arr.filter(a=> {
                return a.TradeTime || a.Volume !== a.CloseVolume 
            })
        },
        update(data){
            this.findAnDmatch(data, this.traderData);
        },
        findAnDmatch(e, arr){
                
                const yesterday = this.yesterday || {};
                const {InstrumentID, Volume, Direction, Price, OpenDate, TradeTime, TradeDate, ExchangeID, OrderSysID, CombOffsetFlag} = e;
                
                 let _volume = e._volume;
                 if(_volume === undefined){
                     _volume = Volume
                 }// 不能修改原数据
                 let item;
                 for(let i = arr.length -1; i >=0 ;i--){
                     const trade = arr[i];
                     if(trade.InstrumentID === InstrumentID && trade.Volume > trade.CloseVolume){
                         item = trade;
                         break;
                     }
                   
                 }
               
                 const combOffsetFlag = !TradeTime || CombOffsetFlag === '0';
                 let open = 0, close = 0, closeToady = 0;
                 if(combOffsetFlag){
                     open = _volume;
                 }else {
                     
                     const _d = Direction === '0'? '1': '0';
                     if(yesterday[InstrumentID] &&yesterday[InstrumentID][_d] >= _volume){
                         close = _volume
                         yesterday[InstrumentID][_d] -= _volume
                     }else{
                         closeToady = _volume;
                     }
                 }

                 if(item){
                     
                     if( item.Direction!==Direction){
                     
                         if(_volume + item.CloseVolume > item.Volume){
                            const gap = item.Volume - item.CloseVolume;
                            item.ClosePrice = (gap * Price +  item.CloseVolume * item.ClosePrice) / item.Volume;
                            e._volume = _volume - gap;
                            if(open){
                                item.open += gap;
                            }
                            if(close){
                                //多计算的平昨要给他加回去
                                item.close += gap;
                                const _d = Direction === '0'? '1': '0';
                                if(yesterday[InstrumentID]){
                                    yesterday[InstrumentID][_d] += e._volume
                                }
                                
                            }
                             if(closeToady){
                                item.closeToady += gap;
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
                        item.TradeTime = TradeTime;
                     }else{
                         
                        
                         if(TradeTime){
                             
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
                                parseIndex: e.parseIndex
                            }) 
                         }else {
                              item.open += open ;
                             item.Volume = item.Volume + _volume; 
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
                         parseIndex: e.parseIndex
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