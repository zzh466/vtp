<template>
  <div id="wrapper" v-loading='loading.length || forcing'  :element-loading-text="forcing?'触发强平操作，正在强平中':'正在获取账号信息'" >
    <el-descriptions size= 'samll' direction="vertical" :column="12" border class="account">
      <el-descriptions-item label="账号">{{userData.userAccount}}</el-descriptions-item>
      <el-descriptions-item label="账户状态"><span :style="{color:locked?'red': 'green'}">{{locked?'锁定': '正常'}}</span></el-descriptions-item>
     
      <el-descriptions-item label="交易日">{{account.TradingDay}}</el-descriptions-item>
      <el-descriptions-item label="当前账户">{{currentAccount.m_AccountId}}</el-descriptions-item>
      <el-descriptions-item label="appId">{{currentAccount.m_AppId}}</el-descriptions-item>
      <el-descriptions-item label="期货账户状态"><span :style="{color:accountStatus?'green': 'red'}">{{accountStatus?'在线': '离线'}}</span></el-descriptions-item>
      <el-descriptions-item label="手续费">{{account.Commission.toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="当前账户盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission).toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="隔节误差">{{deviation.toFixed(2)}}</el-descriptions-item>
       <el-descriptions-item label="实际盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission + deviation).toFixed(2)}}</el-descriptions-item>
      <!-- <el-descriptions-item label="强平线">{{userData.thrRealProfit}}</el-descriptions-item>
      <el-descriptions-item label="可用资金">{{Math.floor(account.Available/ 1000)*1000 }}</el-descriptions-item> -->
       <el-descriptions-item label="总实际盈亏">{{totalProfit}}</el-descriptions-item>
        <!-- <el-descriptions-item v-if="userData.futureAccountVOList.length > 1"><el-button type="primary" size="small" @click="changeAccount">切换账号</el-button></el-descriptions-item> -->
    </el-descriptions>
      
    <main >
      
      <div class="left-side">
        
         <div>
         
           <div class="label">回合信息： <el-button type="primary" size="small" @click="exportroud">导出</el-button>  <el-button  type="primary" style="margin-left: 20px" size="small" @click="puppetReconnect">重连</el-button></div>
           
          <Round ref="round" :data='traderData' :rates='rates'  :price='price' :instrumentInfo='instrumentInfo' :positions="positions" @history-trade="historyTraders = $event"></Round>
        </div>
         
      </div>

      <div class="right-side">
        
       
        <div>
          <div class="label">下单信息：<el-button type="primary" size="small" @click="exportorder">导出</el-button></div>
            <Order ref="order" :traders="traders" :tableData='orderData'/>
        </div>
      </div>
     
    </main>
   
    <el-button type="primary" @click=" timedialogVisible = true;confirmLoading= false">结算单查询</el-button>
    <el-button type="primary" v-if="userData.userAccount.toLowerCase() === 'xqlh'" @click=" historydialogVisible = true">历史成交查询 </el-button>
    <!-- <el-button type="primary" @click="updateConfig">更新配置</el-button> -->
        <p >账户昨仓合约：{{positionsList }}</p>
       <div class="label">订阅合约： <el-button type="primary" style="margin-left: 20px" size="small" @click="reconnect">强制重连</el-button></div>
      <div style="display: flex;">
         
        <div v-for='instrument in subscribelInstruments' :key ='instrument.id' :style="{width: 100 / subscribelInstruments.length + '%', marginRight: '4px'}">
          <Table    height='300' @row-dblclick='start($event, instrument.id)' :tableData='instrumentsData | changeNo(instrument.id)' :columns= 'instrumentsColumns'/>
        </div>
          <!-- <el-button @click="open">商品</el-button>
              <el-button @click="open1">郑商所</el-button>
              <el-button @click="open2">股指</el-button>  -->
              <!-- <el-button @click="forceClose">强平</el-button> -->
      </div>
      <el-dialog
      title="结算单查询"
      :visible.sync="timedialogVisible"
      >
        <el-date-picker
         value-format='yyyyMMdd'
          v-model="confimInfoDate"
          placeholder="请选择查询日期"
        ></el-date-picker>
      <span slot="footer" class="dialog-footer">
        <el-button @click="timedialogVisible=false">取 消</el-button>
        <el-button type="primary" @click="confirmTimeqry" :loading="confirmLoading">确 定</el-button>
      </span>
      </el-dialog>
      <el-dialog
      title="历史成交查询"
      :visible.sync="historydialogVisible"
       width="1000px"
      >
        <div style="margin-bottom: 5px;">
        <el-date-picker
       
          v-model="historyDate"
           type="daterange"
           start-placeholder="开始日期"
           end-placeholder="结束日期">
        ></el-date-picker>
        
        <el-button type="primary" @click="historyQry" :loading="historyLoading">查询</el-button>
        <el-button type="primary" @click="exporthistroy" >导出</el-button>
        <span>共：{{ historyData.length }} 条</span>
      </div>
        <Table height='400' :tableData="historyData" :loading="historyLoading" :columns="historyColumns" ></Table>
      </el-dialog>
    <el-dialog
      title="结算单确认"
      :visible.sync="dialogVisible"
      width="800px"
      :close-on-click-modal='false'
      :close-on-press-escape='false'
      :show-close='false'
      center>
      <div class="confirm-info" v-html="confirmInfo">
      </div>
      <span slot="footer" class="dialog-footer">
        <el-button @click="cancel">取 消</el-button>
        <el-button type="primary" @click="confirm" >确 定</el-button>
      </span>
    </el-dialog>
    <el-dialog :close-on-press-escape='false' :show-close='false' :visible="loginVisible" :close-on-click-modal='false' title="重新登陆"> 
      <loginform @login='relogin' :userAccount ='userData.userAccount'/>
    </el-dialog>
  </div>
</template>

<script>
  import { ipcRenderer } from 'electron';

  import {getWinName, getyyyyMMdd, getHoldCondition, getClientSize, specialExchangeId, subscribeIndicatorKey, speak} from '../utils/utils';
  import request, {TraderSocket}  from '../utils/request';
  import {parseComfirm, columns as historyColumns} from '../utils/parse'
  import Round from './LandingPage/round.vue';
   import Order from './LandingPage/Order.vue';

  import loginform from './Login/form.vue'

 

  function coverZero(num){
    // if(num < 0) return 0;
    return num
  }
  export default {
    name: 'landing-page',
    components: {  Round , loginform, Order},
    watch:{
      locked(val, old){
        if(val && !old){
          this.forceClose()
          ipcRenderer.send('change-lock', true)
        }
        if(!val){
          ipcRenderer.send('change-lock', false)
          this.forceCloseCount = 0
        }
      }
    },
      filters: {
      changeNo(instruments, id){
        
        return instruments.filter(e=> e.id.includes(id)).sort((a,b) =>{
          const a_i = a.instrumentID.match(/[a-zA-Z]+/)[0];
          const b_i = b.instrumentID.match(/[a-zA-Z]+/)[0];
          if (a_i < b_i) {
              return -1;
          }
          if (a_i > b_i) {
              return 1;
          }
         
          if(a_i === b_i){
            return a.instrumentID.match(/[0-9]+/)[0] - b.instrumentID.match(/[0-9]+/)[0]
          }
        });
      }
    },
    computed: {
      userData() {
         return this.$store.state.user.userData
      },
      subscribelInstruments (){
        
        // return this.userData.subInstruments.split(',').map(e => e.replace(/[\n\r]/g, ''))
        
         return this.userData.instrumentConfigVOList.map(e=> ({
          instruments: (e.instruments|| '').split(',').map(e => e.replace(/[\n\r]/g, '')).filter(e=>e),
          id: e.id
        }))
      },
      openvolume_limit(){
          return this.$store.state.user.openvolume_limit.split(';').filter(e=>e).map(e => {
          const msg= e.split(':')
          return {
            instrumentID: msg[0],
            limit : msg[1]
          }
        })
      },
      vtp_client_big_cancelvolume_limit(){
        return this.$store.state.user.vtp_client_big_cancelvolume_limit.split(';').filter(e=>e).map(e =>{
          const msg = e.split(':')
          const exchangeId = msg[0];
          const limit = msg[1].split('-');
          return {
            exchangeId,
            volume: limit[0],
            limit: limit[1]
          }
        })  
      },
      vtp_client_cancelvolume_limit(){
          return this.$store.state.user.vtp_client_cancelvolume_limit.split(';').filter(e=>e).map(e => {
            const msg= e.split(':')
            return {
              instrumentID: msg[0],
              limit : msg[1]
            }
          })
      },
      opened() {
         return this.$store.state.PriceData.InstrumentIDs
      },
      locked: {
        get(){
          return this.$store.state.user.userData.locked;
        },
        set(){
          this.$store.commit('lock-user');
        }
      },
      positionsList(){
        return this.positions.reduce((a,b) => {
          if(!a.includes(b.InstrumentID)){
            a.push(b.InstrumentID)
          }
          return a
        }, []).join(',')
      }
   
    },
    data(){
      const _this = this;
      return {

        dialogVisible: false,
        timedialogVisible: false,
        historydialogVisible: false,
        historyLoading: false,
        historyDate: [],
        confirmLoading: true,
        confimInfoDate: '',
        orders: {},
        loading: ['order', 'trade', 'config'],
        orderData: [],
        instrumentsData: [],
        traderData: [],
        // loading: [],
        traders: [],
        positions:[],
        confirmInfo: '',
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
          fixed: 'left'
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
        },
        {
          label: '开仓限制',
          prop: 'openvolume_limit',
          width: 50
        },{
          label: '今撤单',
          prop: 'todayCancel',
           width: 50,
        },
        {
          label: '撤单限制',
          prop: 'vtp_client_cancelvolume_limit',
          width: 50
        },{
          label: '今大额撤单',
          prop: 'big_todayCancel',
           width: 50,
        },
        {
          label: '大额撤单限制',
          prop: 'big_todayCancel_limit',
          width: 60
        }
        ],
        forcing: false,
        currentAccount: {},
        totalProfit: 0,
        deviation: 0,
         loginVisible: false,
         accountStatus: false,
         historyData: []
      }
    },
    created(){
      this.forceCloseCount = 0;
      this.confirmquerycount=0;
      this.historyTraders = [];
      this.historyColumns = historyColumns;
      let audio = new Audio()
      audio.src = __static+ "/trade.wav";
      this.updateConfig().then(()=>{
        
        const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`));
        
        const broadcast = config.some(e => e.broadcastOpenInterest);
        this.ws = new TraderSocket(this.userData.id, this.$store.state.user.activeCtpaccount);
        if(broadcast ){
          this.ws.onmessage((e)=>{
            console.log(e)
            ipcRenderer.send('broadcast-openinterest', e);
          })
        }
        this.ws.closeTrade = (instruments) => {
          let info = this.instrumentInfo;
          if(instruments) {
            instruments = instruments.split(',')
            info = info.filter(e => instruments.indexOf(e.InstrumentID.match(/^[a-zA-Z]+/)[0]) > -1)
          }
          ipcRenderer.send('force-close', {over_price:  this.$store.state.user.over_price, instrumentInfo: info})
        }
        
         
        this.ws.initTask.push(`NotifyIndicatorBroadcast@${!!this.userData._datachecked}`)
        
        window.$$ws = this.ws
        const activeArr = [];
     
        this.ws.onActiveInstrument((e, needfiter, notifi) =>{
          if(needfiter && !this.subscribelInstruments.find(a => a.instruments.includes(e))){
             return;
          }
          //因为会出现行情重复提醒情况 所以要做过滤
          // if(activeArr.includes(e))return
          // activeArr.push(e)
          // setTimeout(()=>{ 
          //   const index = activeArr.indexOf(e);
          //   if(index >-1){
          //     activeArr.splice(index,1)
          //   }
          // }, 10*60*1000)
          // console.log(e)
         
          if(!notifi){
            notifi = '来大行情了'
          }
          const text = `合约${e}${notifi}`
          //部分合约TA bu等会被当成拼音 所以加个空格
          speak(`合约${e.replace(/^([a-zA-Z])(?=[a-zA-Z])/, '$1 ')}${notifi}`);
          
          // ipcRenderer.send('instrument-notification', e, text)
          let notification =  new Notification('通知', {body: text})
          notification.onclose = () => {
            console.log('close 111111111')
            notification = null
          }
          ipcRenderer.send('info-log', `触发行情弹窗 ${text}`)
          notification.onclick = () => {
            console.log('click 111111111')
            if( this.loading.length)return;
         
              let  config = this.subscribelInstruments.find(a => a.instruments.includes(e))
              
              if(!config){
                
              
                const configs = this.userData.instrumentConfigVOList.slice();
                
                const instruments = configs[0].instruments;
                configs[0] = {...configs[0], instruments: instruments? instruments+ `,${e}`: e};
                this.$store.commit('update-config', configs);
                const openvolume_limit = this.openvolume_limit
                config = configs[0]
                const vtp_client_cancelvolume_limit = this.vtp_client_cancelvolume_limit
                const big_todayCancel_limit = this.getBigCancelLimit(e.ins);
                const data = {
                  instrumentID: e,
                  id: [configs[0].id],
                  yesterdayBuy: 0,
                  yesterdayAsk: 0,
                  todayBuy: 0,
                  todayAsk:0,
                  todayVolume: 0,
                  'todayCancel': 0,
                  openvolume_limit: this.findLimit(openvolume_limit, e.ins),
                  vtp_client_cancelvolume_limit: this.findLimit(vtp_client_cancelvolume_limit, e.ins),
                  big_todayCancel: 0,
                  big_todayCancel_limit_volume: big_todayCancel_limit.volume,
                  big_todayCancel_limit: big_todayCancel_limit.limit
                }
                ipcRenderer.send('add-sub-instruments', e)
                this.instrumentsData.push(data);
      
              }
              this.start({row:{instrumentID: e}}, config.id)
          }
       
        })
        this.login()
         
      })
      this.getCtpInfo();
      // ipcRenderer.on('open-noti-ins', (event,e)=>{
        
      //   if(this.opened.includes(e) || this.loading.length)return;
         
      //   let  config = this.subscribelInstruments.find(a => a.instruments.includes(e))
        
      //   if(!config){
          
        
      //     const configs = this.userData.instrumentConfigVOList.slice();
          
      //     const instruments = configs[0].instruments;
      //     configs[0] = {...configs[0], instruments: instruments? instruments+ `,${e}`: e};
      //     this.$store.commit('update-config', configs);
      //     const openvolume_limit = this.openvolume_limit
      //     config = configs[0]
      //     const vtp_client_cancelvolume_limit = this.vtp_client_cancelvolume_limit
      //     const data = {
      //       instrumentID: e,
      //       id: [configs[0].id],
      //       yesterdayBuy: 0,
      //       yesterdayAsk: 0,
      //       todayBuy: 0,
      //       todayAsk:0,
      //       todayVolume: 0,
      //       'todayCancel': 0,
      //       openvolume_limit: (openvolume_limit.find(({instrumentID})=> instrumentID ===e) || {limit: "无"}).limit,
      //       vtp_client_cancelvolume_limit:  (vtp_client_cancelvolume_limit.find(({instrumentID})=> e.includes(instrumentID)) || {limit: "无"}).limit
      //     }
      //     ipcRenderer.send('add-sub-instruments', e)
      //     this.instrumentsData.push(data);

      //   }
      //   this.start({row:{instrumentID: e}}, config.id)
      // })
      ipcRenderer.on('receive-order', (event, orders, key, needUpdate) =>{
        
        console.log(orders, key, needUpdate)
       if(key){
         
         this.upddateOrder(orders, key, needUpdate)
       }else{
         this.orders = orders;
         this.finishLoading('order');
       }
        
      });
      ipcRenderer.on('account-connect',  (event, status) =>{
        this.accountStatus = status;
      })
       ipcRenderer.on('receive-position', (event, position) =>{
        // console.log(position.filter(a => a.InstrumentID==='IC2201'))
        console.log(position)
        this.positions = position;
        this.finishLoading('position')
      });
      ipcRenderer.on('force-close-finish', ()=>{
        this.forcing = false
      })
       ipcRenderer.on('receive-trade', (event, trader) =>{
        if(Array.isArray(trader)){
          
          this.traders = trader 
        }else{
        
          audio.load();
          audio.play();
          
          this.updateTrader(trader);
          // const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`));
       
          // if(config.find(e=> e.instruments.includes(trader.InstrumentID)).broadcastOpenInterest){
          //   const futureUserId = this.$store.state.user.activeCtpaccount
          
          //   let { ExchangeID, OrderSysID, TradeID, InstrumentID,Volume ,Direction, TradeTime, TradingDay} = trader;
          //   if(!TradeTime ){
          //     return
          //   }else {
              
          //     const time = +new Date(`${(new Date()).toDateString()} ${TradeTime}`);
          //     if(Math.abs(time - new Date()) > 1000 * 30 *60 )return;
          //   }
          
          //   if(Direction === '1'){
          //     Volume = -Volume;
          //   }
          //   this.ws.send(`${futureUserId}-${ExchangeID}-${OrderSysID}-${TradeID}:${InstrumentID}:${Volume}`)
          // }
        }
        console.log(trader, '2336456')
        
      });
       ipcRenderer.on('receive-rate', (event, rates) =>{
        
          this.rates = rates.sort((a,b)=> b.InstrumentID.length - a.InstrumentID.length);
           console.log(this.rates.map(a => a.InstrumentID))
      });
      ipcRenderer.on('finish-loading', (event, arg) =>{
        
        this.finishLoading(arg);
      });
       ipcRenderer.on('add-loading', (event, arg) =>{
        
         const index = this.loading.indexOf(arg);
        // console.log(tag)
        if(index  === -1) {

          this.loading.push(arg)
            ipcRenderer.send('info-log', `增加查询${arg}`)
        }
      });
      ipcRenderer.on('receive-info', (event, arg)=>{
    
       
        const info = arg.map(({Content}) => Content).join('')
        if(this.historyLoading && this.historydialogVisible){
          if(!info && info.length < 100){
            return
          }
          
          console.log(this.historyLoadingTimeout, 'sssss')
          clearTimeout(this.historyLoadingTimeout )
          const {createDate, data} = parseComfirm(info)
          console.log(createDate, data)
          if(createDate === getyyyyMMdd(this.historyDate[1])){
            this.historyLoading =false
          }else{
            //万一选的最后一天周末 会没有 所有要加保底定时器
            console.log(this.historyLoadingTimeout, 'bbbbb')
            this.historyLoadingTimeout = setTimeout(()=>{
              
              this.historyLoading =false
            },30000)
          }
          if(data.length){
            this.historyData = this.historyData.concat(data);
          }
          
          //不太好判断是不是最后一天
          
      
          return;
        }
        this.confirmInfo =  info
        this.dialogVisible = true;
        this.timedialogVisible = false;
      })
       ipcRenderer.on('receive-price', (event, arg)=>{    
        //计算隔节误差;
        console.log(this.historyTraders, '12321313')
        if(this.historyTraders.length){
          
          for(let i = this.historyTraders.length - 1 ; i >= 0; i--){
           
            const {InstrumentID, Volume, Direction, TradeDate , ExchangeID } = this.historyTraders[i];
            const priceData = arg[InstrumentID];
            
            const info = this.instrumentInfo.find(e => InstrumentID === e.InstrumentID);
             console.log(priceData, info, 123136)
            if(priceData && info){
              
              let price;

              
              let { SettlementPrice, ClosePrice, PreClosePrice, PreSettlementPrice , TradingDay, UpdateTime} = priceData[4];
              
              if(ExchangeID === 'CFFEX'){
                const _priceData = this.$store.state.user.instrument_info_pre_tradday[InstrumentID];
                PreSettlementPrice = _priceData.SettlementPrice;
                SettlementPrice = _priceData.SettlementPrice;
                ClosePrice = _priceData.ClosePrice;
                PreClosePrice = _priceData.ClosePrice;
              }
              const current = new Date().getHours()
              if(UpdateTime.length < 8){
                UpdateTime='0' + UpdateTime
              }
              // if(((updateHour >= 15 && updateHour < 20) || (updateHour < 9 && updateHour > 3)) &&  (current < 15 ||current > 17))continue;
              if(((current > 18||current < 3) && UpdateTime < '20:59:00') || (current >3 && (UpdateTime > '02:30:00' && UpdateTime < '08:59:00')))continue;
            
                if(TradeDate  < TradingDay){
                  price = PreSettlementPrice - PreClosePrice
                }else {
                  price =  SettlementPrice - ClosePrice
                }
    
              
              let profit = price*Volume * info.VolumeMultiple
              if(Direction === '1'){
                profit = -profit;
              }
              this.deviation += profit;
              this.historyTraders.splice(i, 1)
            }
          }
        }
         if(this.instrumentsData.some(({todayAsk, todayBuy, yesterdayBuy, yesterdayAsk})=> todayAsk+ yesterdayAsk!== yesterdayBuy+ todayBuy)){
           
            this.price=arg;
         }
       
      })
       ipcRenderer.on('receive-account', (event, arg)=>{
        // console.log(  this.loading, arg)
         if(!arg || this.loginVisible || this.loading.length)return;
        //  console.log(arg)
         this.account=arg
        

        const realProfit = arg.CloseProfit + arg.PositionProfit - arg.Commission+ this.deviation;
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
            deviation: this.deviation,
            id: this.$store.state.user.activeCtpaccount
          }
         
        request({
          url:  '/future/futureAccountTradingInfo',
          method: 'PATCH',
          data
        }).then(res => {
          if(res.code === 'LOGIN_UNFINISHED'){
            this.loginVisible = true
            return
          }
          this.totalProfit =  (res.futureAccountVOList || []).reduce((a,b) => a + b.realProfit, 0).toFixed(2) 
           if(!this.locked){
            if( this.userData.thrRealProfit && this.totalProfit < -this.userData.thrRealProfit){
              
              if(this.forceCloseCount > 2){
                ipcRenderer.send('err-log', `盈亏超过强平两次 触发强平操作${this.totalProfit}`)
                this.$store.dispatch('lock');
                this.locked = true
              }else {
                this.forceCloseCount ++
              }
            }else{
             
            
              this.forceCloseCount = 0;
            }
         }
        
        
        })
      })
      // ipcRenderer.on('receive-instrument', (event, arg)=>{
      //   // console.log(arg)

      //   this.instrumentInfo = arg
      // })
      ipcRenderer.on('update-config', (event, arg)=>{
        // console.log(arg)
        
        this.$store.commit('update-config',arg);
        this.$nextTick(()=>{
          this.init()
        })
      })
    
      this.timoutquery = setTimeout(()=>{
        console.log(this.loading)
        if(this.loading.length !== 0){

          this.$alert('查询超时，不在交易时间或者网络连接有问题', '错误', {
            callback:()=>{
              ipcRenderer.send('close-main', this.loading.toString());
            }
          })
        }
      }, 90000)
      
    },
    methods: {
      exportroud(){
        const {traderColumns, traderData} =  this.$refs.round
        
        this.exportExcel('回合信息', traderData, traderColumns)
      },
      exportorder(){
         const {orderColumns} =  this.$refs.order
        this.exportExcel('下单信息', this.orderData, orderColumns)
      },
      exporthistroy(){
        if(this.historyData.length){
          this.exportExcel(`${getyyyyMMdd(this.historyDate[0])}-${getyyyyMMdd(this.historyDate[1])}成交记录`,  this.historyData, historyColumns);
        }
      },
      init(){
        
        console.log(1111111111111111111111111111111)
        
        const arr = []
        for(let key in this.orders){
          this.orders[key].key =key;
          arr.unshift(this.orders[key])
        }
        this.orderData = arr;
        
        this.traderData = this.positions.concat(this.traders);

    
        const openvolume_limit = this.openvolume_limit
        
        const vtp_client_cancelvolume_limit = this.vtp_client_cancelvolume_limit
        const data = this.subscribelInstruments.reduce((a,b)=> {
          b.instruments.forEach(e =>{
            const repeat = a.find(c=> c.ins===e)
            if(repeat){
              repeat.id.push(b.id)
            }else{
              a.push(({ins:e, id: [b.id]}))
            }
          })
          return a;
          }
          , []).map(e=>{
          const big_todayCancel_limit = this.getBigCancelLimit(e.ins);
          return {
          instrumentID: e.ins,
          id: e.id,
          yesterdayBuy: 0,
          yesterdayAsk: 0,
          todayBuy: 0,
          todayAsk:0,
          todayVolume: 0,
          'todayCancel': 0,
          openvolume_limit: this.findLimit(openvolume_limit, e.ins),
          vtp_client_cancelvolume_limit: this.findLimit(vtp_client_cancelvolume_limit, e.ins),
          big_todayCancel:0,
          big_todayCancel_limit_volume: big_todayCancel_limit.volume,
          big_todayCancel_limit: big_todayCancel_limit.limit


        }})
        
        this.orderData.filter(e => e.OrderStatus === '5').forEach(order => {
          const {InstrumentID} = order;
          const item = data.find(e => e.instrumentID === InstrumentID);
          if(!item) return
          item.todayCancel += 1;
          if(item.big_todayCancel_limit_volume && order.VolumeTotal - order.VolumeTraded >= item.big_todayCancel_limit_volume) item.big_todayCancel += 1
        })
        //过滤
        const orderData = this.orderData;
        // console.log(JSON.parse(JSON.stringify(this.traderData.filter(e => e.InstrumentID === 'v2412'))))
        this.traderData.forEach(trader => {
           this.setTradeItem(trader, orderData, data);
        })
        ipcRenderer.send('update-instrumentsData', data, true);
        this.instrumentsData = data;
        this.deviation = 0;
        //重连时间太长导致遮罩层bug关闭不了
        const a = document.querySelectorAll('.v-modal')
        a.forEach(e => e.remove());
        this.$nextTick(function(){
          this.$refs.round.init();
    
        })
      },
      setTradeItem(trader, orderData, data= this.instrumentsData){
        const {TradeTime, Direction, Volume, ExchangeID , OrderSysID, InstrumentID} = trader;
           let CombOffsetFlag;
           if(TradeTime){
              const order = orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
            if(order) {
              CombOffsetFlag = order.CombOffsetFlag;
              trader.CombOffsetFlag = CombOffsetFlag;
            }
           }
           
           
           const item = data.find(e => e.instrumentID === InstrumentID)
           if(!item)return;
           if(!TradeTime){
            //  console.log(JSON.parse(JSON.stringify(trader)))
             const key = Direction === '0'? 'yesterdayBuy': 'yesterdayAsk';
              item[key] += Volume
           }else {
            
             if(!CombOffsetFlag){
                ipcRenderer.send('err-log', `trader找不到order， ${JSON.stringify(trader)}`);
                //找不到是可能是网络波动导致trader比order更早返回，延迟一段时间后重新渲染
                setTimeout(()=>{
                    this.setTradeItem(trader, this.orderData, data)
                }, 500)
                return
             }
            if(CombOffsetFlag === '0' ){
                const key = Direction === '0'? 'todayBuy': 'todayAsk';
                item[key] += Volume;
                item.todayVolume += Volume;
                trader.open = Volume;
            }else {
               let keyYesterDay = Direction === '0'? 'yesterdayAsk': 'yesterdayBuy';
               let keyToady = Direction === '0'? 'todayAsk': 'todayBuy';
               let close = 'close';
               let closeToady = 'closeToady'
                  //中金先平今再平昨

              if(ExchangeID === 'CFFEX'){
                const temp= keyToady;
                keyToady = keyYesterDay;
                keyYesterDay = temp
                close ='closeToady';
                closeToady = 'close'
              }
               if(item[keyYesterDay] && CombOffsetFlag!== '3'){
                 if(item[keyYesterDay] >= Volume){
                   item[keyYesterDay] -= Volume
                   trader[close] = Volume
                 }else {
                    item[keyToady] -= Volume -  item[keyYesterDay];
                    trader[close] = item[keyYesterDay]
                    trader[closeToady] = Volume -  item[keyYesterDay]
                   item[keyYesterDay]  = 0;
                   
                 }
                 
               }else{
                 
                 item[keyToady] -= Volume;
                 trader[closeToady] = Volume
                  if(item[keyToady] < 0){
                    ipcRenderer.send('err-log', `持仓负数，${JSON.stringify(trader)}`);
                }
               }
            }
           }
      },
      updateTrader(trade){
        this.traderData.push(trade);
        this.traders.push(trade);
        this.setTradeItem(trade,  this.orderData.filter(e => {
          return  e.VolumeTraded
        }))
         ipcRenderer.send('update-instrumentsData', this.instrumentsData);
         
         this.$refs.round.update(trade);
          // if(!this.forceCloseCount){
          //   this.$forceUpdate()
          // }
      },
      upddateOrder(order, key, needUpdate){
        
          order.key = key;
          this.orders[key]= order;
        if(needUpdate){
          
          const index = this.orderData.findIndex(e =>e.key === key);
          console.log(index)
          this.orderData.splice(index, 1 , order);
        }else{
        
          this.orderData.unshift(order)
        }
        if( order.OrderStatus === '5'){
         
          const {InstrumentID} = order;
          const item = this.instrumentsData.find(e => e.instrumentID === InstrumentID);
          if(item)item.todayCancel += 1;
          if(item.big_todayCancel_limit_volume && order.VolumeTotal - order.VolumeTraded >= item.big_todayCancel_limit_volume) item.big_todayCancel += 1;
          ipcRenderer.send('update-instrumentsData', this.instrumentsData);
          // if(!this.forceCloseCount){
          //   this.$forceUpdate()
          // }
          
        }
      },
      exportExcel(title ,data, row){
        const excelData = data.map((item)=>{
          const result = {};
          row.forEach(({label,prop, render, component,componentRender, type}) => {
            if(render){
              result[label] = render(item)
            }else if(component){
              if(!componentRender) return;
              result[label] = componentRender(item)
            }else {
              result[label] = item[prop]
            }
            if(type === 'number'){
              result[label]  = parseFloat( result[label])
            }
          })
          return result
        })
        
        title = this.userData.userAccount + ' ' + title;
       ipcRenderer.send('export-excel', {title ,excelData});
      },
      async start({row},id) {
        
        if(this.locked){
          this.$alert('当前账号已锁定', '锁定' )
          return
        }
        
        const {instrumentID} = row;
        // if(this.userData.subscribeIndicator && this.userData.subscribeIndicator.includes(instrumentID)){
        //   this.ws.ws.send(`NotifyIndicatorInstrument@${instrumentID}`)
        // }
       
        const {width, height} = await getClientSize()
        const info = this.instrumentInfo.find(e => e.InstrumentID === instrumentID);
        if(!info){
           this.$alert('合约尚未订阅行情，请联系管理员订阅！');
           return;
        };

        const {PriceTick, ExchangeID, VolumeMultiple:volumeMultiple} = info;
        let checked  = true;
        
        const config = this.userData.instrumentConfigVOList.find(e => e.id === id );
        if(config.topQuot !== undefined){
          checked = config.topQuot
        }
       
        //给王曼妮做的hardcode 后期从配置里面取
       
        // if(this.userData.userAccount.includes('wmn')){
        //   checked = false;
        // }
        
        const accountIndex = this.currentAccount.futureUserName;
        const accountStatus = this.currentAccount.accountStatus
        ipcRenderer.send('open-window', {id:instrumentID, title: getWinName(instrumentID, accountIndex) + getHoldCondition(row), account: this.userData.id, width, height, tick: PriceTick, exchangeId: ExchangeID, checked,configId:id, accountIndex, accountStatus, volumeMultiple});
        this.$store.dispatch('updateIns', instrumentID);
      },
      stop(){
        ipcRenderer.send('close-all-sub');
         ipcRenderer.send('err-log', `触发锁定，盈亏金额${this.totalProfit}`)
        // this.$store.dispatch('updateIns', '')
      }, 
      updateConfig(){
        return this.$store.dispatch('get-config').then(()=> this.finishLoading('config'));
      },
      getCtpInfo(){
    
        //       this.instrumentInfo = [{"InstrumentID":"fu2412","ExchangeID":"SHFE","InstrumentName":"燃料油2412","ExchangeInstID":"fu2412","ProductID":"fu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20240429","StartDelivDate":"20241205","EndDelivDate":"20241209","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"fu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2403","ExchangeID":"CFFEX","InstrumentName":"中证500指数2412","ExchangeInstID":"IC2403","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"jd2412","ExchangeID":"DCE","InstrumentName":"鲜鸡蛋2412","ExchangeInstID":"jd2412","ProductID":"jd","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":300,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":300,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20241226","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SA205","ExchangeID":"CZCE","InstrumentName":"纯碱205","ExchangeInstID":"SA205","ProductID":"SA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"AP205","ExchangeID":"CZCE","InstrumentName":"鲜苹果205","ExchangeInstID":"AP205","ProductID":"AP","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
        //   ,{"InstrumentID":"ni2412","ExchangeID":"SHFE","InstrumentName":"镍2412","ExchangeInstID":"ni2412","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20241216","StartDelivDate":"20241217","EndDelivDate":"20241219","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"TA205","ExchangeID":"CZCE","InstrumentName":"精对苯二甲酸205","ExchangeInstID":"TA205","ProductID":"TA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"CJ205","ExchangeID":"CZCE","InstrumentName":"干制红枣205","ExchangeInstID":"CJ205","ProductID":"CJ","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":20,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":100,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.18,"ShortMarginRatio":0.18,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2412","ExchangeID":"DCE","InstrumentName":"生猪2412","ExchangeInstID":"lh2412","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20241228","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"CF205","ExchangeID":"CZCE","InstrumentName":"一号棉花205","ExchangeInstID":"CF205","ProductID":"CF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"pp2412","ExchangeID":"DCE","InstrumentName":"聚丙烯2412","ExchangeInstID":"pp2412","ProductID":"pp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"m2412","ExchangeID":"DCE","InstrumentName":"豆粕2412","ExchangeInstID":"m2412","ProductID":"m","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ag2202","ExchangeID":"SHFE","InstrumentName":"白银2202","ExchangeInstID":"ag2202","ProductID":"ag","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":15,"PriceTick":1,"CreateDate":"20210514","OpenDate":"20210616","ExpireDate":"20220215","StartDelivDate":"20220216","EndDelivDate":"20220220","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ag","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SM205","ExchangeID":"CZCE","InstrumentName":"锰硅205","ExchangeInstID":"SM205","ProductID":"SM","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"UR205","ExchangeID":"CZCE","InstrumentName":"尿素205","ExchangeInstID":"UR205","ProductID":"UR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"FG205","ExchangeID":"CZCE","InstrumentName":"玻璃205","ExchangeInstID":"FG205","ProductID":"FG","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"v2412","ExchangeID":"DCE","InstrumentName":"聚氯乙烯2412","ExchangeInstID":"v2412","ProductID":"v","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
        //   ,{"InstrumentID":"eb2404","ExchangeID":"DCE","InstrumentName":"苯乙烯2404","ExchangeInstID":"eb2404","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20240426","StartDelivDate":"","EndDelivDate":"20240429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eb2412","ExchangeID":"DCE","InstrumentName":"苯乙烯2412","ExchangeInstID":"eb2412","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20241228","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SF205","ExchangeID":"CZCE","InstrumentName":"硅铁205","ExchangeInstID":"SF205","ProductID":"SF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"eb2412","ExchangeID":"DCE","InstrumentName":"苯乙烯2412","ExchangeInstID":"eb2412","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20241226","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"l2412","ExchangeID":"DCE","InstrumentName":"线型低密度聚乙烯2412","ExchangeInstID":"l2412","ProductID":"l","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2402","ExchangeID":"CFFEX","InstrumentName":"中证500指数2202","ExchangeInstID":"IC2402","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220217","StartDelivDate":"20220217","EndDelivDate":"20220217","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cu2412","ExchangeID":"SHFE","InstrumentName":"铜2412","ExchangeInstID":"cu2412","ProductID":"cu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":5,"VolumeMultiple":5,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20241215","StartDelivDate":"20241216","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"cu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"hc2412","ExchangeID":"SHFE","InstrumentName":"热轧卷板2412","ExchangeInstID":"hc2412","ProductID":"hc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20241216","StartDelivDate":"20241217","EndDelivDate":"20241219","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"hc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2412","ExchangeID":"SHFE","InstrumentName":"锡2412","ExchangeInstID":"sn2412","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20241216","StartDelivDate":"20241217","EndDelivDate":"20241219","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2412","ExchangeID":"DCE","InstrumentName":"冶金焦炭2412","ExchangeInstID":"j2412","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SR205","ExchangeID":"CZCE","InstrumentName":"白砂糖205","ExchangeInstID":"SR205","ProductID":"SR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.07,"ShortMarginRatio":0.07,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"sc2202","ExchangeID":"INE","InstrumentName":"原油2202","ExchangeInstID":"sc2202","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":30,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20190516","OpenDate":"20190603","ExpireDate":"20241231","StartDelivDate":"20220201","EndDelivDate":"20220208","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"PK204","ExchangeID":"CZCE","InstrumentName":"花生仁204","ExchangeInstID":"PK204","ProductID":"PK","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210416","OpenDate":"20210416","ExpireDate":"20240418","StartDelivDate":"20240418","EndDelivDate":"20240418","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sc2412","ExchangeID":"INE","InstrumentName":"原油2412","ExchangeInstID":"sc2412","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20240429","StartDelivDate":"20241205","EndDelivDate":"20241211","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
        //   ,{"InstrumentID":"sc2404","ExchangeID":"INE","InstrumentName":"原油2404","ExchangeInstID":"sc2404","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210301","OpenDate":"20210401","ExpireDate":"20241231","StartDelivDate":"20240401","EndDelivDate":"20240411","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2209","ExchangeID":"DCE","InstrumentName":"生猪2209","ExchangeInstID":"lh2209","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210928","OpenDate":"20210928","ExpireDate":"20220927","StartDelivDate":"","EndDelivDate":"20220930","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2404","ExchangeID":"DCE","InstrumentName":"液化石油气2404","ExchangeInstID":"pg2404","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20240426","StartDelivDate":"","EndDelivDate":"20240429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI209","ExchangeID":"CZCE","InstrumentName":"菜籽油209","ExchangeInstID":"OI209","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"20220915","EndDelivDate":"20220915","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2412","ExchangeID":"DCE","InstrumentName":"液化石油气2412","ExchangeInstID":"pg2412","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20241228","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sp2412","ExchangeID":"SHFE","InstrumentName":"漂针桨2412","ExchangeInstID":"sp2412","ProductID":"sp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20241216","StartDelivDate":"20241217","EndDelivDate":"20241219","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sp","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2412","ExchangeID":"DCE","InstrumentName":"生猪2412","ExchangeInstID":"lh2412","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20241226","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2412","ExchangeID":"DCE","InstrumentName":"液化石油气2412","ExchangeInstID":"pg2412","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20241226","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2209","ExchangeID":"DCE","InstrumentName":"冶金焦炭2209","ExchangeInstID":"j2209","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2202","ExchangeID":"CFFEX","InstrumentName":"IF2202","ExchangeInstID":"IF2202","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220217","StartDelivDate":"20220217","EndDelivDate":"20220217","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC209","ExchangeID":"CZCE","InstrumentName":"动力煤209","ExchangeInstID":"ZC209","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210908","OpenDate":"20210908","ExpireDate":"20220907","StartDelivDate":"20220907","EndDelivDate":"20220907","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.3,"ShortMarginRatio":0.3,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2403","ExchangeID":"CFFEX","InstrumentName":"IF2403","ExchangeInstID":"IF2403","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI205","ExchangeID":"CZCE","InstrumentName":"菜籽油205","ExchangeInstID":"OI205","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cs2412","ExchangeID":"DCE","InstrumentName":"玉米淀粉2412","ExchangeInstID":"cs2412","ProductID":"cs","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210315","OpenDate":"20210315","ExpireDate":"20241214","StartDelivDate":"","EndDelivDate":"20241217","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}
        //   ,{"InstrumentID":"ni2412","ExchangeID":"SHFE","InstrumentName":"镍2403","ExchangeInstID":"ni2412","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":6,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20241215","StartDelivDate":"20241216","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ni2404","ExchangeID":"SHFE","InstrumentName":"镍2404","ExchangeInstID":"ni2404","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20240415","StartDelivDate":"20240418","EndDelivDate":"20240420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"rb2412","ExchangeID":"SHFE","InstrumentName":"螺纹钢2412","ExchangeInstID":"rb2412","ProductID":"rb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20241216","StartDelivDate":"20241217","EndDelivDate":"20241219","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"rb","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2412","ExchangeID":"DCE","InstrumentName":"焦煤2412","ExchangeInstID":"jm2412","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"i2412","ExchangeID":"DCE","InstrumentName":"铁矿石2412","ExchangeInstID":"i2412","ProductID":"i","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"p2412","ExchangeID":"DCE","InstrumentName":"棕榈油2412","ExchangeInstID":"p2412","ProductID":"p","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2209","ExchangeID":"DCE","InstrumentName":"焦煤2209","ExchangeInstID":"jm2209","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"MA205","ExchangeID":"CZCE","InstrumentName":"甲醇205","ExchangeInstID":"MA205","ProductID":"MA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"IH2202","ExchangeID":"CFFEX","InstrumentName":"上证50指数2202","ExchangeInstID":"IH2202","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220217","StartDelivDate":"20220217","EndDelivDate":"20220217","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eg2412","ExchangeID":"DCE","InstrumentName":"乙二醇2412","ExchangeInstID":"eg2412","ProductID":"eg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20241226","StartDelivDate":"","EndDelivDate":"20241231","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2412","ExchangeID":"SHFE","InstrumentName":"锡2412","ExchangeInstID":"sn2412","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":2,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20241215","StartDelivDate":"20241216","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IH2403","ExchangeID":"CFFEX","InstrumentName":"上证50指数2412","ExchangeInstID":"IH2403","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20241218","StartDelivDate":"20241218","EndDelivDate":"20241218","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2404","ExchangeID":"SHFE","InstrumentName":"锡2404","ExchangeInstID":"sn2404","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20240415","StartDelivDate":"20240418","EndDelivDate":"20240420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC205","ExchangeID":"CZCE","InstrumentName":"动力煤205","ExchangeInstID":"ZC205","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210513","OpenDate":"20210513","ExpireDate":"20241211","StartDelivDate":"20241211","EndDelivDate":"20241211","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.5,"ShortMarginRatio":0.5,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},
        //   {"InstrumentID":"y2412","ExchangeID":"DCE","InstrumentName":"豆油2412","ExchangeInstID":"y2412","ProductID":"y","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20241218","StartDelivDate":"","EndDelivDate":"20241223","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}]
        //  return;
           
           if(this.loading.indexOf('instrument') === -1)this.loading.push('instrument');
           
       
          request({
            url: '/ctp/instrument',
            
          }).then(({instrumentList}) => {
            this.instrumentInfo = instrumentList;
            this.finishLoading('instrument')
          })
       
          
      },
      // closeALL(){
        
      //    const arr = this.$refs.round.traderData.filter(({CloseVolume, Volume, InstrumentID}) => Volume> CloseVolume && this.instrumentsData.find(e => e.instrumentID ===InstrumentID));
      //     if(arr.length){
      //       ipcRenderer.invoke('async-cancel-order').then(()=>{    
      //           ipcRenderer.send('info-log', `${this.userData.userAccount}开始强平`)
      //           arr.forEach(({CloseVolume, Volume,  InstrumentID, OrderSysID, ExchangeID, Direction}) => {
      //             const order = this.orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
      //             const instrumentinfo = this.instrumentInfo.find(e => e.InstrumentID === InstrumentID)
      //             const { PriceTick } = instrumentinfo;
      //             let combOffsetFlag = specialExchangeId.includes(ExchangeID)? '3': '1';
                 
      //             if(order) {
      //               if(!this.forceCount || order.CombOffsetFlag !== '0'){
                     
      //                  combOffsetFlag = order.CombOffsetFlag
      //               }
                   
      //             }
      //             const direction= Direction==='0'? '1': '0'
      //             let over_price = this.$store.state.user.over_price * PriceTick;
      //             if(direction === '1'){
      //               over_price = -over_price;
      //             }
          
      //             let limitPrice = this.price[InstrumentID][direction] + over_price;
      //             if(limitPrice < this.price[InstrumentID][2]){
      //               limitPrice = this.price[InstrumentID][2]
      //             }
      //             if(limitPrice >this.price[InstrumentID][3]){
      //                limitPrice = this.price[InstrumentID][3]
      //             }
      //             if(this.forceCount) {
      //               setTimeout(()=> ipcRenderer.send('trade', {limitPrice, instrumentID: InstrumentID, combOffsetFlag, volumeTotalOriginal: Volume- CloseVolume, direction, ExchangeID}) ,100)
      //             }else{
      //                ipcRenderer.send('trade', {limitPrice, instrumentID: InstrumentID, combOffsetFlag, volumeTotalOriginal: Volume- CloseVolume, direction, ExchangeID}) 
      //             }
                 
              
      //         })
      //         this.forceCount++
      //     })
           
      //   }else{
      //      ipcRenderer.send('info-log', `${this.userData.userAccount}结束强平`)
      //     this.forcing = false;
      //     ipcRenderer.send('stop-subscrible')
      //     clearInterval(this.forcingCloseTime)
      //     this.forcingCloseTime = null;
      //     this.$forceUpdate()
      //   }
       
      // },
      async forceClose(){
        console.log('强平')
        this.forcing  = true;
       
     
        // this.stop();
     
        ipcRenderer.send('info-log', `${this.userData.userAccount}触发强平`)
        ipcRenderer.send('force-close', {over_price:  this.$store.state.user.over_price, instrumentInfo: this.instrumentInfo})
      
        // this.forcingCloseTime = setInterval(()=> {
         
        //   this.closeALL();
        // }, 1300)
        // if(force){
        //   this.forcingCloseTime = setInterval(()=> {
        //     this.closeALL();
        //   }, 1000)
        // }else{
        //    ipcRenderer.send('info-log', `非强平窗口`)
        //   setTimeout(()=> this.forcing = false, 500 )
        // }
        
      },
      finishLoading(tag){
        
        const index = this.loading.indexOf(tag);
        console.log(tag, this.loading)
        if(index > -1) {
          this.loading.splice(index, 1)
          
          if(!this.loading.length){
            
            this.startVolume();
            
            this.init()
          }
        }
       
      },
      startVolume(){
         if(this.started)return;
        this.started = true;
        const {quotVOList } = this.userData;
//         const quotAddr = '192.168.0.19:18899'.split(':');
//         ipcRenderer.send('start-receive', {host: quotAddr[0], port: quotAddr[1], instrumentIDs: ['ag2408','ag2412','au2408',

// 'au2410',
// 'OI409',
// 'ni2408',
// 'OI501',
// 'ni2409',
// 'sc2408',
// 'sc2409',
// 'zn2408',
// 'zn2409',],   iCmdID: 101});
        
        quotVOList.forEach((e) => {
          // if(this.userData.id === 18 ){
            
          //     e.quotAddr = '192.168.0.19:18899'
          // }
          // let  e= quotVOList [2]
           const _quotAddr = e.quotAddr.split(':');
            const instruments = e.subInstruments.split(',')
            ipcRenderer.send('start-receive', {host: _quotAddr[0], port: _quotAddr[1], instrumentIDs: instruments.filter(e => this.subscribelInstruments.some(a=> a.instruments.includes(e))),   iCmdID: 101, instruments});
        })
    },
      cancel(){
        this.dialogVisible = false;
        //不是主动查询的关闭要直接关闭窗口
        
        if(!this.confirmquerycount){
          ipcRenderer.send('close-main');
        }
       
      },
      confirm(){
        this.dialogVisible = false;
         ipcRenderer.send('confirm-settlement');
        
      },
      confirmTimeqry(){
        if(!this.confimInfoDate)return
        this.confirmLoading = true;
        this.confirmquerycount ++
        ipcRenderer.send('query-SettlementInfo', this.confimInfoDate)
      },
      historyQry(){
       
        if(!this.historyDate || this.historyDate.length === 0){
          this.$message('请选择查询时间');
          return
        }
        clearTimeout(this.historyLoadingTimeout)
        this.historyData = []
        this.historyLoading = true;
        let startDate = this.historyDate[0].getTime();
        const endDate= this.historyDate[1].getTime();
        while(startDate <= endDate){
          let date = new Date(startDate);
          let weekday = date.getDay();
          //周六周日不开盘不用查询
          if(weekday === 0 || weekday === 6){
            startDate = startDate + 24*60*60*1000;
            continue
          }
          ipcRenderer.send('query-SettlementInfo', getyyyyMMdd(date))
          startDate = startDate + 24*60*60*1000;
        }
      
      
        
      
      },
      findLimit(limits, instrumentId){
        let limit = limits.find(({instrumentID}) => instrumentID === instrumentId)
        if(limit){
          return limit.limit
        }
        instrumentId = instrumentId.replace(/\d+/, '')
        limit = limits.find(({instrumentID}) => instrumentID === instrumentId)
        if(limit){
          return limit.limit
        }
        return '无'
      },
      getBigCancelLimit(instrumentId){
        const info = this.instrumentInfo.find(({InstrumentID}) => InstrumentID === instrumentId);
        if(!info){
          return{
            limit: 0,
            volume:0
          }
        }
        
        const {ExchangeID, MaxLimitOrderVolume} = info
        const limit_info = this.vtp_client_big_cancelvolume_limit.find(({exchangeId}) => exchangeId === ExchangeID);
        let {limit, volume} = limit_info;
        if(volume < 1){
          volume = MaxLimitOrderVolume * volume
        }
        return {
          limit,
          volume
        }
      },
      reset(){
      
        this.loading.push('order');
        this.loading.push('trade');
        this.login();
      },
      reconnect(){
        
        ipcRenderer.send('tcp-reconnect')
      },
      puppetReconnect(){
        console.log('puppet reconnect')
        if(this.currentAccount.puppet){
          ipcRenderer.send('puppet-reconnect')
        }
       
      },
      login(){
        
        const userData = this.userData;
        const active = this.$store.state.user.activeCtpaccount;
        const account = userData.futureAccountVOList.find(e => e.id === active);
        
        this.currentAccount = account;
      
        const {
          tradeAddr:ctp1_TradeAddress,
          id,
          brokerId: m_BrokerId,
          authCode: m_AuthCode,
          futureUserId: m_InvestorId,
          futureUserId: m_UserId,
          futureUserPwd:m_PassWord,
          appId: m_AppId,
          futureUserId:m_AccountId,
          puppet
        } = account;;
        
         ipcRenderer.send('trade-login', {
           ctp1_TradeAddress,
            m_BrokerId,
            m_AuthCode,
            m_InvestorId,
            m_UserId,
            m_PassWord,
            m_AppId,
            m_AccountId,
            userId: userData.id,
            idol: userData.idol,
            puppet,
            id
          //  instruments: this.subscribelInstruments
          });
      },
      changeAccount(){
         const arr = this.$refs.round.traderData.filter(({CloseVolume, Volume, InstrumentID}) => Volume> CloseVolume && this.instrumentsData.find(e => e.instrumentID ===InstrumentID));
         let pro = Promise.resolve()
         if(arr.length){
           pro = this.$confirm('当年账户有未完成交易单确认切换账户？', '提示', {
            confirmButtonText: '确定',
            cancelButtonText: '取消',
            type: 'warning'
          })
         }
         pro.then(()=>{
          //  ipcRenderer.send('close-all-sub');
            ipcRenderer.send('resize-main',  {width: 500, height: 383});
            this.$router.push('/');
         })
        
      },
      relogin(data){
    
        this.$store.commit('setstate', {
              key: 'userData',
              data
          })
        this.loginVisible = false;
      }
    },
    beforeDestroy(){
      clearTimeout(this.timoutquery);
      ['receive-order', 'receive-position','force-close-finish','receive-trade','receive-rate','finish-loading','add-loading','receive-info','receive-price','receive-account'].forEach(e=> {
        ipcRenderer.removeAllListeners(e)
      })
    }

  }
