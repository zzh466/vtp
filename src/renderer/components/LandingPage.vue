<template>
  <div id="wrapper" v-loading='loading.length'  element-loading-text="正在获取账号信息">
    <main>
      <div class="left-side">
        <div v-for="(value, key) in orders" :key="key">{{value.InstrumentID}}{{value.OrderStatus}}</div>
      </div>

      <div class="right-side">
        <div class="doc">
          <div class="title">Getting Started</div>
            <p v-for='ins in InstrumentIDs' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
             <p v-for='ins in ids' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
               <p v-for='ins in gz' :style="{backgroundColor: activeIns===ins?'pink':''}" @click="start(ins)" :key="ins">{{ins}}</p>
          <button @click="open">商品</button>
          <button @click="open1">郑商所</button>
           <button @click="open2">股指</button>
           <button @click="stop">停止</button>
          <br><br>
        </div>
        <div class="doc">
          <div class="title alt">Other Documentation</div>
          <button class="alt" @click="open('https://electron.atom.io/docs/')">Electron</button>
          <button class="alt" @click="open('https://vuejs.org/v2/guide/')">Vue.js</button>
        </div>
      </div>
    </main>
  </div>
</template>

<script>
  import SystemInformation from './LandingPage/SystemInformation'
  import { ipcRenderer } from 'electron';
  import request from '../utils/request';
  import {getWinName} from '../utils/utils'
  export default {
    name: 'landing-page',
    components: { SystemInformation },
    computed: {
      priceData(){
        return this.$store.state.PriceData.priceData;
      },
      InstrumentIDs (){
        return this.$store.state.PriceData.InstrumentIDs
      },
      activeIns() {
         return this.$store.state.PriceData.activeIns
      },

    },
    data(){
      return {
        ids: ['AP201','AP203','SM205','SM201'],
        gz:['IC2112','IF2112','IC2111','IF2111','IH2111'],
        orders: {},
        loading: ['order', 'trade'],
        orderColumns: [{
          label: '日期',
          prop: '',
        },{
          label: ''
        }]
      }
    },
    mounted(){
      request({
        url: 'user/config',
        type: 'GET'
      }).then(res => {
       
        this.finishLoading('config')
      });
      ipcRenderer.send('trade-login', {});
      ipcRenderer.on('receive-order', (event, orders) =>{
        this.orders = orders;
      });
      ipcRenderer.on('finish-loading', (event, arg) =>{
        console.log(arg)
        this.finishLoading(arg);
      });
      
    },
    methods: {
      start(ins) {
        ipcRenderer.send('open-window', {id:ins, title: getWinName(ins)});
        this.$store.dispatch('updateIns', ins);
      },
      open () {
         ipcRenderer.send('start-receive', {host: '101.132.114.246', port: 18199, instrumentIDs: this.InstrumentIDs, type: 'SP',  iCmdID: 101});
      },
      open1(){
        ipcRenderer.send('start-receive', {host: '101.132.114.246', port: 19188, instrumentIDs: this.ids, type: 'SP', iCmdID: 101});
      },
      open2(){
        ipcRenderer.send('start-receive', {host: '117.185.41.78', port: 18198, instrumentIDs: this.gz, type: 'SP', iCmdID: 101});
      },
      stop(){
        ipcRenderer.send('stop-subscrible');
        this.$store.dispatch('updateIns', '')
      },
      finishLoading(tag){
        const index = this.loading.indexOf(tag);
        if(index > -1) {
          this.loading.splice(tag, 1)
        }
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
    background:
      radial-gradient(
        ellipse at top left,
        rgba(255, 255, 255, 1) 40%,
        rgba(229, 229, 229, .9) 100%
      );
    height: 100vh;
    padding: 60px 80px;
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

  main > div { flex-basis: 50%; }

  .left-side {
    display: flex;
    flex-direction: column;
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
</style>
