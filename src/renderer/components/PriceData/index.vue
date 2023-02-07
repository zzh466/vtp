<template>
  <div class="price-body "  @dblclick="mouseTrade" v-loading='loading' @contextmenu="conditionTrade"> 
    <div class="breath-alert" v-show="showalert"></div>
    <Controller  v-if="showController"/>
    <div class="hold-order">
      <div class="buy-orders">
        <div class="buy-order" v-for="index of broadcast['1']" :style="{ width: stepwidth +'px'}"  :key="index"></div>
      </div>
       <div class="sell-orders">
         <div class="sell-order" v-for="index of broadcast['0']" :style="{ width: stepwidth +'px'}" :key="index"></div>
       </div>
    </div>
    <canvas @mousemove="move" id="can" :width="width + 'px'" :height="height + 'px'"></canvas>
    <div  class="price-tick" v-show="showbar" :style="{ width: stepwidth +'px', left: left + 'px' ,}"></div>
    <el-dialog title="条件单" width="750px" :visible.sync="showCondition" top="5px" :close-on-click-modal="false">
       <el-form ref="form" :model="editcondition" label-width="80px" size="small" :inline="true">
            <el-form-item label='触发价格' prop='price' :rules='[{ required: true, message: `请填写价格`,trigger: "blur"}, { validator: validator, trigger: "blur" }]'>
                <el-input v-model='editcondition.price'  :min='arg.LowerLimitPrice' :step="$route.query.tick" :max="arg.UpperLimitPrice"  type="number"></el-input>
            </el-form-item>
             <el-form-item label='条件' prop='contingentCondition' required>
                <el-select v-model="editcondition.contingentCondition">
                    <el-option v-for="e in limitcondition" :key='e.value'  :value='e.value' :label='e.label'></el-option>
                    
                </el-select>
            </el-form-item>
             <el-form-item label='超价' prop='overprice' required>
                  <el-input v-model='editcondition.overprice' type="number"></el-input>
                    
              </el-form-item>
            <el-form-item label='方向' prop='direction' required>
                <el-select v-model="editcondition.direction">
                    <el-option value='0' label='买'></el-option>
                    <el-option value='1' label='卖'></el-option>
                </el-select>
            </el-form-item>
            
              <el-form-item label='手数' prop='volume' required>
                  <el-input v-model='editcondition.volume'  type="number"></el-input>
                    
              </el-form-item>
              
             
       </el-form>
        <div slot='footer'>
              <el-button size="small" @click='showCondition=false'>取 消</el-button>
                <el-button size="small" type="primary" @click="cofirmCondition">确 定</el-button>
        </div>
    </el-dialog>
  <!-- <div class="condition-tag">
    <el-tag style="margion-top: 3px" effect="plain" v-for='condition, index in conditions' :key="condition.price" @click="addCondition(index)" closable size='mini' @close='closeCondition(index)'>条件单{{index + 1}}</el-tag>
  </div>   -->
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';
import Chart from './chart'
import Gen from './hotkey';
import {getWinName, getHoldCondition, setClientSize, specialExchangeId} from '../../utils/utils'
import {Notification} from 'element-ui'
import Controller from './controller.vue'
export default {
  components: {
    Controller
  },
  // computed:{
  //   showalert: function(){

  //   }
  // },
  watch:{
    traded(val) {
      const buy = [], ask =[];
      val.forEach(({Direction, Volume, Price}) => {
        let same = buy, other=ask;
        if(Direction === '1'){
            same = ask;
            other = buy;
        }
        while(Volume){
          if(other.length){
            other.shift();
          }else{
            same.push(Price)
          }
          Volume--
        }
      })
      let direction = '0', price = buy;
      if(ask.length){
        direction = '1'
        price=ask;
      }
      const results =  {
        direction,
        price
      }
      this.chart.traded = results;
      this.chart.renderTradeOrder();
      console.log(results)
      return results;

    },
    config: {
      deep: true,
      handler({volume, type, closeType}) {
        
        const {id,accountIndex} = this.$route.query;
        const title =getWinName(id, accountIndex, volume, type, closeType) + getHoldCondition(this.instrumet);
        ipcRenderer.send('change-title', {id, title});
      }
    }
  },
  mounted(){
      const chartDom = document.getElementById('can');
      this.instrumet = {};
     this.tasks = [];
     const {id, tick, exchangeId, showController} = this.$route.query;

     
      const config = this.setConfig()
      ipcRenderer.send('register-event', id);
     this.showController = !!showController;
      window.onkeydown =(e)=>{
        
        if(this.showCondition){
          if(e.keyCode === 13){
            this.cofirmCondition()
          }
       
          return
        }
        this.func(e, this);
      }
      let resizeTimeout;
      window.onresize =(e)=>{
        if(!this.chart) return;
        let {innerWidth, innerHeight} =e.target;
       
        clearTimeout(resizeTimeout);
        resizeTimeout = setTimeout(()=> {
          this.width = innerWidth - 80;
          this.height = innerHeight-20;
          if(this.height < 280){
            this.height = 280;
          }
          setClientSize(innerWidth, innerHeight)
          this.$nextTick(function(){
            this.chart.resize( this.width, this.height);
            if(this.arg){
               this.chart.render(this.arg)
            }
          })
          
        }, 500)
      }
      
      // const p = new Promise(a => {
        //  ipcRenderer.invoke('get-pirceTick', id).then(({PriceTick: tick, ExchangeID: exchangeId}) => {
    
        this.exchangeId = exchangeId;
          
        this.chart = new Chart(chartDom, this.width, this.height,tick, config);
        console.log(config)
        //   a();
         
        // })
      // })
        
        ipcRenderer.on(`receive-${id}`, (event, arg) => {
          // p.then(()=>{
            
           
            if(arg){
              // ipcRenderer.send('info-log', JSON.stringify(Object.values(arg)));
              console.log(arg)
              this.arg = arg;
              this.chart.render(arg)
              this.calc(arg)
            }
            
          // })
        })
        ipcRenderer.on(`update-config`, (event, arg) => {
          const config = this.setConfig();
          const {
              barToBorder,
              
              barWidth = 10,
              
              volumeScaleCount,
              volumeScaleHeight = 30,
              volumeScaleTick,
              volumeScaleType
          } = config
        
          
          this.chart.barToBorder = barToBorder;
          this.chart.stepwidth =barWidth;
          this.chart.stepHeight = volumeScaleHeight;
          this.chart.volumeScaleType = volumeScaleType;
          this.chart.volumeScaleCount =volumeScaleCount;
          this.chart.volumeScaleTick = volumeScaleTick;
          
          this.chart.ctx.clearRect(0, 0, this.width, this.height);
          this.chart.resize( this.width, this.height);
          if(this.arg){
              this.chart.render(this.arg)
          }
        })
      
      
      // ipcRenderer.on('place-order', (_, field) => {
       
      //   p.then(()=>{
      //     if(field && field.OrderSubmitStatus === '4'){
      //       Notification({
      //         type: 'error',
      //         message: field.StatusMsg
      //       })
      //       return;
      //     }
      //     this.chart.placeOrder.push(field);
      //     this.chart.renderBakcground();
      //     this.chart.renderVolume();  
      //     this.chart.renderPlaceOrder();
      //     this.chart.renderHighandLow()
      //   })
      // })
      ipcRenderer.on('total-order', (_, orders, current = {}) => { 
        // p.then(()=>{
          console.log(orders, current, 111)
          const arr = [];const id = this.$route.query.id;
          for(let key in orders){
            if(orders[key].InstrumentID === id){
              arr.push(orders[key])
            }
          }
          
          if(arr.length &&this.chart.data.length ){
            let cancel = 0;
            let open = 0
            arr.forEach(e => {
              const {OrderStatus, CombOffsetFlag, VolumeTraded} = e;
              if(CombOffsetFlag === '0'){
                open = VolumeTraded + open;
              }
              if(OrderStatus === '5'){
                cancel += 1;
              }
             
            })

            this.instrumet.todayVolume = open;
            this.instrumet.todayCancel = cancel;
            this.update();
            this.chart.placeOrder = arr;
            this.chart.renderBakcground();
            this.chart.renderVolume();  
            this.chart.renderPlaceOrder();
            this.chart.renderHighandLow();
            if(this.tasks.length && current.OrderStatus === '5'){
              this.tasks.forEach(e => e());
              this.tasks =[];
            }
          }
         
         
        //  })
      })
 
      ipcRenderer.on('order-error', (_, message) => {
        if(message && message.ErrorID === 30) return;
        Notification({
        type: 'error',
        message,
        duration: 1500,
        position: 'bottom-right',
      })})
      ipcRenderer.on('instrumet-data', (_, instrumet) => {
        if(!instrumet){
         
          instrumet = {
            todayBuy: 0,
            todayAsk: 0,
            yesterdayAsk: 0,
            yesterdayBuy: 0
          }
        }
        this.instrumet = instrumet;
        this.update(instrumet)
       
       
      })
      let audio = new Audio()
      audio.src = __static+ "/trade.wav";
      ipcRenderer.on('clear-trader', ()=>{
        this.traded = [];
      })
      ipcRenderer.on('trade-order', (_, field, flag) => {
        let {Direction, Volume, OrderSysID, ExchangeID, CombOffsetFlag, TradeID, TradeType} = field;
          const index = this.traded.findIndex(e => e.TradeType === TradeType && e.ExchangeID + e.OrderSysID + e.TradeID===ExchangeID + OrderSysID + TradeID);
          if(index > -1){
            console.log(this.traded, field, 1112356)
            return;
          }
          if(!flag){
            
            const item =  this.chart.placeOrder.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
           if(item){
             CombOffsetFlag = item.CombOffsetFlag;
           }
            if(CombOffsetFlag === '0'){
              const key = Direction  === '0' ? 'todayBuy': 'todayAsk';
              this.instrumet[key] += Volume;
            }else{

              let yesterDay =  Direction  === '0' ? 'yesterdayAsk': 'yesterdayBuy';
              let todayAsk = Direction  === '0' ? 'todayAsk': 'todayBuy';
              //中金先平今再平昨
              console.log(CombOffsetFlag)
              if(this.exchangeId === 'CFFEX'){
                const temp= todayAsk;
                todayAsk = yesterDay;
                yesterDay = temp
              }

              if(this.instrumet[yesterDay] && CombOffsetFlag!== '3'){
                 if(this.instrumet[yesterDay] >= Volume){
                   this.instrumet[yesterDay] -= Volume
                 }else {
                    this.instrumet[todayAsk] -= Volume -  this.instrumet[yesterDay];
                   this.instrumet[yesterDay]  = 0;
                   
                 }
                 
               }else{
                 this.instrumet[todayAsk] -= Volume;
               }
            }
            this.update();
          }
          //有成交单进来就把先撤后下的队列给清除 防止出现撤单后成交依然挂着
          this.tasks =[]; 
          this.traded.push(field);
          if(this.startTrade){
            audio.load();
            audio.play();
          }
          //  console.log(this.traded.map(({Direction, Volume, Price}) => ({Direction, Volume, Price})))
          
        // })
      })
    
      ipcRenderer.on('receive-broadcast', (_, data) => {
        
        if(data && this.broadcastOpenInterest){
          this.broadcast= data
         
          console.log(this.broadcast)
        }
      })
      ipcRenderer.on('broadcast-indicator', (_, type, value)=>{
        value = parseFloat(value)
        switch(type){
          case "lpdm_1min":
            this.lpdm_1min = value;
            break;
          case 'vm_1min':
            this.vm_1min = value;
        }
      })
      // let price_ =2902.00;
      // setInterval(()=>{
      //   price_ = price_ - 0.5;
      //   const arg = {
      //     ActionDay: "20211203",
      //     AskPrice1: price_ + 1,
      //     AskPrice2: price_ + 1.5,
      //     AskPrice3: price_ + 2,
      //     AskPrice4: price_ + 2.5,
      //     AskPrice5: price_ + 3,
      //     AskVolume1: 2,
      //     AskVolume2: 3,
      //     AskVolume3: 2,
      //     AskVolume4: 3,
      //     AskVolume5: 2,
      //     AveragePrice: 0,
      //     BidPrice1: price_ - 1,
      //     BidPrice2: price_ - 1.5,
      //     BidPrice3: price_ - 2,
      //     BidPrice4: price_ - 2.5,
      //     BidPrice5:price_ - 3,
      //     BidVolume1: 5,
      //     BidVolume2: 4,
      //     BidVolume3: 1,
      //     BidVolume4: 3,
      //     BidVolume5: 3,
      //     ClosePrice: 3,
      //     CurrDelta: 3,
      //     ExchangeID: "",
      //     ExchangeInstID: "",
      //     HighestPrice: 0,
      //     InstrumentID: "j2201",
      //     LastPrice: price_,
      //     LowerLimitPrice: 2467,
      //     LowestPrice: 0,
      //     OpenInterest: 9469,
      //     OpenPrice: 0,
      //     PreClosePrice: 2876,
      //     PreDelta: 0,
      //     PreOpenInterest: 9469,
      //     PreSettlementPrice: 2902,
      //     SettlementPrice: 0,
      //     TradingDay: "20211203",
      //     Turnover: 0,
      //     UpdateMillisec: 0,
      //     UpdateTime: "18:33:52",
      //     UpperLimitPrice: 3337,
      //     Volume: 0}
      //     this.chart.render(arg)
      // }, 500)

  },
  data () {
    let  height  = window.innerHeight -20;
    if(height < 280){
        height = 280;
    }
    this.args = [];
    return {
      showalert: false,
      showController:false,
      width: window.innerWidth -80 ,
      height,
      showbar: false,
      left: 0,
      config: {
        volume: 1,
        type: '0',
        closeType: '0'
      },
      traded: [],
      loading: false,
      stepwidth: 10,
      broadcast: {
        '0': 0,
        '1': 0
      },
      showCondition: false,
     
      editcondition: {
        price: '',
        overprice: 1,
        direction: '0',
        volume: 1,
        contingentCondition: '6'
      },
      arg: {},
      limitcondition: [{
        label: '最新价大于等于条件价',
        value: '6'
      },
      {
        label: '最新价小于等于条件价',
        value: '8'
      },{
        label: '买一价大于等于条件价',
        value: 'E'
      },
      {
        label: '买一价小于等于条件价',
        value: 'H'
      },{
        label: '卖一价大于等于条件价',
        value: 'A'
      },
      {
        label: '卖一价小于等于条件价',
        value: 'C'
      }]
    }
  },
  methods: {
    setConfig(){
      const {account,configId} = this.$route.query;
       const configs =JSON.parse(localStorage.getItem(`config-${account}`));
       
       const config = configs.find(e => e.id === +configId);
    
      console.log(config, this.$route.query);
      if(!config) return;
      console.log(config);
      const {sysCloseTStrategy='0', sysCloseType='0', sysOrderVolume=1} = config;
      this.config.type = sysCloseTStrategy.toString();
      this.config.closeType=sysCloseType.toString();
      this.config.volume = sysOrderVolume ;
      this.broadcastOpenInterest = config.broadcastOpenInterest;
      this.stepwidth = config.barWidth ;
    

       this.func = Gen(config.hotKey)
       return config;
    },
    move(e){
      const {x ,y} = e;
      
      if(x > 104 && x < this.stepwidth * this.chart.count + 105 && y > 54){
        const left = x - (x-105)%this.stepwidth;
       
        this.showbar = true;
        this.left = left
      }else {
        this.showbar = false;
      }
    },
    update(){
        const instrumet =  this.instrumet;
        const {id, accountIndex} = this.$route.query;
        const {volume, type, closeType} = this.config;
        const title =getWinName(id, accountIndex, volume, type, closeType) + getHoldCondition(instrumet);
        ipcRenderer.send('change-title', {id, title});
    },
    conditionTrade(){
      if(!this.showbar || !this.chart.data.length|| this.showCondition )return;
      const index = (this.left - 105) / this.stepwidth;
      let {start} = this.chart;
      const  limitPrice = +this.chart.data[index + start].price;
      const traded = this.chart.traded;
      if(traded.direction && traded.price.length){
        this.editcondition.direction = traded.direction === '0'? '1': '0';
      }
      const {LastPrice} = this.arg;
      
      if(LastPrice > limitPrice){
        this.editcondition.contingentCondition = '8'
      }else{
        this.editcondition.contingentCondition = '6'
      }
      this.showCondition = true;
      this.editcondition.price = limitPrice;
    },
    mouseTrade(){
      if(!this.showbar || !this.chart.data.length || this.showCondition) return;
      const index = (this.left - 105) / this.stepwidth;
      let {buyIndex, askIndex, start, lowerLimitindex, UpperLimitindex} = this.chart;
      buyIndex = buyIndex - start;
      askIndex = askIndex -start
      lowerLimitindex= lowerLimitindex- start;
      UpperLimitindex = UpperLimitindex -start;
      if(index < lowerLimitindex || index > UpperLimitindex) return;
      if(index >buyIndex  && index < askIndex) return;
      let direction = '1';
      if(index <= buyIndex &&  (buyIndex && buyIndex > lowerLimitindex)){
        direction = '0'
      }
      const  limitPrice = +this.chart.data[index + start].price;
      let volumeTotalOriginal = this.config.volume;
       ipcRenderer.send('info-log', `鼠标下单，${limitPrice}, 模式${this.config.type}`)
      this.putOrder(limitPrice, direction,volumeTotalOriginal)

    },
    putOrder(limitPrice, direction, volumeTotalOriginal = this.config.volume, contingentCondition, stopPrice){
       const instrumentID = this.$route.query.id;
      let combOffsetFlag = '0'
      let {traded, holdVolume} = this.chart;
      holdVolume = holdVolume[direction];
      limitPrice = parseFloat(limitPrice)
      if(traded.direction && traded.price.length){
        if(direction !== traded.direction){
          combOffsetFlag = '1';
          if(volumeTotalOriginal + holdVolume> traded.price.length || this.config.closeType === '0'){
            volumeTotalOriginal = traded.price.length - holdVolume;
          }

        }
       
      }
      if(volumeTotalOriginal <=0){
        combOffsetFlag = '0';
         volumeTotalOriginal = this.config.volume
      }
      const traderData= this.checkLock(direction, volumeTotalOriginal,combOffsetFlag);
      if(!traderData) return
      console.log({limitPrice, instrumentID, ...traderData})
      this.startTrade = true;
      
      if(limitPrice < this.chart.lowerLimitPrice){
        limitPrice = this.chart.lowerLimitPrice;
      }
      if(limitPrice > this.chart.UpperLimitPrice){
        limitPrice = this.chart.UpperLimitPrice; 
      }
      if(Array.isArray(traderData.volumeTotalOriginal)){
        if(traderData.volumeTotalOriginal[0]){
          ipcRenderer.send('trade', {limitPrice, instrumentID, direction: traderData.direction, volumeTotalOriginal:traderData.volumeTotalOriginal[0],combOffsetFlag: '1', ExchangeID: this.exchangeId, contingentCondition, stopPrice})
        }
         
        ipcRenderer.send('trade', {limitPrice, instrumentID, direction: traderData.direction, volumeTotalOriginal: traderData.volumeTotalOriginal[1],combOffsetFlag: '3', ExchangeID: this.exchangeId, contingentCondition, stopPrice})

      }else{
        ipcRenderer.send('trade', {limitPrice, instrumentID, ...traderData, ExchangeID: this.exchangeId, contingentCondition, stopPrice})
      }
      
    },
    
    checkLock(direction, volumeTotalOriginal,combOffsetFlag){
      console.log(this.instrumet); 
      const {todayAsk, todayBuy, yesterdayAsk, yesterdayBuy} = this.instrumet;
      const { holdVolume} = this.chart;
      const hold = holdVolume[direction];
      const yesterDay = direction === '0'? yesterdayBuy: yesterdayAsk;
      const oppositeYesterDay = direction === '0'? yesterdayAsk: yesterdayBuy;
       const toDay = direction === '0'? todayBuy: todayAsk;
      const oppositeToday = direction === '0'? todayAsk: todayBuy;
      switch(this.config.type){
        case '0': 
          if(oppositeYesterDay>= volumeTotalOriginal + hold){
            combOffsetFlag = '1';
          }else {
            combOffsetFlag = '0';
          }
          if(combOffsetFlag === '0' && this.exchangeId === 'CFFEX' && yesterDay){
              Notification({
              message: '锁仓模式中金合约合约需要先将昨仓解锁再开仓'
            })
            return ;
          }
          break;
            
        case '1':
          
           if(oppositeYesterDay>= volumeTotalOriginal + hold){
              combOffsetFlag = '1';
            }else if(oppositeToday >= volumeTotalOriginal + hold){
               combOffsetFlag = '1'
              if(specialExchangeId.includes(this.exchangeId)){
                combOffsetFlag = '3'
              }
            }else if(specialExchangeId.includes(this.exchangeId) && combOffsetFlag === '1'){
              volumeTotalOriginal=[oppositeYesterDay, volumeTotalOriginal-oppositeYesterDay];
            }
          break;
        case '2':
          combOffsetFlag = '0'
          break;
        case '3':
          if(combOffsetFlag === '0' && oppositeYesterDay< volumeTotalOriginal + hold && oppositeToday < volumeTotalOriginal + hold){
              Notification({
                message: '持仓不够平仓模式下无法开仓'
              })
              return
          }
          combOffsetFlag = '1';
        
      }
      return {direction, volumeTotalOriginal,combOffsetFlag}
    },
    changeConfig(key, val){
      const value = this.config[key];
      if(value !== val){
        this.config[key] = val;
      }
    },
    changeHotKey(config){
      this.func = Gen(config);
    },
    calc(arg){
      
      
      let {TradingDay, UpdateTime, Volume, LastPrice} = arg;
      TradingDay = TradingDay.substr(0, 4) + '-' + TradingDay.substr(4, 2)+ '-'+ TradingDay.substr(6, 2);
      const time = +new Date(`${TradingDay} ${UpdateTime}`);
      const args = this.args;
      args.push({time, Volume, LastPrice})
      
      if(!this.vm_1min || !this.lpdm_1min) return;
      const last1Min = args[0]
      if(last1Min && time - last1Min.time  >= 60*1000){
        const arr = args.map(e => e.LastPrice)
      
        const A = (Math.max.apply(Math , arr) - Math.min.apply(Math , arr)) / this.lpdm_1min;
        const B = (Volume -last1Min.Volume) / this.vm_1min
        let C, buy = 0, ask = 0;
        for(let i =1 ; i < 6; i++){
          buy = buy + arg[`BidVolume${i}`]
          ask = ask + arg[`AskVolume${i}`]
        }
        if(buy > ask){
          C = buy / ask
        }else {
          C = ask /buy
        }
   
        const X = A *0.4 + B *0.2 + C *0.4;
        if(X >=5.5){
          this.showalert = true;
          
        }else{
          this.showalert = false;
        }
        console.log(X)
        this.args.shift();
      }
      
      
    },
    // init(data){
    //   const {LastPrice} = data;
    //   const xdata = this.intiXAxis(LastPrice, 0.5);
    //   const seriesData = this.initData(data);
    //   const option = {
    //     xAxis: {
    //       type: 'value',
    //       data: xdata,
    //       min: xdata[0]:
    //       max: xdata[xdata.length-1]
    //     },
    //     yAxis: {
    //       type: 'value',
    //       inverse: true,
          
    //       max: 50
    //     },
    //     series:{
    //       data: seriesData,
    //       type: 'bar',
    //     },
    //     animation: false
    //   }
    //   console.log(option)
    //   myChart.setOption(option)

    // },
    // intiXAxis(price, range){
    //   const data= [price];
    //   for(let i = 1; i <= 50; i++) {
    //     data.push(price + i*range);
    //     data.unshift(price - i*range)
    //   }
    //   return data;
    // },
    // setOptions(arg){
    //   const data = this.initData(arg);
    //   myChart.setOption({
    //     series:{
    //       data
    //     }
    //   })
    // },
    
    cofirmCondition(){
      this.$refs.form.validate((valid)=>{
        if(valid){
          
         let {price, overprice, contingentCondition, direction, volume} = this.editcondition;
         price = parseFloat(price)
          const {tick} = this.$route.query;
         
          if(direction === '1'){
            overprice = -overprice
          }
          
        const limitPrice = price+ tick * overprice;
        this.putOrder(limitPrice, direction, parseInt(volume), contingentCondition, price)
        this.showCondition = false;
        }
      })
    },
    validator(rules, value, callback) {
        value = parseFloat(value);
        const {LowerLimitPrice, UpperLimitPrice, LastPrice} = this.arg;
        const {tick} = this.$route.query;
        if(value < LowerLimitPrice || value > UpperLimitPrice){
          return callback(new Error(`价格必须大于${this.arg.LowerLimitPrice.toFixed(this.chart.decimal)}, 小于${this.arg.UpperLimitPrice.toFixed(this.chart.decimal)}`))
        }
        
        if(Math.abs((value -LastPrice).toFixed(2)*100) % (tick*100) !== 0) {
          return callback(new Error(`触发价格非最小单位的价格`))
        }
        callback();
      },
 
  },
  
}
</script>
<style>
.price-body {
  background-color: #000;
  padding: 5px;
  position: relative;
  height: 100vh;
}
#can {
  background-color: #000;
  position: relative;
}
.price-tick{
  position: absolute;
  border: #fff 1px solid;
  left: 150px;
  top: 55px;
  z-index: 10;
  height: 200px;
}
.hold-order {
  position: absolute;
  top: 1px;
  display: flex;
  width: 98vw;
  z-index: 10;
}
.buy-orders {
  display: flex;
  flex-basis: 50%;
  width: 50%;
   justify-content: flex-end;
  text-align: right;
}
.sell-orders {
  display: flex;
  flex-basis: 50%;
   width: 50%;
}
.buy-order {
  background-color: rgb(255, 130, 0);
  margin-left: 2px;
  width: 10px;
  height: 3px;
 
}
.sell-order {
   background-color: rgb(1, 233, 1);
  margin-left: 2px;
  width: 10px;
  height: 3px;
}
.condition-tag{
  position: absolute;
    top: 45px;
    right: 0;
    display: flex;
    flex-direction: column;
}
.breath-alert {
  position:absolute;
  z-index: 10;
    top: 1px;
	width:200px;
	height:10px;
	
	line-height:40px;
	border:1px solid #2b92d4;
	border-radius:1px;
	color:#fff;
	font-size:20px;
	text-align:center;
	cursor:pointer;
	box-shadow:0 1px 2px rgba(0,0,0,.3);
	overflow:hidden;
	background-image:-webkit-gradient(linear,left top,left bottom,from(hsl(352, 80%, 50%)),to(#bb0a0aee));
	-webkit-animation-timing-function:ease-in-out;
	-webkit-animation-name:breathe;
	-webkit-animation-duration:1000ms;
	-webkit-animation-iteration-count:infinite;
	-webkit-animation-direction:alternate;
}
@-webkit-keyframes breathe {
	0% {
	opacity:.5;
	box-shadow:0 1px 2px rgba(255,255,255,0.1);
}
100% {
	opacity:1;
	border:1px solid #cf2a40;
	box-shadow:0 1px 30px #920505;
}
}
</style>