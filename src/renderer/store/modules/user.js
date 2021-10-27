const state = {
   userData: {},
   hotKey: 'Num 5,101,2,0,0,1,0,0;Num 1,97,0,0,1,1,0,0;Num 3,99,0,1,1,1,0,0;Num 4,100,0,0,6,1,0,0;Num 6,102,0,1,6,1,0,0;Num 7,103,0,0,20,1,0,0;Num 9,105,0,1,20,1,0,0;Num 0,96,0,0,0,1,0,0;Num +,107,0,1,0,1,0,0;Q,81,0,1,-1,1,0,0;W,87,0,1,-2,1,0,0;E,69,0,1,-3,1,0,0;C,67,0,0,-3,1,0,0;1,49,3,0,0,1,0,0;3,51,3,0,0,3,0,0;2,50,3,0,0,2,0,0;Z,90,0,0,-1,1,0,0;X,88,0,0,-2,1,0,0;4,52,3,0,0,4,0,0;5,53,3,0,0,5,0,0;U,85,6,0,0,1,0,0;I,73,6,0,0,1,0,1;O,79,6,0,0,1,0,2;P,80,6,0,0,1,0,3;'
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
  