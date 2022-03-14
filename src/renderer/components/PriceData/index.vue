<template>
  <div class="price-body"  @dblclick="mouseTrade" v-loading='loading'> 
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
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';
import Chart from './chart'
import Gen from './hotkey';
import {getWinName, getHoldCondition, setClientSize, specialExchangeId} from '../../utils/utils'
import {Notification} from 'element-ui'
export default {
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
        
        const id = this.$route.query.id;
        const title =getWinName(id, volume, type, closeType) + getHoldCondition(this.instrumet);
        ipcRenderer.send('change-title', {id, title});
      }
    }
  },
  mounted(){
      const chartDom = document.getElementById('can');
      this.instrumet = {};
     
      const {id,account, tick, exchangeId} = this.$route.query;
       const config =JSON.parse(localStorage.getItem(`config-${account}`));
      console.log(config);
      const {sysCloseTStrategy='0', sysCloseType='0', sysOrderVolume=1} = config;
      this.config.type = sysCloseTStrategy.toString() ;
      this.config.closeType=sysCloseType.toString() ;
      this.config.volume = sysOrderVolume ;
      this.broadcastOpenInterest = config.broadcastOpenInterest;
      this.stepwidth = config.barWeight ;
      ipcRenderer.send('register-event', id);

       this.func = Gen(config.hotKey)
     
      window.onkeydown =(e)=>{
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
        console.log(tick, exchangeId)
        this.exchangeId = exchangeId;
     
        this.chart = new Chart(chartDom, this.width, this.height,tick, config);
         
        //   a();
         
        // })
      // })
     
        ipcRenderer.on(`receive-${id}`, (event, arg) => {
          // p.then(()=>{
            
            console.log(arg)
            if(arg){
              // ipcRenderer.send('info-log', JSON.stringify(Object.values(arg)));
              this.arg = arg;
              this.chart.render(arg)
            }
            
          // })
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
      ipcRenderer.on('total-order', (_, orders) => { 
        // p.then(()=>{
          
          const arr = [];const id = this.$route.query.id;
          for(let key in orders){
            if(orders[key].ExchangeInstID === id){
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
        this.instrumet = instrumet;
        this.update(instrumet)
       
       
      })
      let audio = new Audio()
      audio.src = __static+ "/trade.wav";
      ipcRenderer.on('trade-order', (_, field, flag) => {
          
          if(!flag){
            const {Direction, Volume, OrderSysID, ExchangeID} = field;
            const item =  this.chart.placeOrder.find(e => e.ExchangeID + e.OrderSysID ===  ExchangeID + OrderSysID);
            const CombOffsetFlag = item.CombOffsetFlag;
            if(CombOffsetFlag === '0'){
              const key = Direction  === '0' ? 'todayBuy': 'todayAsk';
              this.instrumet[key] += Volume;
            }else{
              const yesterDay =  Direction  === '0' ? 'yesterdayAsk': 'yesterdayBuy';
              const todayAsk = Direction  === '0' ? 'todayAsk': 'todayBuy';
              if( this.instrumet[yesterDay] >= Volume){
                this.instrumet[yesterDay] -= Volume;
              }else{
                this.instrumet[todayAsk] -= Volume;
              }
            }
            this.update();
          }
           
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
    return {
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
      }
    }
  },
  methods: {
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
        const id = this.$route.query.id;
        const {volume, type, closeType} = this.config;
        const title =getWinName(id, volume, type, closeType) + getHoldCondition(instrumet);
        ipcRenderer.send('change-title', {id, title});
    },
    mouseTrade(){
      if(!this.showbar || !this.chart.data.length) return;
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
    putOrder(limitPrice, direction, volumeTotalOriginal = this.config.volume){
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
          ipcRenderer.send('trade', {limitPrice, instrumentID, direction: traderData.direction, volumeTotalOriginal:traderData.volumeTotalOriginal[0],combOffsetFlag: '1', ExchangeID: this.exchangeId})
        }
         
        ipcRenderer.send('trade', {limitPrice, instrumentID, direction: traderData.direction, volumeTotalOriginal: traderData.volumeTotalOriginal[1],combOffsetFlag: '3', ExchangeID: this.exchangeId})

      }else{
        ipcRenderer.send('trade', {limitPrice, instrumentID, ...traderData, ExchangeID: this.exchangeId})
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
    }
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
  }
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
</style>