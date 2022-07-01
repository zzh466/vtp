<template>
     <el-form ref="form" :model="form" label-width="80px" :rules="rules">
      <el-form-item label="用户名" prop="userNm">
        <el-input v-model="form.userNm" :disabled='!!userAccount'></el-input>
      </el-form-item>
       <el-form-item label="密码" prop="userPwd">
        <el-input  type="password"  @keydown.enter.native='login' v-model="form.userPwd"></el-input>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="login" :disabled='disabled'>登录</el-button>
      </el-form-item>
   </el-form>
</template>

<script>
 import { getIPAdress, hostname, version, getMac} from '../../utils/utils';
import request from '../../utils/request';
  import { ipcRenderer } from 'electron';
export default {
    props: ['userAccount'],
    data() {
        return {
            form: {
                userNm: this.userAccount || '',
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
            disabled: false,
        }
    },
    methods : {
        login(){
             if( this.disabled){
                 return
             }
            this.disabled = true
            this.$refs.form.validate((validate) => {
           
            if(validate){
                 
                 const data = {
                    appVersion: version,
                    ip: getIPAdress(),
                    hostNm: hostname,
                    userMAC: getMac(),
                    ...this.form};
                ipcRenderer.send('info-log', `login  ${JSON.stringify(data)}`)
                request({
                url: 'access/loginClient', 
                method: 'POST',
                data
                }).then((res) => {
                this.disabled =  false
                if(res.code === 'REQ_SUCCESS'){
                    
                    this.$emit('login', res)
                    
                
                }else{
                    this.$message.error(res.msg || '登陆失败');
                   
                }
                })
            }else{
                this.disabled =  false
            }
            })
        }
    }
}
</script>
