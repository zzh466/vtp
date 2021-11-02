<template>
  <div id="wrapper" v-loading='loading.length'  element-loading-text="正在获取账号信息">
    <main  v-if='!loading.length'>
      <div class="left-side">
          <div class="doc"> 
              <p v-for='ins in InstrumentIDs' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
              <p v-for='ins in ids' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
              <p v-for='ins in gz' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
              <button @click="open">商品</button>
              <button @click="open1">郑商所</button>
              <button @click="open2">股指</button>
              <button @click="stop">停止</button>
              <br><br>
           </div>
      </div>

      <div class="right-side">

        <el-table :data='orderData'   height="250" size='mini'
          border>
          <el-table-column
            v-for="column in orderColumns"
            :key='column.prop'
            :label="column.label"
            :prop="column.prop"
            :width="column.width"
            row-key='key'>
            <template v-if="column.render || column.component" scope="scope">
              <div v-if='column.render'>{{column.render(scope.row)}}</div>
              <component v-if='column.component' :is='column.component' :data='scope.row'></component>
            </template>
          </el-table-column>
        </el-table>

      </div>
    </main>
  </div>
</template>

<script>
  import Status from './LandingPage/Status'
  import { ipcRenderer } from 'electron';
  import request from '../utils/request';
  import {getWinName, Direction, CombOffsetFlag} from '../utils/utils';
  const StatusMsg = {
    props: ['data'],
    template: `<el-popover
                placement="left"
                width='150px'
                trigger="hover"
                :content="data.StatusMsg">
                <div slot="reference">{{data.StatusMsg.length> 4? data.StatusMsg.slice(0,4) + '……': data.StatusMsg}}</div>
              </el-popover>
            `
  }

  export default {
    name: 'landing-page',
    components: { Status, StatusMsg },
    computed: {
      priceData(){
        return this.$store.state.PriceData.priceData;
      },
      InstrumentIDs (){
        return this.$store.state.PriceData.InstrumentIDs
      },
      activeIns() {
         return this.$store.state.PriceData.activeIns
      },
      orderData(){
        
        const arr = []
        for(let key in this.orders){
          this.orders[key].key =key;
          arr.unshift(this.orders[key])
        }
        console.log(arr)
        return arr;
      },
      tradeData(){

      }
    },
    data(){
      const _this = this;
      return {
        ids: ['AP201','AP203','SM205','SM201'],
        gz:['IC2112','IF2112','IC2111','IF2111','IH2111'],
        orders: {},
        loading: ['order', 'trade', 'config', 'rate'],
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
            prop: 'VolumeTotalOriginal'
        },{
          label: '报价',
          prop: 'LimitPrice',
          render(item){
            return item.LimitPrice.toFixed(3);
          }
        },{
          label: '状态',
          prop: 'OrderStatus',
          component: 'Status'
        },{
           label: '成交均价',
            prop: 'price',
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
                    a.price = Price*Volume;
                    a.volume = price.volume+ Volume;
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
            component: 'StatusMsg',
            width: '100'
        }],
        traders: []
      }
    },
    mounted(){
      request({
        url: 'user/config',
        type: 'GET'
      }).then(res => {
       
        this.finishLoading('config')
      });
      ipcRenderer.send('trade-login', {});
      ipcRenderer.on('receive-order', (event, orders) =>{
    
        this.orders = orders;
      });
       ipcRenderer.on('receive-trade', (event, trader) =>{

        if(Array.isArray(trader)){
          this.traders = trader 
        }else {
          this.traders.push(trader)
        }
      });
       ipcRenderer.on('receive-rate', (event, rates) =>{
          this.rates = rates;
      });
      ipcRenderer.on('finish-loading', (event, arg) =>{
        
        this.finishLoading(arg);
      });
      ipcRenderer.on('confirm-settlement', (event, arg)=>{
        
      })
      setTimeout(()=>{
        if(this.loading.length !== 0){
          this.$alert('查询超时，不在交易时间或者网络连接有问题', '错误', {
            callback(){
              ipcRenderer.send('close-main');
            }
          })
        }
      }, 20000)
      
    },
    methods: {
      start(ins) {
        ipcRenderer.send('open-window', {id:ins, title: getWinName(ins)});
        this.$store.dispatch('updateIns', ins);
      },
      open () {
         ipcRenderer.send('start-receive', {host: '101.132.114.246', port: 18199, instrumentIDs: this.InstrumentIDs, type: 'SP',  iCmdID: 101});
      },
      open1(){
        ipcRenderer.send('start-receive', {host: '101.132.114.246', port: 19188, instrumentIDs: this.ids, type: 'SP', iCmdID: 101});
      },
      open2(){
        ipcRenderer.send('start-receive', {host: '117.185.41.78', port: 18198, instrumentIDs: this.gz, type: 'SP', iCmdID: 101});
      },
      stop(){
        ipcRenderer.send('stop-subscrible');
        this.$store.dispatch('updateIns', '')
      }, 
      finishLoading(tag){
        const index = this.loading.indexOf(tag);
        if(index > -1) {
          this.loading.splice(index, 1)
        }
      }
    }
  }
</script>

<style>
  @import url('https://fonts.googleapis.com/css?family=Source+Sans+Pro');

  * {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
  }

  body { font-family: 'Source Sans Pro', sans-serif; }

  #wrapper {
    height: 100vh;
    padding: 30px 40px;
    width: 100vw;
  }

  #logo {
    height: auto;
    margin-bottom: 20px;
    width: 420px;
  }

  main {
    display: flex;
    justify-content: space-between;
  }


  .left-side {
    display: flex;
    flex-direction: column;
    flex-basis: 30%; 
  }
  .right-side {
    display: flex;
    flex-direction: column;
    flex-basis: 70%; 
  }
  .welcome {
    color: #555;
    font-size: 23px;
    margin-bottom: 10px;
  }

  .title {
    color: #2c3e50;
    font-size: 20px;
    font-weight: bold;
    margin-bottom: 6px;
  }

  .title.alt {
    font-size: 18px;
    margin-bottom: 10px;
  }

  .doc p {
    color: black;
    margin-bottom: 10px;
  }

  .doc button {
    font-size: .8em;
    cursor: pointer;
    outline: none;
    padding: 0.75em 2em;
    border-radius: 2em;
    display: inline-block;
    color: #fff;
    background-color: #4fc08d;
    transition: all 0.15s ease;
    box-sizing: border-box;
    border: 1px solid #4fc08d;
  }

  .doc button.alt {
    color: #42b983;
    background-color: transparent;
  }
</style>
