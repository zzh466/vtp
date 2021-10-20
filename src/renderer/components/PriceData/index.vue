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
      
      console.log(this.$route.query.id);
      const id = this.$route.query.id;
      ipcRenderer.send('register-event', id);
    
     
      window.onkeydown =function(e){
        console.log(e)
      }
      ipcRenderer.invoke('get-pirceTick', id).then(tick => {
        this.chart = new Chart(chartDom, this.width, this.height, tick);
        ipcRenderer.on(`receive-${id}`, (event, arg) => {
          this.chart.render(arg)
        })
      })

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