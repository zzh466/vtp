import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'login',
      component: require('@/components/Login').default
    },
    {
      path: '/main',
      name: 'landing-page',
      component: require('@/components/LandingPage').default
    },
    {
      path: '/price',
      name: 'data-page',
      component: require('@/components/PriceData').default
    }, {
      path: '/config',
      name: 'config-page',
      component: require('@/components/Config').default
    }, 
    {
      path: '/subscribeIndicator',
      name: 'subscribeIndicator-page',
      component: require('@/components/subscribeIndicator').default
    }, 
    {
      path: '/trade',
      name: 'trade-page',
      component: require('@/components/FakeTrade').default
    }, {
      path: '/stock',
      name: 'stock-page',
      component: require('@/components/StockTrade').default
    },
    {
      path: '*',
      redirect: '/'
    }
  ]
})
