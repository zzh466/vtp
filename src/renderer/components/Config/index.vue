<template>
  <div class="config-content" v-loading='loading'>
    <div style="margin: 0px 15px">
    <!-- <el-tabs  @tab-click="change"  type="card" >
      <el-tab-pane :label="`配置${index+1}`" v-for='key,index in configs' :key='key.id' :prop="key.id.toString()"></el-tab-pane>
      
    </el-tabs> -->
     <Table :columns='columns' :tableData='hotkey' @config-action='edit' :height='400' row-key='key'/>
     <el-button style="margin-top: 5px" type='primary' @click='edit({type: "add"})'> 新增快捷键</el-button>
    </div>
    <div style="margin: 20px 0">
      <div style="margin: 0 5px">
      
        <canvas style="background-color: #000;position: relative;" id="preview" :width="width + 'px'" :height="height + 'px'"></canvas>
        <el-button type="primary" @click="preview">预览</el-button>
      </div>
     
      <el-form ref="form" :model="config" label-width="180px">
        <el-form-item v-for='item in formItem' :label="item.name" :key='item.key' :rules="[
            { required: true, message: `请输入${item.name}`, trigger: 'blur' }]">
          <el-input v-model="config[item.key]" type="number"></el-input>
        </el-form-item>
        <el-form-item label='挂单数量刻度类型' prop='volumeScaleType'  :rules='[{ required: true, message: `请选择挂单数量刻度类型`}]'>
              <el-select  v-model='config.volumeScaleType'>
              <el-option :value='0' label="线性无上限"></el-option>
              <el-option :value='1' label="线性有上限"> </el-option>
              <el-option :value='2'  label="非线性无上限"></el-option>
              <el-option :value='3' label="非线性有上限"></el-option>
              </el-select> 
          </el-form-item>
          <el-form-item label='订阅合约' prop='instruments'  :rules='[{ required: true, message: `请选择订阅合约`}]'>
            <!-- <el-transfer filterable :titles= "['全部合约', '已订阅合约']" v-model="config.instruments" :data="subsInstruments"></el-transfer> -->
            <el-input v-model="config.instruments" ></el-input>
          </el-form-item>
          <!-- <el-form-item label='行情提醒合约' prop='subscribeIndicator' >
              <el-select  multiple   filterable v-model="config.subscribeIndicator">
               <el-option v-for="ins in subsInstruments" :key="ins.key"  :value='ins.key' :label="ins.key"></el-option>
              </el-select> 
          </el-form-item> -->
        <el-form-item label='默认平仓方式' prop='sysCloseType'  :rules='[{ required: true, message: `请选择默认平仓方式`}]'>
            <el-select v-model='config.sysCloseType'>
            <el-option :value='0' label="一键全平"></el-option>
            <el-option :value='1' label="按指定手数平">  </el-option>
            </el-select> 
          </el-form-item>
          <el-form-item label='默认平今策略' prop='sysCloseTStrategy'  :rules='[{ required: true, message: `请选择默认平今策略`}]'>
              <el-select  v-model='config.sysCloseTStrategy'>
              <el-option :value='0' label="锁仓"></el-option>
              <el-option :value='1' label="平今"></el-option>
              <el-option :value='2' label="开今"></el-option>
              <el-option :value='3' label="平仓"></el-option>
              </el-select> 
          </el-form-item>
        
          <!-- <el-form-item label='柱子参数类型' prop='calcBarType'  :rules='[{ required: true, message: `请选择挂柱子参数类型`}]'>
              <el-select  v-model='config.calcBarType'>
              <el-option :value='0' label="自动适应"></el-option>
              <el-option :value='1' label="以数量计算">  </el-option>
              <el-option :value='2'  label="以宽度计算"></el-option>
              </el-select> 
          </el-form-item> -->
        
          <!-- <el-form-item v-if="groupId !== 12" label='是否展示他人持仓' prop='broadcastOpenInterest'  :rules='[{ required: true}]'>
            <el-radio-group  v-model='config.broadcastOpenInterest' >
              <el-radio :label='true'>是</el-radio>
              <el-radio :label='false'>否</el-radio>
            </el-radio-group>
          </el-form-item> -->
          <el-form-item  label='默认置顶' prop='topQuot'   :rules='[{ required: true}]'>
            <el-radio-group  v-model='config.topQuot' >
              <el-radio :label='true'>是</el-radio>
              <el-radio :label='false'>否</el-radio>
            </el-radio-group>
          </el-form-item>
          <!-- <el-form-item  label='开盘前一分钟提醒' prop='windowsOpenCd'   :rules='[{ required: true}]'>
            <el-radio-group  v-model='config.windowsOpenCd' >
              <el-radio :label='true'>是</el-radio>
              <el-radio :label='false'>否</el-radio>
            </el-radio-group>
          </el-form-item>
          <el-form-item  label='收盘盘前一分钟提醒' prop='windowsCloseCd'   :rules='[{ required: true}]'>
            <el-radio-group  v-model='config.windowsCloseCd' >
              <el-radio :label='true'>是</el-radio>
              <el-radio :label='false'>否</el-radio>
            </el-radio-group>
          </el-form-item> -->
       </el-form>
      
    </div>
 
      <hotkey-edit  :vlomeaction='vlomeaction' :visible.sync="showModal" @update-form='update'
        :actions='actions' :editItem='editItem' :orderaction='orderaction' :typeMap='typeMap'/>
   

     <div class="config-foot">
          <el-button type="primary" @click="onSubmit">保存</el-button>
        </div>
   </div>
