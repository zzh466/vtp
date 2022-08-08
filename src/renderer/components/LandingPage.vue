<template>
  <div id="wrapper" v-loading='loading.length || forcing'  :element-loading-text="forcing?'触发强平操作，正在强平中':'正在获取账号信息'" >
    <el-descriptions size= 'samll' direction="vertical" :column="10" border class="account">
      <el-descriptions-item label="账号">{{userData.userAccount}}</el-descriptions-item>
         <el-descriptions-item label="交易日">{{account.TradingDay}}</el-descriptions-item>
      <el-descriptions-item label="当前账户">{{currentAccount.futureUserName}}</el-descriptions-item>
   
    
      <el-descriptions-item label="手续费">{{account.Commission.toFixed(2)}}</el-descriptions-item>
      
      <el-descriptions-item label="当前账户盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission).toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="隔节误差">{{deviation.toFixed(2)}}</el-descriptions-item>
       <el-descriptions-item label="实际盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission + deviation).toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="强平线">{{userData.thrRealProfit}}</el-descriptions-item>
      <el-descriptions-item label="可用资金">{{Math.floor(account.Available/ 1000)*1000 }}</el-descriptions-item>
       <el-descriptions-item label="总实际盈亏">{{totalProfit}}</el-descriptions-item>
        <!-- <el-descriptions-item v-if="userData.futureAccountVOList.length > 1"><el-button type="primary" size="small" @click="changeAccount">切换账号</el-button></el-descriptions-item> -->
    </el-descriptions>
      
    <main >
      
      <div class="left-side">
        
         <div>
           <div class="label">回合信息： <el-button type="primary" size="small" @click="exportroud">导出</el-button></div>
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
    <!-- <el-button type="primary" @click="updateConfig">更新配置</el-button> -->
       <div class="label">订阅合约： <el-button type="primary" style="margin-left: 20px" size="small" @click="reconnect">强制重连</el-button></div>
      <div style="display: flex;">
         
      
          <Table v-for='instrument in subscribelInstruments' :key ='instrument.id'  height='300' @row-dblclick='start' :tableData='instrumentsData | changeNo(instrument.instruments)' :columns= 'instrumentsColumns'/>
          <!-- <el-button @click="open">商品</el-button>
              <el-button @click="open1">郑商所</el-button>
              <el-button @click="open2">股指</el-button>  -->
              <!-- <el-button @click="forceClose">强平</el-button> -->
      </div>
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
    <el-dialog :close-on-press-escape='false' :show-close='false' :visible="loginVisible" :close-on-click-modal='false' title="重新登陆"> 
      <loginform @login='relogin' :userAccount ='userData.userAccount'/>
    </el-dialog>
  </div>
</template>

