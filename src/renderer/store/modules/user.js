import request from "../../utils/request"; 
const state = {
   userData: {
     
   },
   over_price: 0,
    openvolume_limit: '',
    broadcast: true,
    config: {},
    activeCtpaccount: 0
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
    }
   
  }
  const actions = {
   
    async 'get-config'({ commit,state }){
     
      const [ over_price, broadcast, openvolume_limit, vtp_client_cancelvolume_limit ] = await Promise.all([request({
        url: 'property/info/vtp_client_forced_liquidation_over_price',
        method: 'GET'
      }), request({
        url: 'property/info/vtp_client_broadcast_openinterest',
        method: 'GET'
      }), request({
        url: 'property/info/vtp_client_openvolume_limit',
        method: 'GET'
      }), request({
        url: 'property/info/vtp_client_cancelvolume_limit',
        method: 'GET'
      })]);
      
      localStorage.setItem(`config-${state.userData.id}`, JSON.stringify(state.userData.instrumentConfigVOList));
      commit('setstate', {
        key: 'over_price',
        data: parseInt(over_price.propertyValue)
      })
      commit('setstate', {
        key: 'broadcast',
        data: broadcast.propertyValue
      })
      commit('setstate', {
        key: 'openvolume_limit',
        data: openvolume_limit.propertyValue || ''
      })
      commit('setstate', {
        key: 'vtp_client_cancelvolume_limit',
        data: vtp_client_cancelvolume_limit.propertyValue || ''
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
  