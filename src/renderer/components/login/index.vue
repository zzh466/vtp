<template>
  <div class="login-container " :style="step === 2? 'padding: 70px 20px;': ''">
   <el-form ref="form" :model="form" label-width="80px" :rules="rules" v-if='step === 1'>
      <el-form-item label="用户名" prop="userNm">
        <el-input v-model="form.userNm"></el-input>
      </el-form-item>
       <el-form-item label="密码" prop="userPwd">
        <el-input  type="password" @keydown.enter.native='login' v-model="form.userPwd"></el-input>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="login" :disabled='disabled'>登录</el-button>
      </el-form-item>
   </el-form>
  
      <el-form label-width="160px" :rules="rules" v-if='step === 2'>
      <el-form-item label="选择交易账户" prop="userNm">
        <el-radio-group v-model="active">
          <el-radio v-for='account,index in accountList' :label="account.id" :key='account.id' >账户{{index+1}}</el-radio>
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
  import request from '../../utils/request';
  import { getIPAdress, hostname, version} from '../../utils/utils';
  export default {
    data () {
      // if (process.env.NODE_ENV === 'development'){
      //    this.$router.push('main');
      //    ipcRenderer.send('resize-main', {width: 1320, height: 840});
      // }
      return {
        form: {
          userNm: '',
          userPwd: ''
        },
        rules: {
          userNm: [
            { required: true, message: '请输入用户名', trigger: 'blur' },
          ],
          userPwd: [
            { required: true, message: '请输入密码', trigger: 'blur' },
          ],
        },
        step: 1,
        accountList: [],
        disabled: false,
        active: 0
      }
    },
    methods: {
      login(){
        this.disabled = true
        this.$refs.form.validate((validate) => {
          if(validate){
            request({
              url: 'access/loginClient', 
              method: 'POST',
              data: {
                appVersion: version,
                ip: getIPAdress(),
                hostNm: hostname,
                ...this.form},
            }).then((res) => {
              if(res.code === 'REQ_SUCCESS'){
                
                
                this.$store.commit('setstate', {
                    key: 'userData',
                    data:{account: this.form.userNm, ...res}
                })
                const { futureAccountVOList} = res;
                if(futureAccountVOList.length > 1){
                  this.accountList = futureAccountVOList;
                  this.active = futureAccountVOList[0].id;
                  this.step = 2
                }else{
                  ipcRenderer.send('resize-main',  {width: 1600, height: 770});
                    this.$router.push('main');
                }
              
              }else{
                this.$message.error(res.msg || '登陆失败');
                this.disabled =  false
              }
            })
          }
        })
      },
      cofirm(){
        this.$store.commit('setstate', {
              key: 'activeCtpaccount',
              data:this.active
          })
        ipcRenderer.send('resize-main',  {width: 1600, height: 770});
        this.$router.push('main');
      }
    }
  }
</script>
<style >
  .login-container {
    padding: 70px;
  }
</style>