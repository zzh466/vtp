<template>
  <div class="price-body"> 
    <canvas id="can" :width="width + 'px'" :height="height + 'px'"></canvas>
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';
import Chart from './chart'


export default {
  mounted(){
      const chartDom = document.getElementById('can');
      this.chart = new Chart(chartDom, this.width, this.height, 0.2)
      console.log(this.$route.query.id)
      ipcRenderer.send('register-event', this.$route.query.id);
      ipcRenderer.on(`receive-${this.$route.query.id}`, (event, arg) => {
       
        this.chart.render(arg)
      })
     
      window.onresize =function(e){
        console.log(e)
      }
      let i = 0
      // setInterval(()=> {
      //    this.chart.render({
      //    LastPrice: 3990,
      //   [`BidPrice1`]: 3989.6,
      //   [`BidPrice2`]:3989.2,
      //   [`BidPrice3`]:3989.0,
      //   [`BidPrice4`]:3988.8,
      //   [`BidPrice5`]:3988.6 - i*0.4,
      //   [`BidVolume1`]: 80,
      //   [`BidVolume2`]: 10,
      //   [`BidVolume3`]: 21,
      //   [`BidVolume4`]: 1,
      //   [`BidVolume5`]: 3,
      //   [`AskPrice1`]: 3990.2,
      //   [`AskPrice2`]:  3990.4,
      //   [`AskPrice3`]: 3990.8,
      //   [`AskPrice4`]: 3991.2,
      //   [`AskPrice5`]:  3991.4,
      //   [`AskVolume1`]: 5,
      //   [`AskVolume2`]: 30,
      //   [`AskVolume3`]: 20,
      //   [`AskVolume4`]: 4, 
         
      //   [`AskVolume5`]: 2
      //  });
      //  i++
      // }, 500)
  },
  data () {
    return {
      BidPrice: 0,
      width: 1200,
      height: 300
    }
  },
  methods: {
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
}
</style>