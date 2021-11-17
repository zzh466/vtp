const state = {
    priceData: 0,
    InstrumentIDs: ['j2201','j2202','j2203','j2205','lh2111','lh2201','jm2201','jm2205', 'pg2111', 'pg2112'],
    activeIns: ''
  }
  
  const mutations = {
    'tarde-data'(state, data) {
       state.priceData  = data;
    },
    'changeactive'(state, data) {
      console.log(data, '11')
      state.activeIns  = data;
   },
   removeactive(state, data) {
    if(state.activeIns  === data){
      state.activeIns = ''
    }
    },
  }
  const actions = {
    'updateData' ({ commit }, data) {    
      commit('tarde-data', data)
    },
    updateIns({ commit }, data) {
      commit('changeactive', data)
    },
    removeIns({commit}, data){
      commit('removeactive', data)
    }
  }
  
  export default {
    state,
    mutations,
    actions
  }
  