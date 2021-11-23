<template>
  <div id="wrapper" v-loading='loading.length || forcing'  :element-loading-text="forcing?'触发强平操作，正在强平中':'正在获取账号信息'" >
    <el-descriptions :style="{fontSize: '20px'}" direction="vertical" :column="10" border>
      <el-descriptions-item label="账号">{{userData.account}}</el-descriptions-item>
      <el-descriptions-item label="交易日">{{account.TradingDay}}</el-descriptions-item>
    
      <el-descriptions-item label="总手续费">{{account.Commission.toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="总实际盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission).toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="强平线">{{userData.thrRealProfit}}</el-descriptions-item>
        <el-descriptions-item label="可用资金">{{account.Available.toFixed(2)}}</el-descriptions-item>
    </el-descriptions>
    <main  v-if='!loading.length'>
      
      <div class="left-side">
          <div class="label">订阅合约:</div>
          <Table height='600' @row-click='start' :tableData='instrumentsData' :columns= 'instrumentsColumns'/>
          <!-- <el-button @click="open">商品</el-button>
              <el-button @click="open1">郑商所</el-button>
              <el-button @click="open2">股指</el-button>  -->
              <!-- <el-button @click="forceClose">强平</el-button> -->
      </div>

      <div class="right-side">
        
        <div>
           <div class="label">回合信息：</div>
          <Round ref="round" :data='traderData' :rates='rates' :price='price' :instrumentInfo=instrumentInfo></Round>
        </div>
        <div>
          <div class="label">下单信息：</div>
            <Table  height='280' :columns='orderColumns' :tableData='orderData'/>
        </div>
      </div>
    </main>
    <el-dialog
      title="结算单确认"
      :visible.sync="dialogVisible"
      width="800px"
      :close-on-click-modal='false'
      :close-on-press-escape='false'
      :show-close='false'
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

  import {getWinName, Direction, CombOffsetFlag, getyyyyMMdd} from '../utils/utils';
  import {TraderSocket} from '../utils/request';
  import Round from './LandingPage/round.vue';
  import Status from './LandingPage/Status.vue';
  import Vue from 'vue';
  import request from "../utils/request"; 
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
  function coverZero(num){
    if(num < 0) return 0;
    return num
  }
  Vue.component('Status', Status);
  export default {
    name: 'landing-page',
    components: {  Round },
    watch:{
      'userData.locked'(val, old){
        if(val){
          this.stop()
        }
      }
    },
    computed: {
      userData() {
         return this.$store.state.user.userData
      },
      subscribelInstruments (){
        return this.userData.subInstruments.split(',')
      },
      activeIns() {
         return this.$store.state.PriceData.activeIns
      },
      locked: {
        get(){
          return this.$store.state.user.userData.locked;
        },
        set(){
          this.$store.commit('lock-user');
        }
      },
      orderData(){
        
        const arr = []
        for(let key in this.orders){
          this.orders[key].key =key;
          arr.unshift(this.orders[key])
        }
        return arr;
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
        //过滤
        const orderData = this.orderData.filter(e => {
        
          return  e.VolumeTraded
        })
        console.log(JSON.parse(JSON.stringify(this.traderData.filter(e => e.InstrumentID === 'v2205'))))
        this.traderData.forEach(trader => {
           const {TradeTime, Direction, Volume, ExchangeID , OrderSysID, InstrumentID} = trader;
           const item = data.find(e => e.instrumentID === InstrumentID)
           if(!item)return;
           if(!TradeTime){
            //  console.log(JSON.parse(JSON.stringify(trader)))
             const key = Direction === '0'? 'yesterdayBuy': 'yesterdayAsk';
              item[key] += Volume
           }else {
            
             const order = orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
             if(!order) {
               
               return
             }
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
      },
      traderData(){
        console.log(this.traders.map(e => e.TradeTime))
        return this.traders.slice().sort((a, b)=>{
              const date1 = a.OpenDate || a.TradeDate
              const date2 = b.OpenDate || b.TradeDate
              return date1 - date2
          })
      }
    },
    data(){
      const _this = this;
      return {
        dialogVisible: false,
        orders: {},
        loading: ['order', 'trade', 'config', 'rate', 'instrument'],
        loading: [],
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
          component: 'Status',
          width: 80,
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
        traders: [],
        confirmInfo: [],
        rates: [],
        price: {},
        instrumentInfo: [],
        account: {
          Commission: 0,
          CloseProfit: 0,
          PositionProfit: 0,
          Available: 0
        },
        instrumentsColumns:[{
          label: '合约',
          prop: 'instrumentID',
        },{
          label: '总多仓',
          prop: 'totalBuy',
          width: 50,
          render(data){
            return coverZero(data.yesterdayBuy+ data.todayBuy);
          }
        },{
          label: '总空仓',
          prop: 'totalAsk',
           width: 50,
          render(data){
            return coverZero(data.yesterdayAsk+ data.todayAsk);
          }
        },{
          label: '昨多仓',
          prop: 'yesterdayBuy',
           width: 50,
        },{
          label: '昨空仓',
          prop: 'yesterdayAsk',
           width: 50,
        },{
          label: '今多仓',
          prop: 'todayBuy',
           width: 50,
          render(data){
            return coverZero(data.todayBuy);
          }
        },{
          label: '今空仓',
          prop: 'todayAsk',
           width: 50,
          render(data){
            return coverZero(data.todayAsk);
          }
        },{
          label: '今开仓',
          prop: 'todayVolume',
           width: 50,
        },{
          label: '今撤单',
          prop: 'todayCancel',
           width: 50,
        }
        ],
        forcing: false
      }
    },
    mounted(){
      
      this.$store.dispatch('get-config').then(()=>{
        
        const userData = this.userData;
        const broadcast = this.$store.state.user.broadcast;
        if(broadcast && !this.ws){
          this.ws = new TraderSocket();
          this.ws.onmessage((e)=>{
            console.log(e.data)
            ipcRenderer.send('broadcast-openinterest', e.data);
          })
        }
        const {
          tradeAddr:ctp1_TradeAddress,
          quotAddr,
          brokerId: m_BrokerId,
          authCode: m_AuthCode,
          futureUserId: m_InvestorId,
          futureUserId: m_UserId,
          futureUserPwd:m_PassWord,
          appId: m_AppId,
          futureUserId:m_AccountId
        } = userData;
         this.finishLoading('config')
         ipcRenderer.send('trade-login', {
           ctp1_TradeAddress,
            m_BrokerId,
            m_AuthCode,
            m_InvestorId,
            m_UserId,
            m_PassWord,
            m_AppId,
            m_AccountId,
           instruments: this.subscribelInstruments
          });
          const _quotAddr = quotAddr.split(':')
          ipcRenderer.send('start-receive', {host: _quotAddr[0], port: _quotAddr[1], instrumentIDs: this.subscribelInstruments,   iCmdID: 101});
      })
     
      ipcRenderer.on('receive-order', (event, orders) =>{
    
        this.orders = orders;
      });
       ipcRenderer.on('receive-trade', (event, trader) =>{
        if(this.ws && this.traders.length){
          const {futureUserId} = this.userData
          let { ExchangeID, OrderSysID, TradeID, InstrumentID,Volume ,Direction, TradeTime} = trader[trader.length -1];
          if(!TradeTime ){
            return
          }
        
          if(Direction === '1'){
            Volume = -Volume;
          }
          this.ws.send(`${futureUserId}-${ExchangeID}-${OrderSysID}-${TradeID}:${InstrumentID}:${Volume}`)
        }
         

        if(Array.isArray(trader)){
          
          this.traders = trader 
        }
      });
       ipcRenderer.on('receive-rate', (event, rates) =>{
        //  console.log(rates)
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
         if(this.instrumentsData.some(({todayAsk, todayBuy, yesterdayBuy, yesterdayAsk})=> todayAsk+ yesterdayAsk!== yesterdayBuy+ todayBuy)){
            this.price=arg;
         }
       
      })
       ipcRenderer.on('receive-account', (event, arg)=>{
         if(!this.locked && this.userData.thrRealProfit && arg.CloseProfit + arg.PositionProfit - arg.Commission < -this.userData.thrRealProfit){
           if(!this.forceCloseTime){
             this.forceCloseTime = setTimeout(()=> this.forceClose(), 2200)
           }
           
         }else{
           clearTimeout(this.forceCloseTime)
           this.forceCloseTime = null;
         }
        this.account=arg
     
        const realProfit = arg.CloseProfit + arg.PositionProfit - arg.Commission;
        const staticBalance = arg.PreBalance + arg.Mortgage + arg.PreFundMortgageIn + arg.FundMortgageIn + arg.FundMortgageOut + arg.FundMortgageAvailable - arg.PreFundMortgageOut - arg.PreCredit - arg.PreMortgage;
        const data ={
            date: getyyyyMMdd(),
            commission: arg.Commission,
            realProfit,
            margin: arg.CurrMargin,
            available: arg.Available,
            balance: arg.Balance,
            closeProfit: arg.CloseProfit,
            orderVolume: this.instrumentsData.reduce((a,b) => a+ b.todayVolume, 0),
            openVolume: this.orderData.length,
            staticBalance,
            positionProfit: arg.PositionProfit,
            userId: this.userData.account
          }
         
        request({
          url: '/user/funtureAccountInfo ',
          method: 'PATCH',
          data
        })
      })
      ipcRenderer.on('receive-instrument', (event, arg)=>{
        // console.log(arg)

        this.instrumentInfo = arg
      })
      setTimeout(()=>{
        if(this.loading.length !== 0){
          this.$alert('查询超时，不在交易时间或者网络连接有问题', '错误', {
            callback(){
              ipcRenderer.send('close-main');
            }
          })
        }
      }, 60000)
      
    },
    methods: {
      start(row) {
        if(this.locked){
          this.$alert('当前账号已锁定', '锁定' )
          return
        }
        const {instrumentID} = row
        ipcRenderer.send('open-window', {id:instrumentID, title: getWinName(instrumentID), account: this.userData.account});
        this.$store.dispatch('updateIns', instrumentID);
      },
      stop(){
        ipcRenderer.send('stop-subscrible');
        // this.$store.dispatch('updateIns', '')
      }, 
      closeALL(){
        ipcRenderer.send('cancel-order');
        const arr = this.$refs.round.traderData.filter(({CloseVolume, Volume}) => Volume> CloseVolume);
        if(arr.length){
          arr.forEach(({CloseVolume, Volume,  InstrumentID, OrderSysID, ExchangeID, Direction}) => {
              const order = this.orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
              const instrumentinfo = this.instrumentInfo.find(e => e.id === InstrumentID)
              const { PriceTick } = instrumentinfo.field;
              let combOffsetFlag = '1';
              if(order) {
                combOffsetFlag = order.CombOffsetFlag
              }
              const direction= Direction==='0'? '1': '0'
              let over_price = this.$store.state.user.over_price * PriceTick;
              if(direction === '1'){
                over_price = -over_price;
              }
       
              const limitPrice = this.price[InstrumentID][direction] + over_price;
              ipcRenderer.send('trade', {limitPrice, instrumentID: InstrumentID, combOffsetFlag, volumeTotalOriginal: Volume- CloseVolume, direction}) 
          })
        }else{
           this.forcing = false;
           clearInterval(this.forcingCloseTime)
        }
      },
      async forceClose(){
       
        this.forcing  = true;
        this.locked = true;
        const force = await this.$store.dispatch('lock');
        
        if(force){
          this.forcingCloseTime = setInterval(()=> {
            this.closeALL();
          }, 1000)
        }else{
          setTimeout(()=> this.forcing = false, 500 )
        }
        
      },
      finishLoading(tag){
        const index = this.loading.indexOf(tag);
        // console.log(tag)
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
    padding: 10px 10px;
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
  .cell {
    padding: 0 4px !important;
  }
  .el-table__cell {
    padding: 2px 0 !important;;
  }
</style>
