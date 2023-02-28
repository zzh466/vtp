import Vue from 'vue';
Vue.component('config-action', {
    props: ['index'],
    template: `<div>
        <el-button type='primary' size='mini' @click='action("edit")'>编辑</el-button>
        <el-button  type='primary' size='mini' @click='action("del")'>删除</el-button>
    </div>`,
    methods: {
        action(type){
            this.$emit('action', {type, index : this.index})
        }
    }
})