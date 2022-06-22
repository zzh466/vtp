<template>
    <div id="wrapper" v-loading='loading'  
     >
    <el-descriptions size= 'samll' direction="vertical" :column="10" border class="account">
      <el-descriptions-item label="账号">{{userData.userAccount}}</el-descriptions-item>
         <el-descriptions-item label="交易日">{{tradingDay}}</el-descriptions-item>
    
   
    
      <el-descriptions-item label="手续费">{{commission}}</el-descriptions-item>
    
       <el-descriptions-item label="总实际盈亏">{{totalProfit.toFixed(2)}}</el-descriptions-item>
        <!-- <el-descriptions-item v-if="userData.futureAccountVOList.length > 1"><el-button type="primary" size="small" @click="changeAccount">切换账号</el-button></el-descriptions-item> -->
    </el-descriptions>
      
    <main >
      
      <div class="left-side">
        
         <div>
           <div class="label">回合信息：
              <el-button type="primary" size="small" @click="exportroud">导出</el-button>
              </div>
          <Round ref="round" :data='tradeData' :rates='rates'  :price='price' :instrumentInfo='instrumentInfo' :positions="positions"></Round>
        </div>
         
      </div>

      <div class="right-side">
        
       
        <div>
          <div class="label">下单信息：
            <el-button type="primary" size="small" @click="exportorder">导出</el-button>
            </div>
            <Order ref="order" :traders="tradeData"  :tableData='orderData'/>
        </div>
      </div>
     
    </main>
    <!-- <el-button type="primary" @click="updateConfig">更新配置</el-button> -->
      <template v-if='showButton'>
      <el-button type="primary" @click='visible=true'>打开合约</el-button>
      <el-button type="primary" @click='topicvisible=true'>选择主题</el-button></template>
       <div class="label">选择主题合约： </div>
      <div style="display: flex;">
         
      
          <Table key ='ins'  height='300' @row-dblclick='start' :tableData='subscribelInstruments' :columns= 'instrumentsColumns'/>
          <!-- <el-button @click="open">商品</el-button>
              <el-button @click="open1">郑商所</el-button>  .
              .
              <el-button @click="open2">股指</el-button>  -->
              <!-- <el-button @click="forceClose">强平</el-button> -->
      </div>
      <el-dialog :visible.sync='visible'>
          <el-form ref="form" :model="subscribelData" label-width="80px">
           
          <el-form-item label='合约' prop='instrumentId' :rules='[{ required: true, message: `请选择合约`,trigger: "change"}]'>
            <el-input v-model="subscribelData.instrumentId" />
                  
        </el-form-item>
        <el-form-item label='配置' prop='configId' :rules='[{ required: true, message: `请选择配置`,trigger: "change"}]'>
            <el-select v-model="subscribelData.configId">
                <el-option v-for='e,index in subscribelInstruments' :value="e.configId" :key="e.configId" :label='"配置" + (index + 1) '></el-option>
              
                
            </el-select>
        </el-form-item>
       <el-form-item label='时间' prop='time' :rules='[{ required: true, message: `请选择合约`,trigger: "change"}]'>
            <el-date-picker
              v-model="subscribelData.time"
              type="datetime"
              placeholder="选择日期时间"
               value-format='yyyyMMddHHmmss'
              >
             
            </el-date-picker>
        </el-form-item>
    </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click='visible=false'>取 消</el-button>
        <el-button type="primary" @click="confirm">确 定</el-button>
    </div>
      </el-dialog>
      <el-dialog :visible.sync='topicvisible'>
        <el-form ref="form1" :model="topicData" label-width="80px">
           <el-form-item label='主题' prop='topic' :rules='[{ required: true, message: `请选择主题`,trigger: "change"}]'>
            <el-select v-model="topicData.topic">
                <el-option v-for='e in topics' :value="e.topic" :key="e.topic" :label='e.topic'></el-option>
              
                
            </el-select>
        </el-form-item>
        
    </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click='topicvisible=false'>取 消</el-button>
        <el-button type="primary" @click="confirmTopic">确 定</el-button>
    </div>
      </el-dialog>
  </div>
</template>
<script>
import {ipcRenderer} from 'electron';
import request  from '../utils/request';
  import {getWinName, getyyyyMMdd, getClientSize} from '../utils/utils';
import Round from './LandingPage/round.vue';
   import Order from './LandingPage/Order.vue';
