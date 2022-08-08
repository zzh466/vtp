<template>
      <Table  height='280' :columns='orderColumns' row-key='key' :tableData='tableData'/>
</template>
<script>
  import { Direction, CombOffsetFlag, Status as parseStatus} from '../../utils/utils';

  import Status from './Status.vue';
  
  import Vue from 'vue';

 
  Vue.component('StatusMsg', {
    props: ['data'],
    template: `<el-popover
                placement="left"
                width='150px'
                trigger="hover"
                :content="data.StatusMsg">
                <div slot="reference">{{data.StatusMsg.length> 4? data.StatusMsg.slice(0,4) + '……': data.StatusMsg}}</div>
              </el-popover>
            `
  })
 
  Vue.component('Status', Status);
export default ({
   
    data(){
         const _this = this;
        return{
        orderColumns: [{
          label: '合约',
          prop: 'InstrumentID',
        },{
          label: '日期',
          prop:'InsertDate'
        },{
          label: '时间',
          prop: 'InsertTime'
        },{
          label: '方向',
          prop: 'Direction',
          class(item){
             return item.Direction === '1'? 'sell-direction': '';
          }, 
          render(item){
            return Direction[item.Direction];
          }
        },{
          label: '开平',
          prop: 'CombOffsetFlag',
           render(item){
            return CombOffsetFlag[item.CombOffsetFlag];
          }
        },{
          label: '手数',
           type: 'number',
            prop: 'VolumeTotalOriginal'
        },{
          label: '报价',
          prop: 'LimitPrice',
           type: 'number',
          render(item){
            return item.LimitPrice.toFixed(3);
          }
        },{
          label: '状态',
          prop: 'OrderStatus',
          component: 'Status',
          width: 80,
          componentRender(item){
            const msg =  parseStatus.find(({key})=> key === item.OrderStatus);
            if(msg){
              return msg.msg
            }
          }
        },{
           label: '成交均价',
            prop: 'price',
            type: 'number',
            render(item){
              
              switch(item.OrderStatus){
                case '5':
                  return '0.000'
          
                case '0':
                case '1':
                case '2':
                  const {ExchangeID , OrderSysID} = item;
                  const traders = _this.traders.filter(trade=> trade.ExchangeID ===ExchangeID &&   trade.OrderSysID ===OrderSysID);
                  const price = {price: 0, volume : 0};
                  
                  traders.reduce(function(a, b){
                    const {Price, Volume} = b;
                    a.price =a.price + Price*Volume;
                    a.volume = price.volume+ Volume;
                    return a;
                  }, price);
                  let average = 0;
                  if(price.volume){
                    average = price.price/price.volume
                  }
                  return average.toFixed(3)
                default: 
                  return ''
              }
            }
        },{
           label: '详细信息',
            prop: 'StatusMsg',
           
            width: '200'
        }],
        }
    },
    props: ['tableData', 'traders']
})
</script>
