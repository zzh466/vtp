<template>
     <el-form ref="form" :model="form" label-width="80px" :rules="rules">
      <el-form-item label="用户名" prop="userNm">
        <el-input v-model="form.userNm" :disabled='!!userAccount'></el-input>
      </el-form-item>
       <el-form-item label="密码" prop="userPwd">
        <el-input  type="password"  @keydown.enter.native='login' v-model="form.userPwd"></el-input>
      </el-form-item>
      <div v-show="showhidden">
          <el-form-item v-for="value in hiddenArr" :label="value.label" :key= "value.key" :prop= "value.key">
            <el-input v-model="form[value.key]" ></el-input>
        </el-form-item>
      </div>
      <el-form-item>
        <el-button type="primary" @click="login" :disabled='disabled'>登录</el-button>
      </el-form-item>
   </el-form>
</template>

<script>
 import { getIPAdress, hostname, version, getMac} from '../../utils/utils';
import request from '../../utils/request';
  import { ipcRenderer } from 'electron';
const hiddenArr= [{
    key: 'authcode',
    label: 'authcode'
},{
    key: 'brokeId',
    label: 'brokeId'
},{
    key: 'appId',
    label: 'appId'
},{
    key: 'tradeAddress',
    label: '交易地址'
},{
    key: 'quotAddress',
    label: '行情地址'
}]
export default {
    props: ['userAccount'],
    data() {
         ipcRenderer.invoke('get-config', 'login-config').then(e => {
          
           hiddenArr.forEach(item =>{
             this.form[item.key] = e[item.key]
           })
         })
        return {
            showhidden: false,
            form: {
                userNm: this.userAccount || '',
                userPwd: '',
                authcode: '',
                appId: '',
                tradeAddress: '',
                quotAddress: '',
                brokeId: ''
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
            hiddenArr
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
                 
               
                    
                this.$emit('login', res)
                ipcRenderer.send('set-config', 'login-config', res)
                
               
                }else{
                    this.disabled =  false
                }
            })
        }
    }
}
</script>