export default {
    components: {  Round , Order},
    watch: {
      topicvisible(val){
        if(val){
          request({
            url: 'quot/topicquot'}).then(({list}) => {
               this.topics = list;
            })
           
        }
      }
    },
    data(){
        return {
            loading: true,
            instrumentsColumns: [{
              label: '合约',
              prop: 'ins',
            }],
            orderData: [],
            tradeData: [],
            rates: [],
            price: {},
            instrumentInfo: [],
            tradingDay:  getyyyyMMdd(),
            commission: 0,
            totalProfit: 0,
            positions: [],
            visible: false,
             topicvisible: false,
            subscribelData: {
              instrumentId: '',
              time: '',
              configId: ''
            },
            topicData:{
              topic: ''
            },
            topics: [],
            showButton: true,
            subscribelInstruments: []
        }
        
    },
    created(){
       
        this.init().then(([{instrumentList}, {commissionRateList}])=>{
            this.instrumentInfo = instrumentList;
            this.rates = commissionRateList.sort((a, b) => b.InstrumentID.length - a.InstrumentID.length);
            this.loading = false;
            const storageKey= `fake-trade-${this.userData.id}`;
            ipcRenderer.send('set-account', this.userData.id);
            let history = localStorage.getItem(storageKey);
            
            if(history){
              history = JSON.parse(history)
              const {date, tradeData, orderData} = history;
              const now = new Date().toLocaleDateString();
             
              if(date === now){
                
                this.tradeData = tradeData|| [];
                this.orderData = orderData|| [];
                this.$nextTick(function(){
                   this.$refs.round.init();
                })
              }else{
                localStorage.setItem(storageKey, '')
              }
            }
          
            ipcRenderer.send('fake-trade', {id: this.userData.id, orderData: this.orderData, tradeData: this.tradeData});
            
        })
        ipcRenderer.on('receive-order', (evnet, order) => {
          const index= this.orderData.findIndex(e => e.key === order.key)
          if(index > -1){
            this.orderData.splice(index, 1, order)
          }else{
             this.orderData.unshift(order)
          }
           this.setStroge()
        })
        ipcRenderer.on('receive-trade', (evnet, trade) => {
          console.log(trade)
           this.$refs.round.update(trade);
          this.tradeData.unshift(trade)
          this.setStroge()
        })
        ipcRenderer.on('receive-price', (evnet, price) => {
          this.price = price
          this.$nextTick(function(){
            const {commission, total} = this.$refs.round.traderData.reduce((a,b) => {
              a.commission = a.commission + parseFloat(b.commission);
              a.total = a.total + b.closeProfit + b.optionProfit - b.commission;
              return a
            }, {commission:0, total: 0});
            this.commission=commission.toFixed(2);
            this.totalProfit = total;
          })
        })

    },
    computed: {
     
      userData() {
         return this.$store.state.user.userData
      },
     
    },
    methods: {
        exportroud(){
        const {traderColumns, traderData} =  this.$refs.round
        this.exportExcel('模拟交易回合信息', traderData, traderColumns)
      },
       exportorder(){
         const {orderColumns} =  this.$refs.order
        this.exportExcel('模拟交易下单信息', this.orderData, orderColumns)
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
      setStroge(){
        const storageKey= `fake-trade-${this.userData.id}`;
        const now = new Date().toLocaleDateString();
        const data = {
          date: now,
          tradeData: this.tradeData,
          orderData: this.orderData
        }
        localStorage.setItem(storageKey, JSON.stringify(data));
      },
       init(){
        return Promise.all([request({
            url: '/ctp/instrument',
            
          }),request({
            url: '/ctp/instrumentCommissionRate',
            
          }),
          this.$store.dispatch('get-config')])
      },
      start({row}){
        
         const {ins} = row;
        const {width, height} = getClientSize()
        const info = this.instrumentInfo.find(e => e.InstrumentID.match(/^[a-zA-Z]+/)[0] === ins.match(/^[a-zA-Z]+/)[0]);
        this.instrumentInfo.push({...info, InstrumentID: ins})
        if(!info){
           this.$alert('改合约尚未订阅行情，请联系管理员订阅！');
           return;
        };
         const {PriceTick, ExchangeID} = info; 
        const active = this.$store.state.user.activeCtpaccount;
        const account = this.userData.futureAccountVOList.find(e => e.id === active);
        const configId = this.userData.instrumentConfigVOList.find(e => e.instruments.includes(ins.match(/^[a-zA-Z]+/)[0])).id
        const accountIndex = account.futureUserName;
        ipcRenderer.send('open-window', {id:ins, title: getWinName(ins, accountIndex), account: this.userData.id, width, height, tick: PriceTick, exchangeId: ExchangeID, checked: true,configId, accountIndex});
      },
      confirmTopic(){
        this.$refs.form1.validate((valid) => {
          if(valid){
             
           
            const {topic} = this.topicData;
            ipcRenderer.send('send-fake-trade-msg',`NotifyGroupQuotDataHist@${topic}`)
            
            const e = this.topics.find(e => e.topic === topic);
            
            this.subscribelInstruments = e.instrumentIDList.map(e => ({ins: e}))    ;
            this.showButton = false     
             this.topicvisible = false; 
          }
         })
      },
      confirm(){
        this.$refs.form.validate((valid) => {
          if(valid){
            const { instrumentId , time , configId} = this.subscribelData;
            if(this.opened && this.opened!== instrumentId){
              ipcRenderer.send('close-all-sub');
              ipcRenderer.send('send-fake-trade-msg',`NotifyQuotDataHistCancel`)
            }
            ipcRenderer.send('send-fake-trade-msg',`NotifyQuotDataHist@${instrumentId}:${time}`)
              const active = this.$store.state.user.activeCtpaccount;
            const account = this.userData.futureAccountVOList.find(e => e.id === active);
              const {width, height} = getClientSize()
                const info = this.instrumentInfo.find(e => e.InstrumentID.match(/^[a-zA-Z]+/)[0] === instrumentId.match(/^[a-zA-Z]+/)[0]);
                const {PriceTick, ExchangeID} = info; 
            const accountIndex = account.futureUserName;
            ipcRenderer.send('open-window', {id:instrumentId, title: getWinName(instrumentId, accountIndex), account: this.userData.id, width, height, tick: PriceTick, exchangeId: ExchangeID, checked: true,configId, accountIndex, showController : true});
            this.visible = false;
            this.opened = instrumentId
            this.showButton = false;
          }
        })
      }
    }
}
</script>
