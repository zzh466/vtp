<template>
    <div class="config-content" v-loading='loading' >
      <div style="margin: 0px 15px">
        <el-form ref="form" :model="formdata" label-width="120px">
          
                <el-form-item v-for = 'item in list' :key="item.indicatorCode" :label='item.indicatorNmCn' :prop='item.indicatorNmEn' :rules='[{ required: true, message: `请输入${item.indicatorNmCn}`,trigger: "blur"}, {
                    trigger: "blur",
                    validator,

                }]'>
                    <el-input v-model='formdata[item.indicatorNmEn]' type="number"></el-input>
                    <p class="memo">{{ item.memo }}</p>
                </el-form-item>
               
          
        </el-form> 
      </div>
   
       <div class="config-foot">
            <el-button  @click="reset">恢复默认</el-button>
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
          
            request({
                url: '/user/indicator',
                method: 'GET'
            }).then(res =>{
                
                if(res.code === 'REQ_SUCCESS'){
                    this.loading = false;
                    this.list = res.indicatorList;
                 
                    this.list.forEach(e => {
                        this.$set(this.formdata, e.indicatorNmEn, e.value || e.defaultValue)
                    })
                }
               
            })
        },
        data () {

            return {
                list : [],
                
                loading: true,
                formdata : {

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
                        for(let key in this.formdata){
                            let item = this.list.find(e => e.indicatorNmEn === key)
                            arr.push({
                                indicatorCode : item.indicatorCode,
                                value: this.formdata[key]
                            })
                        }
                        this.loading= true;
                        request({
                            method: 'PATCH',
                            url: '/user/indicator',
                            data: {
                                indicatorList: arr
                            }
                        }).then(res=> {
                            this.loading = false;
                            if(res.code === 'REQ_SUCCESS'){
                                this.$message.success('修改成功');
                            
                            
                            }else {
                                this.$message.error(res.msg);
                            }
                        })
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