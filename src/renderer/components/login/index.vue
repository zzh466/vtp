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

  </div>
</template>

<script>

  import { ipcRenderer } from 'electron';
  
 
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
          this.$store.commit('setstate', {
              key: 'userData',
              data
          })
          const { futureAccountVOList} = data;
            this.changeActive(futureAccountVOList[0].id);
          if(futureAccountVOList.length > 1){
               
            this.$nextTick(()=>  this.step = 2)
            
          }else{
            ipcRenderer.send('resize-main',  {width: 1600, height: 770});
              this.$router.push('main');
          }
              
             
      },
      cofirm(){
        
        ipcRenderer.send('resize-main',  {width: 1600, height: 770});
        this.$router.push('main');
      },
      changeActive(value){
        console.log(arguments)
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