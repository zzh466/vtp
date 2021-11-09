<template>
  <div id="wrapper" v-loading='loading.length'  element-loading-text="正在获取账号信息">
    <main  v-if='!loading.length'>
      <div class="left-side">
          <div class="label">订阅合约:</div>
          <Table @row-click='start' :tableData='instrumentsData' :columns= 'instrumentsColumns'/>
          <!-- <el-button @click="open">商品</el-button>
              <el-button @click="open1">郑商所</el-button>
              <el-button @click="open2">股指</el-button>
              <el-button @click="stop">停止</el-button> -->
      </div>

      <div class="right-side">
        <div>
          <div class="label">下单信息：</div>
            <Table  height='300' :columns='orderColumns' :tableData='orderData'/>
        </div>
        <div>
           <div class="label">回合信息：</div>
          <Round :data='traders' :rates='rates' :price='price' ></Round>
        </div>
      </div>
    </main>
    <el-dialog
      title="结算单确认"
      :visible.sync="dialogVisible"
      width="800px"
      center>
      <div class="confirm-info" v-html="confirmInfo.join('')">
      </div>
      <span slot="footer" class="dialog-footer">
        <el-button @click="cancel">取 消</el-button>
        <el-button type="primary" @click="confirm">确 定</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
  import { ipcRenderer } from 'electron';
  import request from '../utils/request';
  import {getWinName, Direction, CombOffsetFlag} from '../utils/utils';
  import Round from './LandingPage/round.vue';
  import Status from './LandingPage/Status.vue';
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
  export default {
    name: 'landing-page',
    components: {  Round },
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
        return arr;
      },
      subscribelInstruments(){
        const ExchangeId = this.InstrumentIDs.concat(this.ids, this.gz);
        return ExchangeId;
      },
      instrumentsData(){
        const data = this.subscribelInstruments.map(e=>({
          instrumentID: e,
          yesterdayBuy: 0,
          yesterdayAsk: 0,
          todayBuy: 0,
          todayAsk:0,
          todayVolume: 0,
          'todayCancel': 0
        }))
        this.orderData.filter(e => e.OrderStatus === '5').forEach(order => {
          const {InstrumentID} = order;
          const item = data.find(e => e.instrumentID === InstrumentID);
          if(item)item.todayCancel += 1;
        })
        const orderData = this.orderData.filter(e => e.OrderStatus === '0')
        // console.log(JSON.parse(JSON.stringify(traders)))
        this.traders.forEach(trader => {
           const {TradeTime, Direction, Volume, ExchangeID , OrderSysID, InstrumentID} = trader;
           const item = data.find(e => e.instrumentID === InstrumentID)
           if(!TradeTime){
            //  console.log(JSON.parse(JSON.stringify(trader)))
             const key = Direction === '0'? 'yesterdayBuy': 'yesterdayAsk';
              item[key] += Volume
           }else {
            // console.log(JSON.parse(JSON.stringify(trader)))
             const order = orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
             if(!order) return
              const {CombOffsetFlag} = order;
            if(CombOffsetFlag === '0'){
                const key = Direction === '0'? 'todayBuy': 'todayAsk';
                item[key] += Volume;
                item.todayVolume += Volume;
            }else {
               const keyYesterDay = Direction === '0'? 'yesterdayAsk': 'yesterdayBuy';
               const keyToady = Direction === '0'? 'todayAsk': 'todayBuy';
               if(item[keyYesterDay] >= Volume){
                 item[keyYesterDay] -= Volume
               }else {
                 item[keyToady] -= Volume;
               }
            }
           }
        })
        ipcRenderer.send('update-instrumentsData', data);
        return data
      }
    },
    data(){
      const _this = this;
      return {
        dialogVisible: false,
        ids: ['AP201','AP203','SM205','SM201'],
        gz:['IC2112','IF2112','IC2111','IF2111','IH2111'],
        orders: {},
        loading: ['order', 'trade', 'config', 'rate'],
        // loading: [],
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
        traders: [],
        confirmInfo: [],
        rates: [],
        price: {},
        instrumentsColumns:[{
          label: '合约',
          prop: 'instrumentID',
        },{
          label: '总多仓',
          prop: 'totalBuy',
          render(data){
            return data.yesterdayBuy+ data.todayBuy;
          }
        },{
          label: '总空仓',
          prop: 'totalAsk',
          render(data){
            return data.yesterdayAsk+ data.todayAsk;
          }
        },{
          label: '昨多仓',
          prop: 'yesterdayBuy',
        },{
          label: '昨空仓',
          prop: 'yesterdayAsk',
        },{
          label: '今多仓',
          prop: 'todayBuy',
        },{
          label: '今空仓',
          prop: 'todayAsk',
        },{
          label: '今开仓',
          prop: 'todayVolume',
        },{
          label: '今撤单',
          prop: 'todayCancel',
        }
        ],
      }
    },
    mounted(){
      request({
        url: 'user/config',
        type: 'GET'
      }).then(res => {
        this.open();
        this.open1();
        this.open2();
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
         console.log(rates)
          this.rates = rates;
      });
      ipcRenderer.on('finish-loading', (event, arg) =>{
        
        this.finishLoading(arg);
      });
      ipcRenderer.on('receive-info', (event, arg)=>{
        this.confirmInfo =  arg.map(({Content}) => Content)
        this.dialogVisible = true;
      })
       ipcRenderer.on('receive-price', (event, arg)=>{
        this.price=arg
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
      start(row) {
        const {instrumentID} = row
        ipcRenderer.send('open-window', {id:instrumentID, title: getWinName(instrumentID)});
        this.$store.dispatch('updateIns', instrumentID);
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
      },
      cancel(){
        this.dialogVisible = false;
        ipcRenderer.send('close-main');
      },
      confirm(){
        this.dialogVisible = false;
         ipcRenderer.send('confirm-settlement');
        
      },
      
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
  .label {
    margin: 10px 0;
  }
  .confirm-info{
    height: 400px;
    overflow-x: hidden;
    overflow-y: scroll;
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
    padding-left: 30px;
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
