<template>
  <div class="login-container ">
   <el-form ref="form" :model="form" label-width="80px" :rules="rules" >
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
  </div>
</template>

<script>

  import { ipcRenderer } from 'electron';
  import request from '../../utils/request';
  export default {
    data () {
      if (process.env.NODE_ENV === 'development'){
         this.$router.push('main');
      }
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
        disabled: false
      }
    },
    methods: {
      login(){
        this.disabled = true
        this.$refs.form.validate((validate) => {
          if(validate){
            request({
              url: 'access/login', 
              method: 'POST',
              data: this.form,
            }).then((res) => {
              if(res.code === 'REQ_SUCCESS'){
                ipcRenderer.send('resize-main', {width: 1400, height: 500});
                this.$router.push('main');
              }else{
                this.$message.error(res.msg || '登陆失败');
                this.disabled =  false
              }
            })
          }
        })
        
      }
    }
  }
</script>
<style >
  .login-container {
    padding: 40px;
  }
</style>