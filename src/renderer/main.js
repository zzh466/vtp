import Vue from 'vue'
import axios from 'axios'
import { ipcRenderer } from 'electron'
import App from './App'
import router from './router'
import store from './store'
import ElementUI, {MessageBox, Notification} from 'element-ui';
import 'element-ui/lib/theme-chalk/index.css';
import './element'


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
 
})
/* eslint-disable no-new */
Vue.use(ElementUI);
Vue.component('Table', {
  props: ['columns', 'height', 'tableData'],
  template: ` <el-table :data='tableData'   :height="height" size='mini' @row-click='rowClick' @row-dblclick='dbclick'
          border>
          <el-table-column
            v-for="column in columns"
            :key='column.prop'
            :label="column.label"
            :prop="column.prop"
            :width="column.width|| 60"
            row-key='key'
            :highlight-current-row='true'
            >
            <template v-if="column.render || column.component" scope="scope">
              <div v-if='column.render' :class='column.class? typeof column.class === "function"?column.class(scope.row): column.class: ""'>{{column.render(scope.row)}}</div>
              <component v-if='column.component' :is='column.component' :data='scope.row'></component>
            </template>
          </el-table-column>
        </el-table>`,
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
