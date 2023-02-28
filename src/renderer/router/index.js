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
      path: '*',
      redirect: '/'
    }
  ]
})
