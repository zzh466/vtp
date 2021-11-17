import request from "../../utils/request"; 
const state = {
   userData: {
     
   },
   over_price: 0,
    openvolume_limit: 0,
  
    config: {}
  }
  
  const mutations = {
    'setstate'(state, {key, data}) {

       state[key]  = data;
    },

    'lock-user'(state){
      const {userData} = state;
      state.userData = {...userData, locked: true}
    }
   
  }
  const actions = {
   
    async 'get-config'({ commit,state }){
    
      const [config_1, over_price, broadcast, openvolume_limit] = await Promise.all([request({
        url: 'user/config',
        type: 'GET'
      }), request({
        url: 'property/info/vtp_client_forced_liquidation_over_price',
        type: 'GET'
      }), request({
        url: 'property/info/vtp_client_broadcast_openinterest',
        type: 'GET'
      }), request({
        url: 'property/info/vtp_client_openvolume_limit',
        type: 'GET'
      })]);
      localStorage.setItem(`config-${state.userData.account}`, JSON.stringify(config_1));
      commit('setstate', {
        key: 'over_price',
        data: over_price.propertyValue
      })
      commit('setstate', {
        key: 'broadcast',
        data: broadcast.propertyValue
      })
      commit('setstate', {
        key: 'openvolume_limit',
        data: openvolume_limit.propertyValue
      })
     
    }

  }
  
  export default {
    state,
    mutations,
    actions
  }
  