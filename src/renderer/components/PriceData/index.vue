<template>
  <div class="price-body"  @dblclick="mouseTrade"> 
    <canvas @mousemove="move" id="can" :width="width + 'px'" :height="height + 'px'"></canvas>
    <div  class="price-tick" v-show="showbar" :style="{ width: '15px', left: this.left + 'px' }"></div>
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';
import Chart from './chart'


export default {
  mounted(){
      const chartDom = document.getElementById('can');
      
      console.log(this.$route.query.id);
      const id = this.$route.query.id;
      ipcRenderer.send('register-event', id);
    
     
      window.onkeydown =function(e){
        console.log(e)
      }
      ipcRenderer.invoke('get-pirceTick', id).then(tick => {
        console.log(tick)
        this.chart = new Chart(chartDom, this.width, this.height,tick, {width: this.stepwidth});
        ipcRenderer.on(`receive-${id}`, (event, arg) => {
          this.chart.render(arg)
        })
      })

  },
  data () {
    return {
      BidPrice: 0,
      width: 1400,
      height: 300,
      showbar: false,
      left: 0,
      stepwidth: 15,
      volume: 1,
      
    }
  },
  methods: {
    move(e){
      const {x ,y} = e;
  
      if(x > 104 && y > 54){
        this.showbar = true;
        this.left = x - (x-105)%this.stepwidth;
      }else {
        this.showbar = false
      }
    },
    mouseTrade(){
      if(!this.showbar) return;
      const index = (this.left - 105) / this.stepwidth;
      let {buyIndex, askIndex, start} = this.chart;
      buyIndex = buyIndex - start;
      askIndex = askIndex -start
      if(index >buyIndex  && index < askIndex) return;
      let direction = '1';
      if(index <= buyIndex){
        direction = '0'
      }
      const  limitPrice = +this.chart.data[index + start].price;
      const volumeTotalOriginal = this.volume;
      const instrumentID = this.$route.query.id;
      const combOffsetFlag = '0'
      console.log('tarde')
      ipcRenderer.send('trade', {limitPrice, direction, volumeTotalOriginal, instrumentID, combOffsetFlag})

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
</style>