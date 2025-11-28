<template>
  <div class="login-container " :style="step === 2? 'padding: 70px 20px;': ''">
   <loginform @login='login' v-if='step === 1'>
      
    </loginform>
   
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
      async login(data){
          const thrRealProfit = await ipcRenderer.invoke('get-config', 'thrRealProfit');
          const config = await ipcRenderer.invoke('get-config', 'config')
          this.$store.commit('setstate', {
              key: 'userData',
              data: {
                  futureAccountVOList: {
                     futureUserId: data.userNm,
                    futureUserPwd: data.userPwd,
                    ...data

                  },
                  thrRealProfit,
                  instrumentConfigVOList: config,
                  id: 88
              }
          })
           
            ipcRenderer.send('resize-main',  {width: 1600, height: 770});
            this.$router.replace('main');
          
              
             
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