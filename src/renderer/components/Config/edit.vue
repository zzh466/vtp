<template>
    <el-dialog :visible='visible' @close='$emit("update:visible", false)'>
     <el-form ref="form" :model="config" label-width="80px">
            <el-form-item label='快捷键' prop='key' :rules='[{ required: true, message: `请选择快捷键`,trigger: "blur"}]'>
                <el-input :value='config.key' @keydown.native='inputHotkey'></el-input>
            </el-form-item>
            <el-form-item label='动作' prop='action' :rules='[{ required: true, message: `请选择动作`,trigger: "change"}]'>
            <el-select v-model="config.action">
                
                <el-option v-for='e,index in actions' :value="index.toString()" :key="index" :label='e'></el-option>
                
            </el-select>
        </el-form-item>
        
        <template v-if='orderaction.includes(config.action)'>
            
           
            <el-form-item label='方向' prop='direction' :rules="[{ required: true, message: `请选择方向`,trigger: 'change'}]">
                <el-select v-model="config.direction">
                    <el-option value='0' label='买'></el-option>
                    <el-option value='1' label='卖'></el-option>
                </el-select>
            </el-form-item>
            <el-form-item v-if="config.action !== '12'" label='超价' prop='overprice'  :rules="[{ required: true, message: `请输入超价`,trigger: 'blur'}]">
                <el-input v-model="config.overprice"></el-input>
            </el-form-item>
        </template>
        <el-form-item v-if="vlomeaction.includes(config.action)" label='手数' prop='volume' :rules='[{ required: true, message: `请输入手数`,trigger: "blur"}]'>
            <el-input v-model='config.volume'></el-input>
        </el-form-item>
        <el-form-item  v-if='config.action==="6"'  label='平今策略' prop='close' :rules="[{ required: true, message: `请选择平今策略`,trigger: 'change'}]">
            
                <el-select v-model='config.close'>
                    <el-option v-for='e,index in typeMap'  :value="index.toString()" :label='e' :key="index"></el-option>
                </el-select>
            
        </el-form-item>
    </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click='$emit("update:visible", false)'>取 消</el-button>
        <el-button type="primary" @click="cofirm">确 定</el-button>
    </div>
    </el-dialog>
</template>
<script>
    const arr = ['key', 'code', 'action', 'direction', 'overprice', 'volume', '', 'close']
    export default {
        props: ['actions', 'vlomeaction', 'editItem', 'orderaction', 'typeMap', 'visible'],
        data() {
            return {
                config: {
                    action: '',
                    direction: '',
                    overprice: '',
                    volume: '',
                    close: '',
                    key: '',
                    code: ''
                }
            }
        },
        methods: {
            inputHotkey(e) {
                            
                e.preventDefault();
                console.log(e)
                
                this.config.key = e.key;
                //区分小键盘
                if(e.keyCode >=95 && e.keyCode <= 111){
                    this.config.key = 'Num '+ this.config.key
                }
                this.config.code = e.keyCode.toString();
                
            },
            cofirm(){
                this.$refs['form'].validate((valid) => {
                    if (valid) {
                        const data = []
                        arr.forEach((key,index)=>{
                            if(key){
                            data[index] = this.config[key] || 0
                            }
                            
                        })
                        this.$emit('update-form', data)
                    }
                })
            }
        },
        watch: {
            editItem(val){
                arr.forEach((key,index)=>{
                    if(key){
                        this.config[key] = val[index] ===undefined? '': val[index];
                    }
                    
                })
            }
        }
    }
</script>