<script>
  import { ipcRenderer } from 'electron';

  import {getWinName, getyyyyMMdd, getHoldCondition, getClientSize, specialExchangeId} from '../utils/utils';
  import request, {TraderSocket}  from '../utils/request';
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
        }
        if(!val){
          this.forceCloseCount = 0
        }
      }
    },
      filters: {
      changeNo(instruments, _instruments){
        
        return instruments.filter(e=> _instruments.includes(e.instrumentID));
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
          configId: e.id
        }))
      },
      openvolume_limit(){
          return this.$store.state.user.openvolume_limit
      },
      vtp_client_cancelvolume_limit(){
          return this.$store.state.user.vtp_client_cancelvolume_limit
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
      }
   
    },
    data(){
      const _this = this;
      return {
        dialogVisible: false,
        orders: {},
        loading: ['order', 'trade', 'config'],
        orderData: [],
        instrumentsData: [],
        traderData: [],
        // loading: [],
        traders: [],
        positions:[],
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
        },
        {
          label: '开仓限制',
          prop: 'openvolume_limit',
          width: 60
        },
        {
          label: '撤单限制',
          prop: 'vtp_client_cancelvolume_limit',
          width: 60
        }
        ],
        forcing: false,
        currentAccount: {},
        totalProfit: 0,
        deviation: 0,
         loginVisible: false
      }
    },
    created(){
      this.forceCloseCount = 0;
      this.historyTraders = [];
      this.updateConfig().then(()=>{
        
        const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`));
        ipcRenderer.send('set-account', this.userData.id);
        
        const broadcast = config.some(e => e.broadcastOpenInterest);
        this.ws = new TraderSocket(this.userData.id);
        if(broadcast ){
          this.ws.onmessage((e)=>{
            console.log(e)
            ipcRenderer.send('broadcast-openinterest', e);
          })
        }
        this.login()
         
      })
      this.getCtpInfo();
      ipcRenderer.on('receive-order', (event, orders, key, needUpdate) =>{
        
        console.log(orders, key, needUpdate)
       if(key){
         
         this.upddateOrder(orders, key, needUpdate)
       }else{
         this.orders = orders;
         this.finishLoading('order');
       }
        
      });
  
       ipcRenderer.on('receive-position', (event, position) =>{
        // console.log(position.filter(a => a.InstrumentID==='IC2201'))
        
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
         
          this.updateTrader(trader);
          const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`));
       
          if(config.find(e=> e.instruments.includes(trader.InstrumentID)).broadcastOpenInterest){
            const futureUserId = this.$store.state.user.activeCtpaccount
          
            let { ExchangeID, OrderSysID, TradeID, InstrumentID,Volume ,Direction, TradeTime, TradingDay} = trader;
            if(!TradeTime ){
              return
            }else {
              
              const time = +new Date(`${(new Date()).toDateString()} ${TradeTime}`);
              if(Math.abs(time - new Date()) > 1000 * 30 *60 )return;
            }
          
            if(Direction === '1'){
              Volume = -Volume;
            }
            this.ws.send(`${futureUserId}-${ExchangeID}-${OrderSysID}-${TradeID}:${InstrumentID}:${Volume}`)
          }
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
        
        this.confirmInfo =  arg.map(({Content}) => Content)
        this.dialogVisible = true;
      })
       ipcRenderer.on('receive-price', (event, arg)=>{    
        //计算隔节误差;
        console.log(this.historyTraders)
        if(this.historyTraders.length){
          
          for(let i = this.historyTraders.length - 1 ; i >= 0; i--){
           
            const {InstrumentID, Volume, Direction, TradeDate  } = this.historyTraders[i];
            const priceData = arg[InstrumentID];
            const info = this.instrumentInfo.find(e => InstrumentID === e.InstrumentID);
             console.log(priceData, info)
            if(priceData && info){
              let price
              
              const { SettlementPrice, ClosePrice, PreClosePrice, PreSettlementPrice , TradingDay} = priceData[4];
              
              if(TradeDate  < TradingDay ){
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
         if(!arg || this.loginVisible)return;
         console.log(arg)
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
          this.totalProfit =  res.futureAccountVOList.reduce((a,b) => a + b.realProfit, 0).toFixed(2) 
           if(!this.locked){
            if( this.userData.thrRealProfit && this.totalProfit < -this.userData.thrRealProfit){
              
              if(this.forceCloseCount > 2){
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
      init(){
        
        console.log(1111111111111111111111111111111)
        
        const arr = []
        for(let key in this.orders){
          this.orders[key].key =key;
          arr.unshift(this.orders[key])
        }
        this.orderData = arr;
        
        this.traderData = this.positions.concat(this.traders);

    
        const openvolume_limit = this.openvolume_limit.split(';').filter(e=>e).map(e => {
          const msg= e.split(':')
          return {
            instrumentID: msg[0],
            limit : msg[1]
          }
        })
        const vtp_client_cancelvolume_limit = this.vtp_client_cancelvolume_limit.split(';').filter(e=>e).map(e => {
          const msg= e.split(':')
          return {
            instrumentID: msg[0],
            limit : msg[1]
          }
        })
        const data = this.subscribelInstruments.reduce((a,b)=> a.concat(b.instruments), []).map(e=>({
          instrumentID: e,
          configId: e.configId,
          yesterdayBuy: 0,
          yesterdayAsk: 0,
          todayBuy: 0,
          todayAsk:0,
          todayVolume: 0,
          'todayCancel': 0,
          openvolume_limit: (openvolume_limit.find(({instrumentID})=> instrumentID ===e) || {limit: "无"}).limit,
          vtp_client_cancelvolume_limit:  (vtp_client_cancelvolume_limit.find(({instrumentID})=> e.includes(instrumentID)) || {limit: "无"}).limit
        }))
        
        this.orderData.filter(e => e.OrderStatus === '5').forEach(order => {
          const {InstrumentID} = order;
          const item = data.find(e => e.instrumentID === InstrumentID);
          if(item)item.todayCancel += 1;
        })
        //过滤
        const orderData = this.orderData;
        // console.log(JSON.parse(JSON.stringify(this.traderData.filter(e => e.InstrumentID === 'v2205'))))
        this.traderData.forEach(trader => {
           this.setTradeItem(trader, orderData, data);
        })
        ipcRenderer.send('update-instrumentsData', data, true);
        this.instrumentsData = data;
        this.deviation = 0;
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
             }
            if(CombOffsetFlag === '0' ){
                const key = Direction === '0'? 'todayBuy': 'todayAsk';
                item[key] += Volume;
                item.todayVolume += Volume;
            }else {
               const keyYesterDay = Direction === '0'? 'yesterdayAsk': 'yesterdayBuy';
               const keyToady = Direction === '0'? 'todayAsk': 'todayBuy';
              
               if(item[keyYesterDay] && CombOffsetFlag!== '3'){
                 if(item[keyYesterDay] >= Volume){
                   item[keyYesterDay] -= Volume
                 }else {
                    item[keyToady] -= Volume -  item[keyYesterDay];
                   item[keyYesterDay]  = 0;
                   
                 }
                 
               }else{
                 
                 item[keyToady] -= Volume;
                  if(item[keyToady] < 0){
                    ipcRenderer.send('err-log', `持仓负数，${JSON.stringify(trader)}`);
                }
               }
            }
           }
      },
      updateTrader(trade){
        this.traderData.push(trade);
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
        title = this.userData.userAccount + title;
       ipcRenderer.send('export-excel', {title ,excelData});
      },
      start({row}) {
        if(this.locked){
          this.$alert('当前账号已锁定', '锁定' )
          return
        }
        const {instrumentID} = row;
        const {width, height} = getClientSize()
        const info = this.instrumentInfo.find(e => e.InstrumentID === instrumentID);
        if(!info){
           this.$alert('改合约尚未订阅行情，请联系管理员订阅！');
           return;
        };
        const {PriceTick, ExchangeID} = info;
        //给王曼妮做的hardcode 后期从配置里面取
        let checked  = true;
        if(this.userData.userAccount.includes('wmn')){
          checked = false;
        }
        const configId = this.subscribelInstruments.find(e => e.instruments.includes(instrumentID)).configId
        const accountIndex = this.currentAccount.futureUserName;
        ipcRenderer.send('open-window', {id:instrumentID, title: getWinName(instrumentID, accountIndex) + getHoldCondition(row), account: this.userData.id, width, height, tick: PriceTick, exchangeId: ExchangeID, checked,configId, accountIndex});
        this.$store.dispatch('updateIns', instrumentID);
      },
      stop(){
        const account = this.account;
        ipcRenderer.send('close-all-sub');
         ipcRenderer.send('err-log', `触发锁定，盈亏金额${account.CloseProfit + account.PositionProfit - account.Commission}`)
        // this.$store.dispatch('updateIns', '')
      }, 
      updateConfig(){
        return this.$store.dispatch('get-config').then(()=> this.finishLoading('config'));
      },
      getCtpInfo(){
          // this.instrumentInfo = [{"InstrumentID":"fu2205","ExchangeID":"SHFE","InstrumentName":"燃料油2205","ExchangeInstID":"fu2205","ProductID":"fu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20220429","StartDelivDate":"20220505","EndDelivDate":"20220509","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"fu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2203","ExchangeID":"CFFEX","InstrumentName":"中证500指数2203","ExchangeInstID":"IC2203","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"jd2205","ExchangeID":"DCE","InstrumentName":"鲜鸡蛋2205","ExchangeInstID":"jd2205","ProductID":"jd","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":300,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":300,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SA205","ExchangeID":"CZCE","InstrumentName":"纯碱205","ExchangeInstID":"SA205","ProductID":"SA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"AP205","ExchangeID":"CZCE","InstrumentName":"鲜苹果205","ExchangeInstID":"AP205","ProductID":"AP","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          // ,{"InstrumentID":"ni2205","ExchangeID":"SHFE","InstrumentName":"镍2205","ExchangeInstID":"ni2205","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"TA205","ExchangeID":"CZCE","InstrumentName":"精对苯二甲酸205","ExchangeInstID":"TA205","ProductID":"TA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"CJ205","ExchangeID":"CZCE","InstrumentName":"干制红枣205","ExchangeInstID":"CJ205","ProductID":"CJ","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":20,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":100,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.18,"ShortMarginRatio":0.18,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2203","ExchangeID":"DCE","InstrumentName":"生猪2203","ExchangeInstID":"lh2203","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"CF205","ExchangeID":"CZCE","InstrumentName":"一号棉花205","ExchangeInstID":"CF205","ProductID":"CF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"pp2205","ExchangeID":"DCE","InstrumentName":"聚丙烯2205","ExchangeInstID":"pp2205","ProductID":"pp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"m2205","ExchangeID":"DCE","InstrumentName":"豆粕2205","ExchangeInstID":"m2205","ProductID":"m","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ag2206","ExchangeID":"SHFE","InstrumentName":"白银2206","ExchangeInstID":"ag2206","ProductID":"ag","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":15,"PriceTick":1,"CreateDate":"20210514","OpenDate":"20210616","ExpireDate":"20220615","StartDelivDate":"20220616","EndDelivDate":"20220620","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ag","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SM205","ExchangeID":"CZCE","InstrumentName":"锰硅205","ExchangeInstID":"SM205","ProductID":"SM","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"UR205","ExchangeID":"CZCE","InstrumentName":"尿素205","ExchangeInstID":"UR205","ProductID":"UR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"FG205","ExchangeID":"CZCE","InstrumentName":"玻璃205","ExchangeInstID":"FG205","ProductID":"FG","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"v2205","ExchangeID":"DCE","InstrumentName":"聚氯乙烯2205","ExchangeInstID":"v2205","ProductID":"v","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          // ,{"InstrumentID":"eb2204","ExchangeID":"DCE","InstrumentName":"苯乙烯2204","ExchangeInstID":"eb2204","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20220426","StartDelivDate":"","EndDelivDate":"20220429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eb2203","ExchangeID":"DCE","InstrumentName":"苯乙烯2203","ExchangeInstID":"eb2203","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SF205","ExchangeID":"CZCE","InstrumentName":"硅铁205","ExchangeInstID":"SF205","ProductID":"SF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"eb2205","ExchangeID":"DCE","InstrumentName":"苯乙烯2205","ExchangeInstID":"eb2205","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"l2205","ExchangeID":"DCE","InstrumentName":"线型低密度聚乙烯2205","ExchangeInstID":"l2205","ProductID":"l","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2206","ExchangeID":"CFFEX","InstrumentName":"中证500指数2206","ExchangeInstID":"IC2206","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cu2203","ExchangeID":"SHFE","InstrumentName":"铜2203","ExchangeInstID":"cu2203","ProductID":"cu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":5,"VolumeMultiple":5,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"cu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"hc2205","ExchangeID":"SHFE","InstrumentName":"热轧卷板2205","ExchangeInstID":"hc2205","ProductID":"hc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"hc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2205","ExchangeID":"SHFE","InstrumentName":"锡2205","ExchangeInstID":"sn2205","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2205","ExchangeID":"DCE","InstrumentName":"冶金焦炭2205","ExchangeInstID":"j2205","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SR205","ExchangeID":"CZCE","InstrumentName":"白砂糖205","ExchangeInstID":"SR205","ProductID":"SR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.07,"ShortMarginRatio":0.07,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"sc2206","ExchangeID":"INE","InstrumentName":"原油2206","ExchangeInstID":"sc2206","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":30,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20190516","OpenDate":"20190603","ExpireDate":"20220531","StartDelivDate":"20220601","EndDelivDate":"20220608","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"PK204","ExchangeID":"CZCE","InstrumentName":"花生仁204","ExchangeInstID":"PK204","ProductID":"PK","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210416","OpenDate":"20210416","ExpireDate":"20220418","StartDelivDate":"20220418","EndDelivDate":"20220418","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sc2205","ExchangeID":"INE","InstrumentName":"原油2205","ExchangeInstID":"sc2205","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20220429","StartDelivDate":"20220505","EndDelivDate":"20220511","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          // ,{"InstrumentID":"sc2204","ExchangeID":"INE","InstrumentName":"原油2204","ExchangeInstID":"sc2204","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210301","OpenDate":"20210401","ExpireDate":"20220331","StartDelivDate":"20220401","EndDelivDate":"20220411","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2209","ExchangeID":"DCE","InstrumentName":"生猪2209","ExchangeInstID":"lh2209","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210928","OpenDate":"20210928","ExpireDate":"20220927","StartDelivDate":"","EndDelivDate":"20220930","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2204","ExchangeID":"DCE","InstrumentName":"液化石油气2204","ExchangeInstID":"pg2204","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20220426","StartDelivDate":"","EndDelivDate":"20220429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI209","ExchangeID":"CZCE","InstrumentName":"菜籽油209","ExchangeInstID":"OI209","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"20220915","EndDelivDate":"20220915","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2203","ExchangeID":"DCE","InstrumentName":"液化石油气2203","ExchangeInstID":"pg2203","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sp2205","ExchangeID":"SHFE","InstrumentName":"漂针桨2205","ExchangeInstID":"sp2205","ProductID":"sp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sp","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2205","ExchangeID":"DCE","InstrumentName":"生猪2205","ExchangeInstID":"lh2205","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2205","ExchangeID":"DCE","InstrumentName":"液化石油气2205","ExchangeInstID":"pg2205","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2209","ExchangeID":"DCE","InstrumentName":"冶金焦炭2209","ExchangeInstID":"j2209","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2206","ExchangeID":"CFFEX","InstrumentName":"IF2206","ExchangeInstID":"IF2206","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC209","ExchangeID":"CZCE","InstrumentName":"动力煤209","ExchangeInstID":"ZC209","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210908","OpenDate":"20210908","ExpireDate":"20220907","StartDelivDate":"20220907","EndDelivDate":"20220907","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.3,"ShortMarginRatio":0.3,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2203","ExchangeID":"CFFEX","InstrumentName":"IF2203","ExchangeInstID":"IF2203","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI205","ExchangeID":"CZCE","InstrumentName":"菜籽油205","ExchangeInstID":"OI205","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cs2203","ExchangeID":"DCE","InstrumentName":"玉米淀粉2203","ExchangeInstID":"cs2203","ProductID":"cs","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210315","OpenDate":"20210315","ExpireDate":"20220314","StartDelivDate":"","EndDelivDate":"20220317","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}
          // ,{"InstrumentID":"ni2203","ExchangeID":"SHFE","InstrumentName":"镍2203","ExchangeInstID":"ni2203","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":6,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ni2204","ExchangeID":"SHFE","InstrumentName":"镍2204","ExchangeInstID":"ni2204","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20220415","StartDelivDate":"20220418","EndDelivDate":"20220420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"rb2205","ExchangeID":"SHFE","InstrumentName":"螺纹钢2205","ExchangeInstID":"rb2205","ProductID":"rb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"rb","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2205","ExchangeID":"DCE","InstrumentName":"焦煤2205","ExchangeInstID":"jm2205","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"i2205","ExchangeID":"DCE","InstrumentName":"铁矿石2205","ExchangeInstID":"i2205","ProductID":"i","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"p2205","ExchangeID":"DCE","InstrumentName":"棕榈油2205","ExchangeInstID":"p2205","ProductID":"p","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2209","ExchangeID":"DCE","InstrumentName":"焦煤2209","ExchangeInstID":"jm2209","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"MA205","ExchangeID":"CZCE","InstrumentName":"甲醇205","ExchangeInstID":"MA205","ProductID":"MA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"IH2206","ExchangeID":"CFFEX","InstrumentName":"上证50指数2206","ExchangeInstID":"IH2206","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eg2205","ExchangeID":"DCE","InstrumentName":"乙二醇2205","ExchangeInstID":"eg2205","ProductID":"eg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2203","ExchangeID":"SHFE","InstrumentName":"锡2203","ExchangeInstID":"sn2203","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":2,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IH2203","ExchangeID":"CFFEX","InstrumentName":"上证50指数2203","ExchangeInstID":"IH2203","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2204","ExchangeID":"SHFE","InstrumentName":"锡2204","ExchangeInstID":"sn2204","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20220415","StartDelivDate":"20220418","EndDelivDate":"20220420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC205","ExchangeID":"CZCE","InstrumentName":"动力煤205","ExchangeInstID":"ZC205","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210513","OpenDate":"20210513","ExpireDate":"20220511","StartDelivDate":"20220511","EndDelivDate":"20220511","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.5,"ShortMarginRatio":0.5,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},
          // {"InstrumentID":"y2205","ExchangeID":"DCE","InstrumentName":"豆油2205","ExchangeInstID":"y2205","ProductID":"y","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}]
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
       
     
        this.stop();
     
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
        console.log(tag)
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
        // const quotAddr = '127.0.0.1:18199'.split(':');
        // ipcRenderer.send('start-receive', {host: quotAddr[0], port: quotAddr[1], instrumentIDs: ['jm2209', 'j2209'],   iCmdID: 101});
        quotVOList.forEach((e) => {
           const _quotAddr = e.quotAddr.split(':');
            const instruments = e.subInstruments.split(',')
             ipcRenderer.send('start-receive', {host: _quotAddr[0], port: _quotAddr[1], instrumentIDs: instruments.filter(e => this.subscribelInstruments.some(a=> a.instruments.includes(e))),   iCmdID: 101});
        })
    },
      cancel(){
        this.dialogVisible = false;
        ipcRenderer.send('close-main');
      },
      confirm(){
        this.dialogVisible = false;
         ipcRenderer.send('confirm-settlement');
        
      },
      reset(){
      
        this.loading.push('order');
        this.loading.push('trade');
        this.login();
      },
      reconnect(){
        ipcRenderer.send('tcp-reconnect')
      },
      login(){
        
        const userData = this.userData;
        const active = this.$store.state.user.activeCtpaccount;
        const account = userData.futureAccountVOList.find(e => e.id === active);
       this.currentAccount = account;
      
        const {
          tradeAddr:ctp1_TradeAddress,
        
          brokerId: m_BrokerId,
          authCode: m_AuthCode,
          futureUserId: m_InvestorId,
          futureUserId: m_UserId,
          futureUserPwd:m_PassWord,
          appId: m_AppId,
          futureUserId:m_AccountId
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
