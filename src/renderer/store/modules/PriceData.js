const state = {
    priceData: 0,
    InstrumentIDs: [],
    activeIns: ''
  }
  
  const mutations = {
    'tarde-data'(state, data) {
       state.priceData  = data;
    },
    'changeactive'(state, data) {
      // console.log(data, '11')
      state.activeIns  = data;
      if(!state.InstrumentIDs.includes(data)){
        state.InstrumentIDs.push(data);

      }
     
   },
   removeactive(state, data) {
    
      if(state.activeIns  === data){
        state.activeIns = ''
      }
      const index = state.InstrumentIDs.indexOf(data);
      if(index > -1){
        state.InstrumentIDs.splice(index, 1)
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
  