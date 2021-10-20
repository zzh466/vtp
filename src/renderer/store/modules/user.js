const state = {
   userData: {}
  }
  
  const mutations = {
    'setstate'(state, {key, data}) {
       state[key]  = data;
    },
   
  }
  const actions = {
    setUserData ({ commit }, data) {    
      commit('setstate', {
          key: 'userData',
          data
      })
    },
  }
  
  export default {
    state,
    mutations,
    actions
  }
  