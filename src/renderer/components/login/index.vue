<template>
  <div class="login-container " :style="step === 2? 'padding: 70px 20px;': ''">
   <loginform @login='login' v-if='step === 1'>
      
    </loginform>
    
      <el-form label-width="160px"  v-else-if='step === 2'>
      <el-form-item label="选择交易账户" >
        <el-radio-group :value="active" >
          <el-radio v-for='account in accountList' @change="changeActive(account.id)" :label="account.id" :key='account.id' >{{account.futureUserName}}</el-radio>
        </el-radio-group>
      </el-form-item>
     
      <el-form-item>
        <el-button type="primary" @click="cofirm">确认</el-button>
      </el-form-item>
   </el-form>
    <!-- <el-checkbox v-if='step === 1' v-model='datachecked'> 行情监测</el-checkbox> -->
      <!-- <el-checkbox v-if='step === 1' v-model='checked'> 历史行情交易</el-checkbox> -->
  </div>
</template>

<script>

  import { ipcRenderer } from 'electron';
  
  import request from '../../utils/request';
  import loginform from './form.vue'
  export default {
    components : {
      loginform
    },
    data () {
      // if (process.env.NODE_ENV === 'development'){
      //    this.$router.push('main');
      //    ipcRenderer.send('resize-main', {width: 1320, height: 840});
      // }
      
      return {
       
        step: this.$store.state.user.activeCtpaccount? 2: 1,
        checked: false,
        datachecked: false
        
       
      }
    },
    computed: {
      accountList: function(){
        
        return this.$store.state.user.userData.futureAccountVOList
      },
      active: function(){
        
        return this.$store.state.user.activeCtpaccount
      }
    },
    methods: {
      login(data){
          data = {...data, _datachecked:this.datachecked}
          this.$store.commit('setstate', {
              key: 'userData',
              data
          })
            const { futureAccountVOList} = data;
            if(!futureAccountVOList && !futureAccountVOList.length){
              this.$message.error('当前账户没有绑定期货账户');
              return
            }
            this.changeActive(futureAccountVOList[0].id);
          if(this.checked){
             ipcRenderer.send('resize-main',  {width: 1600, height: 770});
              this.$router.replace('trade');
              return;
          }
        
          if(futureAccountVOList.length > 1){
               
            this.$nextTick(()=>  this.step = 2)
            
          }else{
           
              this.checkactive()
          }
              
             
      },
      checkactive(){
        request({
        url: `monitor/fawsstate/${this.accountId}`, 
        method: 'GET',
        }).then((res) => {
          if(res.code === 'REQ_SUCCESS'){
            if(res.isBusy){
              this.$message.error('当前账户已经登录请联系管理员')
              return
            }
            ipcRenderer.send('resize-main',  {width: 1600, height: 770});
            this.$router.replace('main');
          }
          
        })
       
      },
      cofirm(){
        this.checkactive()
      },
      changeActive(value){
        this.accountId = value;
        this.$store.commit('setstate', {
              key: 'activeCtpaccount',
              data:value
          })
      }
    }
  }
</script>
<style >
  .login-container {
    padding: 70px;
  }
</style>