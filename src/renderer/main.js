import Vue from 'vue'
import axios from 'axios'
import { ipcRenderer } from 'electron'
import App from './App'
import router from './router'
import store from './store'

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
  ipcRenderer.removeListener(`receive-${arg}`, (event, arg) => {
  })
})
/* eslint-disable no-new */
new Vue({
  components: { App },
  router,
  store,
  template: '<App/>'
}).$mount('#app')
