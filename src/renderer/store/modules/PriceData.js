const state = {
    priceData: 0,
    InstrumentIDs: ['j2201','j2110','j2111','j2112','j2202','j2203','j2205','lh2111','lh2201','jm2201','jm2110','jm2111','jm2112','jm2202','jm2203','jm2205'],
    activeIns: ''
  }
  
  const mutations = {
    'tarde-data'(state, data) {
       state.priceData  = data;
    },
    'changeactive'(state, data) {
      state.activeIns  = data;
   },
  }
  const actions = {
    'updateData' ({ commit }, data) {    
      commit('tarde-data', data)
    },
    updateIns({ commit }, data) {
 
      commit('changeactive', data)
    },
  }
  
  export default {
    state,
    mutations,
    actions
  }
  