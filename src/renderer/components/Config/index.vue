<template>
  <div class="config-content" v-loading='loading'>
    <div style="margin: 0px 15px">
    <el-tabs  @tab-click="change"  type="card" >
      <el-tab-pane :label="`配置${index+1}`" v-for='key,index in configs' :key='key.id' :prop="key.id.toString()"></el-tab-pane>
      
    </el-tabs>
     <Table :columns='columns' :tableData='hotKey' @config-action='edit' :height='400' row-key='key'/>
     <el-button style="margin-top: 5px" type='primary' @click='edit({type: "add"})'> 新增快捷键</el-button>
    </div>
    <div style="margin: 20px 0">
      <el-form ref="form" :model="config" label-width="180px">
        <el-form-item v-for='item in formItem' :label="item.name" :key='item.key' :rules="[
            { required: true, message: `请输入${item.name}`, trigger: 'blur' }]">
          <el-input v-model="config[item.key]" type="number"></el-input>
        </el-form-item>
          <el-form-item label='订阅合约' prop='instruments'  :rules='[{ required: true, message: `请选择订阅合约`}]'>
            <el-transfer filterable :titles= "['全部合约', '已订阅合约']" v-model="config.instruments" :data="subsInstruments"></el-transfer>
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
          <el-form-item label='挂单数量刻度类型' prop='volumeScaleType'  :rules='[{ required: true, message: `请选择挂单数量刻度类型`}]'>
              <el-select  v-model='config.volumeScaleType'>
              <el-option :value='0' label="线性无上限"></el-option>
              <el-option :value='1' label="线性有上限"> </el-option>
              <el-option :value='2'  label="非线性无上限"></el-option>
              <el-option :value='3' label="非线性有上限"></el-option>
              </el-select> 
          </el-form-item>
          <!-- <el-form-item label='柱子参数类型' prop='calcBarType'  :rules='[{ required: true, message: `请选择挂柱子参数类型`}]'>
              <el-select  v-model='config.calcBarType'>
              <el-option :value='0' label="自动适应"></el-option>
              <el-option :value='1' label="以数量计算">  </el-option>
              <el-option :value='2'  label="以宽度计算"></el-option>
              </el-select> 
          </el-form-item> -->
        
          <el-form-item v-if="groupId !== 12" label='是否展示他人持仓' prop='broadcastOpenInterest'  :rules='[{ required: true}]'>
            <el-radio-group  v-model='config.broadcastOpenInterest' >
              <el-radio :label='true'>是</el-radio>
              <el-radio :label='false'>否</el-radio>
            </el-radio-group>
          </el-form-item>
        
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
    name: '挂单数量刻度数量',
    key: 'volumeScaleCount',
    extra: '有上限类型使用'
  },{
    name: '挂单数量刻度高度',
    key: 'volumeScaleHeight',
    extra: '无上限类型使用'
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
        key: 'name'
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

  export default {
    components: {
      'hotkey-edit': Edit
    },
    created(){
      
    
   
      Promise.all([request({
        url: '/quot/info',
        method: 'GET'
      }), request({
        url: '/user/info',
        method: 'GET'
      }), ]).then(([res1, res2]) => {
        
        this.subsInstruments = res1.quotInfoVOList.reduce((a,b) =>  a.concat(b.instrumentList.map(e => ({key: e, label: e}))), [])
        this.loading = false;
        
        const {vtpUserId} =res2
      
        const config = JSON.parse(localStorage.getItem(`config-${vtpUserId}`));
        this.configs = config.map(e => ({...e, instruments: (e.instruments||'').split(',').filter(e=>e),hotKey: e.hotKey.split(';').filter(e => e).map(key => {
          return key.split(',')
        })})) 
        
        this.groupId = res2.groupId;
        this.config = this.configs[0]
        this.hotKey = this.configs[0].hotKey;

      })
     

    },
    data () {
     
      return {
          loading: true,
          config: {},
          hotKey: [],
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
          groupId: 11
      }
    },
    methods: {
      onSubmit(){
        const config = this.configs.map(e => {
          const data ={};
          for(let key in e){
            if(typeof e[key] === 'string'){
              data[key] = parseInt(e[key])
            }else{
              data[key] =e[key]
            }
          }
          data.hotKey= e.hotKey.map(key=>key.join(',')).join(';');
          data.instruments =e.instruments.filter(e => this.subsInstruments.find(a => a.key === e)).join(',');
        
          return data;
        })
         const {id} =this.$route.query;
         console.log(config)
         this.loading = true;
         request({
           method: 'PATCH',
           url: '/user/config',
           data: {
             configVOList: config
           }
         }).then(res=> {
            this.loading = false;
             if(res.code === 'REQ_SUCCESS'){
               this.$message.success('修改成功');
             
              ipcRenderer.send('update-all-config', config);
             }else {
                this.$message.error(res.msg);
             }
         })
      
      }, 
      edit({type, index}){
        switch(type){
          case 'edit':
          case 'add': 
          
            this.showModal = true;
            this.editIndex = index;
            this.editItem = this.hotKey[index] || {};
            break;
          case 'del':
            this.hotKey.splice(index, 1);
           

        }
        console.log(type, index)
      },
      update(arr){
        const code =arr[1]
  
        const key = this.hotKey.some((e,index)=> index !== this.editIndex && e[1]===code);
        if(key){
          this.$alert('当前快捷键已存在');
          return
        }
        this.showModal =false;
        if(this.editIndex !== undefined){
          this.hotKey.splice(this.editIndex, 1, arr)
        }else{
          this.hotKey.unshift(arr);
        }
        
      },
      change(tab) {
        const {index} = tab;
        this.config = this.configs[index];
        this.hotKey = this.configs[index].hotKey;
      },
      
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