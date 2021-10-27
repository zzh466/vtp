import Vue from 'vue'
import axios from 'axios'
import { ipcRenderer } from 'electron'
import App from './App'
import router from './router'
import store from './store'
import ElementUI from 'element-ui';
import 'element-ui/lib/theme-chalk/index.css';
import './element'


if (!process.env.IS_WEB) Vue.use(require('vue-electron'))
Vue.http = Vue.prototype.$http = axios
Vue.config.productionTip = false
ipcRenderer.on('receive-tarde-data', (event, arg) => {
  
  window._$store.dispatch('updateData', arg)
})

ipcRenderer.on('change-ins', (event, arg) => {
  
  window._$store.dispatch('updateIns', arg)
})
ipcRenderer.on('remove-window', (event, arg) => {
  window._$store.dispatch('removeIns', arg)
})
/* eslint-disable no-new */
Vue.use(ElementUI);

new Vue({
  components: { App },
  router,
  store,

  template: '<App/>'
}).$mount('#app')
