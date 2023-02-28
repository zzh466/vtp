import Vue from 'vue'
import axios from 'axios'
import { ipcRenderer } from 'electron'
import App from './App'
import router from './router'
import store from './store'

import ElementUI, {MessageBox} from 'element-ui';
import 'xe-utils'
import {  Header,Column, Table} from 'vxe-table'
// import 'vxe-table/lib/style.css'

import 'element-ui/lib/theme-chalk/index.css';






if (!process.env.IS_WEB) Vue.use(require('vue-electron'))
Vue.http = Vue.prototype.$http = axios
Vue.config.productionTip = false

ipcRenderer.on('change-ins', (event, arg) => {
  
  window._$store.dispatch('updateIns', arg)
})
ipcRenderer.on('remove-window', (event, arg) => {
  window._$store.dispatch('close', arg)
})
ipcRenderer.on('error-msg', (event, {msg, code}) => {
  MessageBox({
    type: 'error',
    title: '错误',
    message: msg,
    callback(){
      if(code){
        event.sender.send(`error-msg-${code}`)
      }
    }
  })
  setTimeout(()=>MessageBox.close(), 5000)
})
/* eslint-disable no-new */

Vue.use(Table).use( Header).use(Column);
Vue.use(ElementUI);

Vue.component('Table', {
  props: ['columns', 'height', 'tableData', "rowKey"],
  template: ` <vxe-table :data='tableData' :column-config="{useKey: rowKey, isCurrent: true}"  :height="height" size='mini' @row-click='cell-click' @cell-dblclick='dbclick' border='full'>
          <vxe-column
            v-for="column in columns"
            :key='column.prop'
            :title="column.label"
            :field="column.prop"
            :width="column.width|| 60"
            
            >
            <template v-if="column.render || column.component" scope="scope">
              <div v-if='column.render' :class='column.class? typeof column.class === "function"?column.class(scope.row): column.class: ""'>{{column.render(scope.row, scope.$rowIndex )}}</div>
              <component v-if='column.component' :is='column.component' :data='scope.row' :index='scope.$rowIndex' @action='$emit(column.component, $event)'></component>
            </template>
          </vxe-column>
        </vxe-table>`,
    methods:{
    rowClick(...args){
      this.$emit('row-click', ...args);
    },
    dbclick(...args){
      this.$emit('row-dblclick', ...args);
    },
  }
})
new Vue({
  components: { App },
  router,
  store,

  template: '<App/>'
}).$mount('#app')
