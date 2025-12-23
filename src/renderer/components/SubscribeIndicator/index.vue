<template>
    <div class="config-content" v-loading='loading' >
      <div style="margin: 0px 15px">
        <el-form ref="form" :model="formdata" label-width="120px">
          
                <el-form-item v-for = 'item in list' :key="item.key" :label='item.name' :prop='item.key' :rules='[{ required: true, message: `请输入${item.name}`,trigger: "blur"}, {
                    trigger: "blur",
                    validator,

                }]'>
                    <el-input v-model='formdata[item.key]'></el-input>
                   
                </el-form-item>
               
          
        </el-form> 
      </div>
   
       <div class="config-foot">
           
            <el-button type="primary" @click="onSubmit">保存</el-button>
          </div>
     </div>
  </template>
  
  <script>
  
    import { ipcRenderer } from 'electron';
 
    import request from '../../utils/request';
    import {subscribeIndicatorKey } from '../../utils/utils';
    export default {
        created(){
            
            ipcRenderer.invoke('get-config', 'vtp_client_openvolume_limit').then(e => {
                
                this.formdata.open_limit = e
            } );
             ipcRenderer.invoke('get-config', 'vtp_client_cancelvolume_limit').then(e => this.formdata.close_limit = e);
           
        },
        data () {

            return {
                list : [{
                    name: '开仓限制',
                    key: 'open_limit'
                },{
                    name: '撤单限制',
                    key: 'close_limit'
                }],
                
                loading: false,
                formdata : {
                    open_limit: '',
                    close_limit: ''
                }
            }
        },
        methods: {
            validator(rule, value, callback) {
                if(value < 2){
                    return callback(new Error( "参数不得小于2"));
                }
                callback()
            },
            reset(){
                this.list.forEach(e => {
                    this.formdata[e.indicatorNmEn] = e.defaultValue
                })
            },
            onSubmit(){
                if(this.loading) return
                this.$refs.form.validate((valid) => {
                    let arr = []
                    if(valid){
                         ipcRenderer.send('set-config', 'vtp_client_openvolume_limit', this.formdata.open_limit);
                        ipcRenderer.send('set-config', 'vtp_client_cancelvolume_limit', this.formdata.close_limit);
                        ipcRenderer.send('update-configs', ['vtp_client_openvolume_limit', 'vtp_client_cancelvolume_limit'])
                    }
                })
              
            
            }
            
        }
    }
  </script>
  <style >
.memo{
    color: #606266;
   
}
  </style>