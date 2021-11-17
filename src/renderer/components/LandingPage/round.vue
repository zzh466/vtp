<template>
    <Table :tableData='traderData' height="280" :columns='traderColumns'/>
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
    props: ['data', 'rates', 'price', 'instrumentInfo'],
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
                    render(item){
                        return Direction[item.Direction];
                    }
                },
                {
                    label: '手数',
                    prop: 'Volume',
                    render(item){
            
                        const arr = [item.CloseVolume ,item.Volume];
                        return arr.join('/');
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
                    render(data){
                        return data.Price.toFixed(2)
                    }
                },
                {
                    label: '平仓均价',
                    prop: 'ClosePrice',
                    render(data){
                    
                        return data.ClosePrice.toFixed(2)
                    }
                },
                {
                     label: '盈亏点',
                    prop: 'range',
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
                    render(data){
                        
                
                        const rate = _this.rates.find(e => data.InstrumentID.startsWith(e.InstrumentID))
                        const info = _this.instrumentInfo.find(e => data.InstrumentID === e.id);
                        if(!rate || !info){
                            data.commission = 0;
                            return 0
                        } 
                        const {OpenRatioByMoney, OpenRatioByVolume, CloseTodayRatioByMoney, CloseTodayRatioByVolume, CloseRatioByMoney, CloseRatioByVolume} = rate;
                        const {VolumeMultiple} = info.field;
                        let commission  = (VolumeMultiple * OpenRatioByMoney * data.Price + OpenRatioByVolume )* data.Volume;
                        if(data.ClosePrice){
                            if(data.closeType === '1'){
                                 commission = commission+ (VolumeMultiple * CloseTodayRatioByMoney * data.ClosePrice + CloseTodayRatioByVolume )* data.CloseVolume;
                            }else{
                                 commission = commission+  (VolumeMultiple * CloseRatioByMoney * data.ClosePrice + CloseRatioByVolume )* data.CloseVolume;
                            }
                        }
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
                        return commission.toFixed(2);
                    }
                },
                {
                     label: '持仓盈亏',
                    prop: 'optionProfit',
                    render(data){
                         const {  Price, Volume, CloseVolume, Direction, InstrumentID} = data;
                         let profit = 0
                         if( Volume> CloseVolume){
                              const info = _this.instrumentInfo.find(e => InstrumentID === e.id);
                             const volume  = Volume - CloseVolume;
                             
                             if(_this.price[InstrumentID] && info){
                                 profit =(_this.price[InstrumentID][Direction] - Price) *volume * info.field.VolumeMultiple;
                                
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
                    render(data){
                           const {  Price, CloseVolume, ClosePrice , InstrumentID, Direction} = data;
                         let profit = 0
                         if(CloseVolume ){
                              const info = _this.instrumentInfo.find(e => InstrumentID === e.id);
                              if(info){
                                   profit = (ClosePrice -Price) * CloseVolume* info.field.VolumeMultiple
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
                    render(data){
                        return (data.closeProfit + data.optionProfit - data.commission).toFixed(2);
                    }
                }
            ]
        }
    },
    computed:{
        traderData(){
            let arr = []
            function findAnDmatch(e){
                
                 const {InstrumentID, Volume, Direction, Price, OpenDate, TradeTime, TradeDate, ExchangeID} = e;
                 let _volume = e._volume;
                 if(_volume === undefined){
                     _volume = Volume
                 }// 不能修改原数据
                 const item =arr.slice().reverse().find(trade => trade.InstrumentID === InstrumentID && trade.Volume > trade.CloseVolume)
                 if(item){
                     if( item.Direction!==Direction){
                         if(_volume + item.CloseVolume > item.Volume){
                            const gap = item.Volume - item.CloseVolume;
                            item.ClosePrice = (gap * Price +  item.CloseVolume * item.ClosePrice) / item.Volume;
                            e._volume = _volume - gap;
                            item.CloseVolume = item.Volume;
                            findAnDmatch(e)
                        }else{
                            if(!TradeTime){
                                item.Volume = item.Volume - _volume;
                            }else{
                                item.ClosePrice = (item.CloseVolume * item.ClosePrice  + Price*_volume )/ (item.CloseVolume + _volume);
                                item.CloseVolume = item.CloseVolume + _volume;
                            }
                            
                        }
                        item.TradeTime = TradeTime;
                     }else{
                         if(TradeTime){
                                arr.unshift({
                                InstrumentID,
                                Volume: e._volume || Volume,
                                Direction,
                                Price,
                                TradeDate: TradeDate,
                                TradeTime,
                                CloseVolume: 0,
                                ClosePrice: 0
                            })
                         }else {
                             item.Volume = item.Volume + Volume; 
                         }
                        
                         
                     }
                    
                   
                }else{
                   
                    arr.unshift({
                        InstrumentID,
                        Volume:e._volume || Volume,
                        Direction,
                        Price,
                        TradeDate: TradeDate||OpenDate,
                        TradeTime,
                        CloseVolume: 0,
                        ClosePrice: 0,
                        closeType: TradeTime && ExchangeID !== 'CFFEX'? '1': '0'
                    })
                }
            }
            this.data.sort((a, b)=>{
                const date1 = a.OpenDate || a.TradeDate
                const date2 = b.OpenDate || b.TradeDate
                return date1 - date2
            }).forEach(e => {
                findAnDmatch(e);
            });
            return arr.filter(a=> {
                return a.TradeTime || a.Volume !== a.CloseVolume 
            })
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