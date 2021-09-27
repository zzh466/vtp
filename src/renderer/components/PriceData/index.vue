<template>
  <div id='echart-container'> 
     {{BidPrice}}
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';
import * as echarts from 'echarts';
let myChart;
let init = false;
export default {
  mounted(){
      const chartDom = document.getElementById('echart-container');
      myChart = echarts.init(chartDom);
      console.log(this.$route.query.id)
      ipcRenderer.send('register-event', this.$route.query.id);
      ipcRenderer.on(`receive-${this.$route.query.id}`, (event, arg) => {
        if(!init){
          this.init(arg)
          init = true;
        }else {
          this.setOptions(arg)
        }
        this.BidPrice = arg.LastPrice
      })
  },
  data () {
    return {
      BidPrice: 0
    }
  },
  methods: {
    init(data){
      const {LastPrice} = data;
      const xdata = this.intiXAxis(LastPrice, 0.5);
      const seriesData = this.initData(data);
      const option = {
        xAxis: {
          type: 'value',
          data: xdata,
          min: xdata[0],
          max: xdata[xdata.length-1]
        },
        yAxis: {
          type: 'value',
          inverse: true,
          
          max: 50
        },
        series:{
          data: seriesData,
          type: 'bar',
        },
        animation: false
      }
      console.log(option)
      myChart.setOption(option)

    },
    intiXAxis(price, range){
      const data= [price];
      for(let i = 1; i <= 50; i++) {
        data.push(price + i*range);
        data.unshift(price - i*range)
      }
      return data;
    },
    setOptions(arg){
      const data = this.initData(arg);
      myChart.setOption({
        series:{
          data
        }
      })
    },
    initData(data){
      const d = [];
      for(let i = 1; i<=5; i++){
        d.push({
          value:[data[`BidPrice${i}`], data[`BidVolume${i}`]],
          itemStyle: {
            color: 'red'
          }
        });
        d.unshift({
          value:[data[`AskPrice${i}`], data[`AskVolume${i}`]],
           itemStyle: {
            color: 'green'
          }
        });
      }
      return d
    }
  }
}
</script>
<style>
#echart-container {
  height:300px;
}
</style>