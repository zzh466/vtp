import { stat } from "original-fs";
import request from "../../utils/request"; 
import {subscribeIndicatorKey} from '../../utils/utils'
  import { ipcRenderer } from 'electron';
const state = {
   userData: {
     
   },
   over_price: 0,
    vtp_client_openvolume_limit: '',
    vtp_client_openvolume_limit2: '',
    vtp_client_cancelvolume_limit: '',
    broadcast: true,
    config: {},
    activeCtpaccount: 0,
    vtp_server_indicator_array: []
  }
  
  const mutations = {
    'setstate'(state, {key, data}) {
      
       state[key]  = data;
    },

    'lock-user'(state){
      const {userData} = state;
      state.userData = {...userData, locked: true}
    },
    'unlock-user'(state){
      const {userData} = state;
      state.userData = {...userData, locked: false}
    },
    changeThr(state, thrRealProfit){
      const {userData} = state;
      state.userData = {...userData, thrRealProfit}
    },
    'update-config'(state, instrumentConfigVOList){
      
      localStorage.setItem(`config-${state.userData.id}`, JSON.stringify(instrumentConfigVOList));
      const {userData} = state;
      state.userData = {...userData, instrumentConfigVOList}
    }
  }
  const actions = {
   
    async 'get-config'({ commit,state }){
     
      const [ over_price, broadcast, vtp_client_cancelvolume_limit, vtp_client_openvolume_limit, vtp_client_openvolume_limit2, vtp_server_indicator_array,vtp_client_big_cancelvolume_limit ] = await Promise.all(
        [ "vtp_client_forced_liquidation_over_price", 'vtp_client_broadcast_openinterest', 'vtp_client_cancelvolume_limit', 'vtp_client_openvolume_limit','vtp_client_openvolume_limit2', 'vtp_server_indicator_array', 'vtp_client_big_cancelvolume_limit'].map(e =>ipcRenderer.invoke('get-config', e)));
       
     
      
      // localStorage.setItem(subscribeIndicatorKey, state.userData.subscribeIndicator);
      localStorage.setItem(`config-${state.userData.id}`, JSON.stringify(state.userData.instrumentConfigVOList));
      commit('setstate', {
        key: 'over_price',
        data: parseInt(over_price)
      })
      commit('setstate', {
        key: 'broadcast',
        data: broadcast
      })
      commit('setstate', {
        key: 'vtp_client_openvolume_limit',
        data: vtp_client_openvolume_limit || ''
      })
       commit('setstate', {
        key: 'vtp_client_openvolume_limit2',
        data: vtp_client_openvolume_limit2 || ''
      })
      commit('setstate', {
        key: 'vtp_client_cancelvolume_limit',
        data: vtp_client_cancelvolume_limit || ''
      })
      commit('setstate', {
        key: 'vtp_server_indicator_array',
        data: vtp_server_indicator_array || ''
      })
      commit('setstate', {
        key: 'vtp_client_big_cancelvolume_limit',
        data: vtp_client_big_cancelvolume_limit || ''
      })
     
      
    },
    async lock(){
      const result = await request({
        url: 'user/lock',
        method: 'POST'
      });
      if(result.code === "REQ_SUCCESS"){
        return result.lock
      }
      return false;
    }

  }
  
  export default {
    state,
    mutations,
    actions
  }
  