</template>

<script>

  import { ipcRenderer } from 'electron';
  import './action'
  import Edit from'./edit'
  import request from '../../utils/request';
  import Chart from '../PriceData/chart'
  import {closeTypeMap, typeMap } from '../../utils/utils';

  const actions = ['下单','先撤再下','当前交易所全部撤单','修改手数','修改平仓方式','切换平今策略','设置平今策略','全交易所全部撤单','当前合约全部撤单','当前合约撤最近一手报单' ,'撤最近一手报单' ,'当前合约全部平仓', '市价下单'];
  const orderaction = ['0', '1', '12']
  const vlomeaction = ['3']
  const formItem = [{
    name: '柱子滚动最小刻度',
    key: 'barToBorder',
  },{
    name: '柱子宽度',
    key: 'barWidth',
   
  },{
    name: '挂单量数字横向偏移',
    key: 'volumeXOffset',
   
  },{
    name: '挂单量数字纵向偏移',
    key: 'volumeYOffset',
    extra: '正值为买单量向下偏移，负值为卖单量向下偏移'
  },{
    name: '挂单数量刻度高度',
    key: 'volumeScaleHeight',
    extra: '无上限类型使用'
  },{
    name: '挂单数量刻度数量',
    key: 'volumeScaleCount',
    extra: '有上限类型使用'
  },{
    name: '挂单数量刻度间隔值',
    key: 'volumeScaleTick',
    extra: '线性类型使用'
  },{
    name: '默认手数',
    key: 'sysOrderVolume',
   
  },]
   const columns =  [{
        label: '快捷键',
        prop: '0',
        key: 'name',
       
      }, {
          label: '动作',
          key: 'action',
          prop: '2',
          width: '150',
          render(a){
            
            if(a[2] === '6'){
                return `设置为${typeMap[a[7]]}模式`;
            }
            return actions[a[2]];
          }
      }, {
          label: '方向',
          key: 'direction',
          prop: '3',
          render(a){
            if(orderaction.includes(a[2])){
                return ['买', '卖'][a[3]];
            }
            return ''
          }
      }, {
        label: '超价',
        key: 'overprice',
        prop: '4',
        render(a, b){
          if(orderaction.includes(a[2])){
              return a[4];
          }
          return ''
        }
    }, {
        label: '手数',
        key: 'volume',
        prop: '5',
        render(a, b){
          if(vlomeaction.includes(a[2])){
              return a[5];
          }
          return ''
        }
    }, {
        label: '操作',
        prop: 'operation',
        key: 'operation',
        width: 200,
        component: 'config-action'
      }]
      const price_ = 2902.00;
    const fakeData = {
          ActionDay: "20211203",
          AskPrice1: price_ + 0.5,
          AskPrice2: price_ + 1.5,
          AskPrice3: price_ + 2,
          AskPrice4: price_ + 2.5,
          AskPrice5: price_ + 3,
          AskVolume1: 12,
          AskVolume2: 3,
          AskVolume3: 2,
          AskVolume4: 3,
          AskVolume5: 2,
          AveragePrice: 0,
          BidPrice1: price_,
          BidPrice2: price_ - 1.5,
          BidPrice3: price_ - 2,
          BidPrice4: price_ - 2.5,
          BidPrice5:price_ - 3,
          BidVolume1: 5,
          BidVolume2: 4,
          BidVolume3: 1,
          BidVolume4: 3,
          BidVolume5: 3,
          ClosePrice: 3,
          CurrDelta: 3,
          ExchangeID: "",
          ExchangeInstID: "",
          HighestPrice: 0,
          InstrumentID: "j2201",
          LastPrice: price_,
          LowerLimitPrice: 2467,
          LowestPrice: 0,
          OpenInterest: 9469,
          OpenPrice: 0,
          PreClosePrice: 2876,
          PreDelta: 0,
          PreOpenInterest: 9469,
          PreSettlementPrice: 2902,
          SettlementPrice: 0,
          TradingDay: "20211203",
          Turnover: 0,
          UpdateMillisec: 0,
          UpdateTime: "18:33:52",
          UpperLimitPrice: 3337,
          Volume: 0
        
      }
  
  
  export default {
    components: {
      'hotkey-edit': Edit
    },
    created(){
      
    
   
      ipcRenderer.invoke('get-config', 'config').then(e =>{

    
        this.loading = false;
        
        // const {vtpUserId} =res2
      
       this.config = e;
        const chartDom = document.getElementById('preview');
       
        // this.groupId = res2.groupId;
      
      
     
        console.log('config', this.config)
        this.chart = new Chart(chartDom,this.width, this.height,0.5, this.config);
        this.chart.render(fakeData)
    
        this.hotkey = this.config.hotkey.split(';').map(e=> e.split(','));
        console.log(this.hotkey)

      })
     

    },
   
      
    
    data () {
     
      return {
          loading: true,
          config: {},
          hotkey: [],
          configs :[],
          columns,
          formItem,
          subsInstruments: [],
          showModal: false,
          editItem: {},
          vlomeaction,
          actions,
          orderaction,
          typeMap,
          groupId: 11,
          width: 980,
          height: 300
      }
    },
    methods: {
      onSubmit(){
       
          const data ={};
          const e= this.config
          for(let key in e){
            if(typeof e[key] === 'string'){
              data[key] = parseInt(e[key])
            }else{
              data[key] =e[key]
            }
          }
          data.hotkey= e.hotkey.map(key=>key.join(',')).join(';');
          data.instruments =e.instruments.filter(e => this.subsInstruments.find(a => a.key === e)).join(',');
        
        
       
         const {id} =this.$route.query;
         console.log(data)
        //  this.loading = true;
        debugger
        ipcRenderer.send('set-config', 'config', data);
        ipcRenderer.send('update-all-config', data);
        //  request({
        //    method: 'PATCH',
        //    url: '/user/config',
        //    data: {
        //      configVOList: config
        //    }
        //  }).then(res=> {
        //     this.loading = false;
        //      if(res.code === 'REQ_SUCCESS'){
        //        this.$message.success('修改成功');
        //       request({
        //         method: 'GET',
        //         url: '/user/info'}).then(res=>{
        //           if(res.code === "REQ_SUCCESS")     
        //            ipcRenderer.send('update-all-config', res.instrumentConfigVOList);
        //           this.preview()
        //         })
                
        //      }else {
        //         this.$message.error(res.msg);
        //      }
        //  })
      
      }, 
      preview(){
       
        ['barToBorder', 'barWidth', 'volumeScaleHeight', 'volumeScaleType', 'volumeScaleCount', 'volumeScaleTick', 'volumeXOffset' , 'volumeYOffset'].forEach(key =>{
        
                console.log(this.config[key])
                this.chart[key] = parseInt(this.config[key])
                
     
          }
        )
        this.chart.ctx.clearRect(0, 0, this.width, this.height);
        this.chart.resize( this.width, this.height);
        this.chart.render(fakeData)
      },
      edit({type, index}){
        switch(type){
          case 'edit':
          case 'add': 
          
            this.showModal = true;
            this.editIndex = index;
            this.editItem = this.hotkey[index] || {};
            break;
          case 'del':
            this.hotkey.splice(index, 1);
           

        }
        console.log(type, index)
      },
      update(arr){
        const code =arr[1]
  
        const key = this.hotkey.some((e,index)=> index !== this.editIndex && e[1]===code);
        if(key){
          this.$alert('当前快捷键已存在');
          return
        }
        this.showModal =false;
        if(this.editIndex !== undefined){
          this.hotkey.splice(this.editIndex, 1, arr)
        }else{
          this.hotkey.unshift(arr);
        }
        
      },
      change(tab) {
        const {index} = tab;
        this.config = this.configs[index];
        this.hotkey = this.configs[index].hotkey;
        this.preview();
      }
    }
  }
</script>
<style >
  .config-content{
    padding: 15px 0px 80px  0px;
    padding-bottom: 50px;
  }
  .config-foot{
    background: #eee;
    position: fixed;
    bottom: 0;
    padding: 5px 50px;
    width: 100%;
    text-align: right;
    border-top: 2px;
  }
</style>