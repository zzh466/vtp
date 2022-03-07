<template>
  <div id="wrapper" v-loading='loading.length || forcing'  :element-loading-text="forcing?'触发强平操作，正在强平中':'正在获取账号信息'" >
    <el-descriptions size= 'samll' direction="vertical" :column="10" border class="account">
      <el-descriptions-item label="账号">{{userData.account}}</el-descriptions-item>
      <el-descriptions-item label="交易日">{{account.TradingDay}}</el-descriptions-item>
    
      <el-descriptions-item label="总手续费">{{account.Commission.toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="总实际盈亏">{{(account.CloseProfit + account.PositionProfit - account.Commission).toFixed(2)}}</el-descriptions-item>
      <el-descriptions-item label="强平线">{{userData.thrRealProfit}}</el-descriptions-item>
      <el-descriptions-item label="可用资金">{{Math.floor(account.Available/ 1000)*1000 }}</el-descriptions-item>
    
    </el-descriptions>
      
    <main  v-if='!loading.length'>
      
      <div class="left-side">
        
         <div>
           <div class="label">回合信息： <el-button type="primary" size="small" @click="exportroud">导出</el-button></div>
          <Round ref="round" :data='traderData' :rates='rates' :price='price' :instrumentInfo='instrumentInfo' :positions="positions"></Round>
        </div>
         
      </div>

      <div class="right-side">
        
       
        <div>
          <div class="label">下单信息：<el-button type="primary" size="small" @click="exportExcel('下单信息', orderData, orderColumns)">导出</el-button></div>
            <Table  height='280' :columns='orderColumns' :tableData='orderData'/>
        </div>
      </div>
     
    </main>
    <!-- <el-button type="primary" @click="updateConfig">更新配置</el-button> -->
      <div style="display: flex; flex">
          <Table v-for='instrument in subscribelInstruments' :key ='instrument.exchangeNo'  height='300' @row-dblclick='start' :tableData='instrumentsData | changeNo(instrument.instruments)' :columns= 'instrumentsColumns'/>
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
  </div>
</template>

<script>
  import { ipcRenderer, dialog } from 'electron';

  import {getWinName, Direction, CombOffsetFlag, getyyyyMMdd, getHoldCondition, getClientSize} from '../utils/utils';
  import request, {TraderSocket}  from '../utils/request';
  import Round from './LandingPage/round.vue';
  import Status from './LandingPage/Status.vue';
    import {Status as parseStatus, specialExchangeId} from '../utils/utils'
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
  function coverZero(num){
    if(num < 0) return 0;
    return num
  }
  Vue.component('Status', Status);
  export default {
    name: 'landing-page',
    components: {  Round },
    // watch:{
    //   'userData.locked'(val, old){
    //     if(val){
    //       this.stop()
    //     }
    //   }
    // },
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
         return this.userData.quotVOList.map(e=> ({
          instruments: e.subInstruments.split(',').map(e => e.replace(/[\n\r]/g, '')),
          exchangeNo: e.exchangeNo
        }))
      },
      openvolume_limit(){
          return this.$store.state.user.openvolume_limit
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
        const openvolume_limit = this.openvolume_limit.split(';').map(e => {
          const msg= e.split(':')
          return {
            instrumentID: msg[0],
            limit : msg[1]
          }
        })
        const data = this.subscribelInstruments.reduce((a,b)=> a.concat(b.instruments), []).map(e=>({
          instrumentID: e,
          yesterdayBuy: 0,
          yesterdayAsk: 0,
          todayBuy: 0,
          todayAsk:0,
          todayVolume: 0,
          'todayCancel': 0,
          openvolume_limit: (openvolume_limit.find(({instrumentID})=> instrumentID ===e) || {limit: "无"}).limit 
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
        // console.log(JSON.parse(JSON.stringify(this.traderData.filter(e => e.InstrumentID === 'v2205'))))
        this.traderData.forEach(trader => {
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
            
           
            if(CombOffsetFlag === '0'){
                const key = Direction === '0'? 'todayBuy': 'todayAsk';
                item[key] += Volume;
                item.todayVolume += Volume;
            }else {
               const keyYesterDay = Direction === '0'? 'yesterdayAsk': 'yesterdayBuy';
               const keyToady = Direction === '0'? 'todayAsk': 'todayBuy';
               if(item[keyYesterDay] ){
                 if(item[keyYesterDay] >= Volume){
                   item[keyYesterDay] -= Volume
                 }else {
                    item[keyToady] -= Volume -  item[keyYesterDay];
                   item[keyYesterDay]  = 0;
                   
                 }
                 
               }else{
                
                 item[keyToady] -= Volume;
               }
            }
           }
        })
        ipcRenderer.send('update-instrumentsData', data);
        return data
      },
      traderData(){
        
        return this.positions.concat(this.traders);
      }
    },
    data(){
      const _this = this;
      return {
        dialogVisible: false,
        orders: {},
        loading: ['order', 'trade', 'config'],
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
          width: 80
        }
        ],
        forcing: false
      }
    },
    created(){
      
      this.updateConfig().then(()=>{
        
        const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`));

        
        const broadcast = config.some(e => e.broadcastOpenInterest);
        if(broadcast && !this.ws){
          this.ws = new TraderSocket();
          this.ws.onmessage((e)=>{
            console.log(e.data)
            ipcRenderer.send('broadcast-openinterest', e.data);
          })
        }
        this.login()
         
      })
      this.getCtpInfo();
      ipcRenderer.on('receive-order', (event, orders) =>{
        
        this.orders = orders;
      });
  
       ipcRenderer.on('receive-position', (event, position) =>{
        // console.log(position.filter(a => a.InstrumentID==='IC2201'))
        
        this.positions = position;
        this.finishLoading('position')
      });
       ipcRenderer.on('receive-trade', (event, trader) =>{
        if(Array.isArray(trader)){
          
          this.traders = trader 
        }
        
        if(this.ws && this.traders.length){
          const {futureUserId} = this.userData
        
          let { ExchangeID, OrderSysID, TradeID, InstrumentID,Volume ,Direction, TradeTime, TradingDay} = trader[trader.length -1];
          const item = this.subscribelInstruments.find(e=>e.instruments.includes(InstrumentID));
          if(!item) return;
          const {exchangeNo} = item;
          const config =JSON.parse(localStorage.getItem(`config-${this.userData.id}`)).find(e => e.exchangeNo === exchangeNo);
           
          if(!TradeTime || !config.broadcastOpenInterest){
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
         
        console.log(trader, '2336456')
        
      });
       ipcRenderer.on('receive-rate', (event, rates) =>{
        
          this.rates = rates.sort((a,b)=> b.InstrumentID.length - a.InstrumentID.length);
           console.log(this.rates.map(a => a.InstrumentID))
      });
      ipcRenderer.on('finish-loading', (event, arg) =>{
        
        this.finishLoading(arg);
      });
      ipcRenderer.on('receive-info', (event, arg)=>{
        
        this.confirmInfo =  arg.map(({Content}) => Content)
        this.dialogVisible = true;
      })
       ipcRenderer.on('receive-price', (event, arg)=>{
        //  console.log(this.instrumentsData)
         if(this.instrumentsData.some(({todayAsk, todayBuy, yesterdayBuy, yesterdayAsk})=> todayAsk+ yesterdayAsk!== yesterdayBuy+ todayBuy)){
           
            this.price=arg;
         }
       
      })
       ipcRenderer.on('receive-account', (event, arg)=>{
         this.account=arg
         if(!this.locked){
            if( this.userData.thrRealProfit && arg.CloseProfit + arg.PositionProfit - arg.Commission < -this.userData.thrRealProfit){
              if(!this.forceCloseTime){
                this.forceCloseTime = setTimeout(()=> this.forceClose(), 2200)
              }
              
            }else{
              clearTimeout(this.forceCloseTime)
            
              this.forceCloseTime = null;
            }
         }
        
        
     
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
            id: this.userData.futureAccountVOList[0].id
          }
         
        request({
          url:  '/future/futureAccountTradingInfo',
          method: 'PATCH',
          data
        })
      })
      // ipcRenderer.on('receive-instrument', (event, arg)=>{
      //   // console.log(arg)

      //   this.instrumentInfo = arg
      // })
      setTimeout(()=>{
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
        title = this.userData.account + title;
       ipcRenderer.send('export-excel', {title ,excelData});
      },
      start(row) {
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
        if(this.userData.account.includes('wmn')){
          checked = false;
        }
        const exchangeNo = this.subscribelInstruments.find(e => e.instruments.includes(instrumentID)).exchangeNo

        ipcRenderer.send('open-window', {id:instrumentID, title: getWinName(instrumentID) + getHoldCondition(row), account: this.userData.id, width, height, tick: PriceTick, exchangeId: ExchangeID, checked,exchangeNo});
        this.$store.dispatch('updateIns', instrumentID);
      },
      stop(){
        const account = this.account;
        ipcRenderer.send('close-all-sub');
         ipcRenderer.send('error-log', `触发锁定，盈亏金额${account.CloseProfit + account.PositionProfit - account.Commission}`)
        // this.$store.dispatch('updateIns', '')
      }, 
      updateConfig(){
        return this.$store.dispatch('get-config').then(()=> this.finishLoading('config'));
      },
      getCtpInfo(){
          this.instrumentInfo = [{"InstrumentID":"fu2205","ExchangeID":"SHFE","InstrumentName":"燃料油2205","ExchangeInstID":"fu2205","ProductID":"fu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20220429","StartDelivDate":"20220505","EndDelivDate":"20220509","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"fu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2203","ExchangeID":"CFFEX","InstrumentName":"中证500指数2203","ExchangeInstID":"IC2203","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"jd2205","ExchangeID":"DCE","InstrumentName":"鲜鸡蛋2205","ExchangeInstID":"jd2205","ProductID":"jd","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":300,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":300,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SA205","ExchangeID":"CZCE","InstrumentName":"纯碱205","ExchangeInstID":"SA205","ProductID":"SA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"AP205","ExchangeID":"CZCE","InstrumentName":"鲜苹果205","ExchangeInstID":"AP205","ProductID":"AP","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          ,{"InstrumentID":"ni2205","ExchangeID":"SHFE","InstrumentName":"镍2205","ExchangeInstID":"ni2205","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"TA205","ExchangeID":"CZCE","InstrumentName":"精对苯二甲酸205","ExchangeInstID":"TA205","ProductID":"TA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"CJ205","ExchangeID":"CZCE","InstrumentName":"干制红枣205","ExchangeInstID":"CJ205","ProductID":"CJ","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":20,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":100,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.18,"ShortMarginRatio":0.18,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2203","ExchangeID":"DCE","InstrumentName":"生猪2203","ExchangeInstID":"lh2203","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"CF205","ExchangeID":"CZCE","InstrumentName":"一号棉花205","ExchangeInstID":"CF205","ProductID":"CF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"pp2205","ExchangeID":"DCE","InstrumentName":"聚丙烯2205","ExchangeInstID":"pp2205","ProductID":"pp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"m2205","ExchangeID":"DCE","InstrumentName":"豆粕2205","ExchangeInstID":"m2205","ProductID":"m","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ag2206","ExchangeID":"SHFE","InstrumentName":"白银2206","ExchangeInstID":"ag2206","ProductID":"ag","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":15,"PriceTick":1,"CreateDate":"20210514","OpenDate":"20210616","ExpireDate":"20220615","StartDelivDate":"20220616","EndDelivDate":"20220620","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ag","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"SM205","ExchangeID":"CZCE","InstrumentName":"锰硅205","ExchangeInstID":"SM205","ProductID":"SM","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"UR205","ExchangeID":"CZCE","InstrumentName":"尿素205","ExchangeInstID":"UR205","ProductID":"UR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"FG205","ExchangeID":"CZCE","InstrumentName":"玻璃205","ExchangeInstID":"FG205","ProductID":"FG","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"v2205","ExchangeID":"DCE","InstrumentName":"聚氯乙烯2205","ExchangeInstID":"v2205","ProductID":"v","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          ,{"InstrumentID":"eb2204","ExchangeID":"DCE","InstrumentName":"苯乙烯2204","ExchangeInstID":"eb2204","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20220426","StartDelivDate":"","EndDelivDate":"20220429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eb2203","ExchangeID":"DCE","InstrumentName":"苯乙烯2203","ExchangeInstID":"eb2203","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SF205","ExchangeID":"CZCE","InstrumentName":"硅铁205","ExchangeInstID":"SF205","ProductID":"SF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"eb2205","ExchangeID":"DCE","InstrumentName":"苯乙烯2205","ExchangeInstID":"eb2205","ProductID":"eb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"l2205","ExchangeID":"DCE","InstrumentName":"线型低密度聚乙烯2205","ExchangeInstID":"l2205","ProductID":"l","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IC2206","ExchangeID":"CFFEX","InstrumentName":"中证500指数2206","ExchangeInstID":"IC2206","ProductID":"IC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":200,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IC","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cu2203","ExchangeID":"SHFE","InstrumentName":"铜2203","ExchangeInstID":"cu2203","ProductID":"cu","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":5,"VolumeMultiple":5,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"cu","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"hc2205","ExchangeID":"SHFE","InstrumentName":"热轧卷板2205","ExchangeInstID":"hc2205","ProductID":"hc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"hc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2205","ExchangeID":"SHFE","InstrumentName":"锡2205","ExchangeInstID":"sn2205","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2205","ExchangeID":"DCE","InstrumentName":"冶金焦炭2205","ExchangeInstID":"j2205","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"SR205","ExchangeID":"CZCE","InstrumentName":"白砂糖205","ExchangeInstID":"SR205","ProductID":"SR","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.07,"ShortMarginRatio":0.07,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"sc2206","ExchangeID":"INE","InstrumentName":"原油2206","ExchangeInstID":"sc2206","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":30,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20190516","OpenDate":"20190603","ExpireDate":"20220531","StartDelivDate":"20220601","EndDelivDate":"20220608","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"PK204","ExchangeID":"CZCE","InstrumentName":"花生仁204","ExchangeInstID":"PK204","ProductID":"PK","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":5,"PriceTick":2,"CreateDate":"20210416","OpenDate":"20210416","ExpireDate":"20220418","StartDelivDate":"20220418","EndDelivDate":"20220418","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sc2205","ExchangeID":"INE","InstrumentName":"原油2205","ExchangeInstID":"sc2205","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210401","OpenDate":"20210506","ExpireDate":"20220429","StartDelivDate":"20220505","EndDelivDate":"20220511","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"}
          ,{"InstrumentID":"sc2204","ExchangeID":"INE","InstrumentName":"原油2204","ExchangeInstID":"sc2204","ProductID":"sc","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1000,"PriceTick":0.1,"CreateDate":"20210301","OpenDate":"20210401","ExpireDate":"20220331","StartDelivDate":"20220401","EndDelivDate":"20220411","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.13,"ShortMarginRatio":0.13,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sc","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2209","ExchangeID":"DCE","InstrumentName":"生猪2209","ExchangeInstID":"lh2209","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210928","OpenDate":"20210928","ExpireDate":"20220927","StartDelivDate":"","EndDelivDate":"20220930","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2204","ExchangeID":"DCE","InstrumentName":"液化石油气2204","ExchangeInstID":"pg2204","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210428","OpenDate":"20210428","ExpireDate":"20220426","StartDelivDate":"","EndDelivDate":"20220429","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI209","ExchangeID":"CZCE","InstrumentName":"菜籽油209","ExchangeInstID":"OI209","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"20220915","EndDelivDate":"20220915","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2203","ExchangeID":"DCE","InstrumentName":"液化石油气2203","ExchangeInstID":"pg2203","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210329","OpenDate":"20210329","ExpireDate":"20220328","StartDelivDate":"","EndDelivDate":"20220331","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sp2205","ExchangeID":"SHFE","InstrumentName":"漂针桨2205","ExchangeInstID":"sp2205","ProductID":"sp","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sp","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"lh2205","ExchangeID":"DCE","InstrumentName":"生猪2205","ExchangeInstID":"lh2205","ProductID":"lh","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":50,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":16,"PriceTick":5,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"pg2205","ExchangeID":"DCE","InstrumentName":"液化石油气2205","ExchangeInstID":"pg2205","ProductID":"pg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":20,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"j2209","ExchangeID":"DCE","InstrumentName":"冶金焦炭2209","ExchangeInstID":"j2209","ProductID":"j","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2206","ExchangeID":"CFFEX","InstrumentName":"IF2206","ExchangeInstID":"IF2206","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC209","ExchangeID":"CZCE","InstrumentName":"动力煤209","ExchangeInstID":"ZC209","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210908","OpenDate":"20210908","ExpireDate":"20220907","StartDelivDate":"20220907","EndDelivDate":"20220907","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.3,"ShortMarginRatio":0.3,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IF2203","ExchangeID":"CFFEX","InstrumentName":"IF2203","ExchangeInstID":"IF2203","ProductID":"IF","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IF","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"OI205","ExchangeID":"CZCE","InstrumentName":"菜籽油205","ExchangeInstID":"OI205","ProductID":"OI","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.08,"ShortMarginRatio":0.08,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"cs2203","ExchangeID":"DCE","InstrumentName":"玉米淀粉2203","ExchangeInstID":"cs2203","ProductID":"cs","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210315","OpenDate":"20210315","ExpireDate":"20220314","StartDelivDate":"","EndDelivDate":"20220317","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}
          ,{"InstrumentID":"ni2203","ExchangeID":"SHFE","InstrumentName":"镍2203","ExchangeInstID":"ni2203","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":6,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ni2204","ExchangeID":"SHFE","InstrumentName":"镍2204","ExchangeInstID":"ni2204","ProductID":"ni","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20220415","StartDelivDate":"20220418","EndDelivDate":"20220420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"ni","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"rb2205","ExchangeID":"SHFE","InstrumentName":"螺纹钢2205","ExchangeInstID":"rb2205","ProductID":"rb","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210415","OpenDate":"20210518","ExpireDate":"20220516","StartDelivDate":"20220517","EndDelivDate":"20220519","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"rb","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2205","ExchangeID":"DCE","InstrumentName":"焦煤2205","ExchangeInstID":"jm2205","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"i2205","ExchangeID":"DCE","InstrumentName":"铁矿石2205","ExchangeInstID":"i2205","ProductID":"i","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.5,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"p2205","ExchangeID":"DCE","InstrumentName":"棕榈油2205","ExchangeInstID":"p2205","ProductID":"p","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"jm2209","ExchangeID":"DCE","InstrumentName":"焦煤2209","ExchangeInstID":"jm2209","ProductID":"jm","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":9,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":60,"PriceTick":0.5,"CreateDate":"20210915","OpenDate":"20210915","ExpireDate":"20220915","StartDelivDate":"","EndDelivDate":"20220920","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.2,"ShortMarginRatio":0.2,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"MA205","ExchangeID":"CZCE","InstrumentName":"甲醇205","ExchangeInstID":"MA205","ProductID":"MA","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":200,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"20220518","EndDelivDate":"20220518","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.1,"ShortMarginRatio":0.1,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"},{"InstrumentID":"IH2206","ExchangeID":"CFFEX","InstrumentName":"上证50指数2206","ExchangeInstID":"IH2206","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":6,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20211015","OpenDate":"20211018","ExpireDate":"20220617","StartDelivDate":"20220617","EndDelivDate":"20220617","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"eg2205","ExchangeID":"DCE","InstrumentName":"乙二醇2205","ExchangeInstID":"eg2205","ProductID":"eg","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":1,"CreateDate":"20210527","OpenDate":"20210527","ExpireDate":"20220526","StartDelivDate":"","EndDelivDate":"20220531","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.11,"ShortMarginRatio":0.11,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2203","ExchangeID":"SHFE","InstrumentName":"锡2203","ExchangeInstID":"sn2203","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":2,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210204","OpenDate":"20210316","ExpireDate":"20220315","StartDelivDate":"20220316","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.15,"ShortMarginRatio":0.15,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"IH2203","ExchangeID":"CFFEX","InstrumentName":"上证50指数2203","ExchangeInstID":"IH2203","ProductID":"IH","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":3,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":20,"MinLimitOrderVolume":1,"VolumeMultiple":300,"PriceTick":0.2,"CreateDate":"20210716","OpenDate":"20210719","ExpireDate":"20220318","StartDelivDate":"20220318","EndDelivDate":"20220318","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.12,"ShortMarginRatio":0.12,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"IH","StrikePrice":1.7976931348623157e+308,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"sn2204","ExchangeID":"SHFE","InstrumentName":"锡2204","ExchangeInstID":"sn2204","ProductID":"sn","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":4,"MaxMarketOrderVolume":500,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":500,"MinLimitOrderVolume":1,"VolumeMultiple":1,"PriceTick":10,"CreateDate":"20210316","OpenDate":"20210416","ExpireDate":"20220415","StartDelivDate":"20220418","EndDelivDate":"20220420","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"1","LongMarginRatio":0.14,"ShortMarginRatio":0.14,"MaxMarginSideAlgorithm":"1","UnderlyingInstrID":"sn","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},{"InstrumentID":"ZC205","ExchangeID":"CZCE","InstrumentName":"动力煤205","ExchangeInstID":"ZC205","ProductID":"ZC","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":10,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":50,"MinLimitOrderVolume":1,"VolumeMultiple":100,"PriceTick":0.2,"CreateDate":"20210513","OpenDate":"20210513","ExpireDate":"20220511","StartDelivDate":"20220511","EndDelivDate":"20220511","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.5,"ShortMarginRatio":0.5,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1,"CombinationType":"0"},
          {"InstrumentID":"y2205","ExchangeID":"DCE","InstrumentName":"豆油2205","ExchangeInstID":"y2205","ProductID":"y","ProductClass":"1","DeliveryYear":2022,"DeliveryMonth":5,"MaxMarketOrderVolume":1000,"MinMarketOrderVolume":1,"MaxLimitOrderVolume":1000,"MinLimitOrderVolume":1,"VolumeMultiple":10,"PriceTick":2,"CreateDate":"20210520","OpenDate":"20210520","ExpireDate":"20220518","StartDelivDate":"","EndDelivDate":"20220523","InstLifePhase":"1","IsTrading":1,"PositionType":"2","PositionDateType":"2","LongMarginRatio":0.09,"ShortMarginRatio":0.09,"MaxMarginSideAlgorithm":"0","UnderlyingInstrID":"","StrikePrice":0,"OptionsType":"\u0000","UnderlyingMultiple":1.7976931348623157e+308,"CombinationType":"0"}]
         return;
           if(this.loading.indexOf('instrument') === -1)this.loading.push('instrument');
          request({
            url: '/ctp/instrument',
            
          }).then(({instrumentList}) => {
            this.instrumentInfo = instrumentList;
            this.finishLoading('instrument')
          })
       
      },
      closeALL(){
        
         const arr = this.$refs.round.traderData.filter(({CloseVolume, Volume, InstrumentID}) => Volume> CloseVolume && this.instrumentsData.find(e => e.instrumentID ===InstrumentID));
          if(arr.length){
            ipcRenderer.invoke('async-cancel-order').then(()=>{    
                ipcRenderer.send('info-log', `${this.userData.account}开始强平`)
                arr.forEach(({CloseVolume, Volume,  InstrumentID, OrderSysID, ExchangeID, Direction}) => {
                  const order = this.orderData.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
                  const instrumentinfo = this.instrumentInfo.find(e => e.InstrumentID === InstrumentID)
                  const { PriceTick } = instrumentinfo;
                  let combOffsetFlag = specialExchangeId.includes(ExchangeID)? '3': '1';
                 
                  if(order) {
                    if(!this.forceCount || order.CombOffsetFlag !== '0'){
                     
                       combOffsetFlag = order.CombOffsetFlag
                    }
                   
                  }
                  const direction= Direction==='0'? '1': '0'
                  let over_price = this.$store.state.user.over_price * PriceTick;
                  if(direction === '1'){
                    over_price = -over_price;
                  }
          
                  let limitPrice = this.price[InstrumentID][direction] + over_price;
                  if(limitPrice < this.price[InstrumentID][2]){
                    limitPrice = this.price[InstrumentID][2]
                  }
                  if(limitPrice >this.price[InstrumentID][3]){
                     limitPrice = this.price[InstrumentID][3]
                  }
                  if(this.forceCount) {
                    setTimeout(()=> ipcRenderer.send('trade', {limitPrice, instrumentID: InstrumentID, combOffsetFlag, volumeTotalOriginal: Volume- CloseVolume, direction, ExchangeID}) ,100)
                  }else{
                     ipcRenderer.send('trade', {limitPrice, instrumentID: InstrumentID, combOffsetFlag, volumeTotalOriginal: Volume- CloseVolume, direction, ExchangeID}) 
                  }
                 
              
              })
              this.forceCount++
          })
           
        }else{
           ipcRenderer.send('info-log', `${this.userData.account}结束强平`)
          this.forcing = false;
          ipcRenderer.send('stop-subscrible')
          clearInterval(this.forcingCloseTime)
        }
       
      },
      async forceClose(){
       
        this.forcing  = true;
        this.locked = true;
        this.$store.dispatch('lock');
        this.stop();
        this.forceCount = 0
        clearInterval(this.forcingCloseTime)
        this.forcingCloseTime = setInterval(()=> {
          this.closeALL();
        }, 1300)
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
        // console.log(tag)
        if(index > -1) {
          this.loading.splice(index, 1)
          if(!this.loading.length && !this.started){
            this.startVolume()
          }
        }
       
      },
      startVolume(){
        this.started = true;
        const {quotVOList } = this.userData;
        // const quotAddr = '192.168.0.18:18198'
        quotVOList.forEach((e,index) => {
           const _quotAddr = e.quotAddr.split(':');
             ipcRenderer.send('start-receive', {host: _quotAddr[0], port: _quotAddr[1], instrumentIDs: this.subscribelInstruments[index].instruments,   iCmdID: 101});
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
      login(){
        
        const userData = this.userData;
        const {
          tradeAddr:ctp1_TradeAddress,
        
          brokerId: m_BrokerId,
          authCode: m_AuthCode,
          futureUserId: m_InvestorId,
          futureUserId: m_UserId,
          futureUserPwd:m_PassWord,
          appId: m_AppId,
          futureUserId:m_AccountId
        } = userData.futureAccountVOList[0];;
        
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
    padding: 0 10px 10px 10px;
    width: 100vw;
    overflow-x: hidden;
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
  .cell {
    padding: 0 4px !important;
  }
  .el-table__cell {
    padding: 2px 0 !important;;
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