</script>

<style >


  @import url('https://fonts.googleapis.com/css?family=Source+Sans+Pro');

  * {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
  }

  body { font-family: 'Source Sans Pro', sans-serif; }

  #wrapper {
    height: 100vh;
    padding: 0 10px 10px 10px;
    width: 100vw;
    overflow-x: hidden;
   
  }
  #wrapper .vxe-cell {
      padding: 0 4px !important;
    }
  #wrapper  .vxe-body--column {
      padding: 2px 0 !important;;
    }
  #logo {
    height: auto;
    margin-bottom: 20px;
    width: 420px;
  }

  main {
    display: flex;
    justify-content: space-between;
    margin-top: 110px;
    top: 0;
    overflow-x: scroll;
  }
  .label {
    margin: 10px 0;
    display: flex;
    align-items: center;
  }
  .confirm-info{
    height: 400px;
    overflow-x: hidden;
    overflow-y: scroll;
    white-space: pre-line;
  }
  .left-side {
    display: flex;
    flex-direction: column;
    flex-basis: 48%; 
  }
  .right-side {
    display: flex;
    flex-direction: column;
    flex-basis: 48%; 
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

  .sell-direction {
    text-align: right;
  }
  .account {
    position: fixed;
    width: 98%;
    z-index: 10;
  }
</style>
