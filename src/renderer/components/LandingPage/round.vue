<template>
    <Table :tableData='traderData' height="250" :columns='traderColumns'/>
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
    props: ['data', 'rates', 'price', 'instruments'],
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
                       let range =  data.ClosePrice - data.Price;
                     
                       const {ClosePrice , Price, Volume, CloseVolume, Direction, InstrumentID} = data;
                       range = ClosePrice - Price;
                       if(_this.price[InstrumentID] && Volume > CloseVolume){
                           range = (range * CloseVolume + (Volume -CloseVolume) *( _this.price[InstrumentID][Direction] -Price)) / Volume;
                       }
                        if(data.Direction==='1'){
                           range = -range;
                       }
                       return range.toFixed(2)
                    }
                },
                {
                     label: '手续费',
                    prop: 'commission',
                    render(data){
                        //closeType 1平今 2平昨
                        return data.Price.toFixed(2)
                    }
                },
                {
                     label: '持仓盈亏',
                    prop: 'optionProfit',
                    render(data){
                        return data.Price.toFixed(3)
                    }
                },
                {
                     label: '平仓盈亏',
                    prop: 'closeProfit',
                    render(data){
                        return data.Price.toFixed(3)
                    }
                },
                {
                     label: '实际盈亏',
                    prop: 'true',
                    render(data){
                        return data.Price.toFixed(3)
                    }
                }
            ]
        }
    },
    computed:{
        traderData(){
            let arr = []
            function findAnDmatch(e){
                 const {InstrumentID, Volume, Direction, Price, OpenDate, TradeTime, TradeDate} = e;
                 const item =arr.find(trade => trade.InstrumentID === InstrumentID && trade.Volume > trade.CloseVolume)
                 if(item){
                     if( item.Direction!==Direction){
                         if(Volume + item.CloseVolume > item.Volume){
                            const gap = item.Volume - item.CloseVolume;
                            item.ClosePrice = (gap * Price +  item.CloseVolume * item.ClosePrice) / item.Volume;
                            e.Volume = Volume - gap;
                            item.CloseVolume = item.Volume;
                            
                            if(item.TradeTime){
                                item.closeType='1'
                            }else{
                                 item.closeType='0'
                            }
                            findAnDmatch(e)
                        }else{
                            if(!TradeTime){
                                item.Volume = item.Volume - Volume;
                            }else{
                                item.ClosePrice = (item.CloseVolume * item.ClosePrice  + Price*Volume )/ (item.CloseVolume + Volume);
                                item.CloseVolume = item.CloseVolume + Volume;
                            }
                            
                        }
                        item.TradeTime = TradeTime;
                     }else{
                         if(TradeTime){
                                arr.unshift({
                                InstrumentID,
                                Volume,
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
                        Volume,
                        Direction,
                        Price,
                        TradeDate: TradeDate||OpenDate,
                        TradeTime,
                        CloseVolume: 0,
                        ClosePrice: 0
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