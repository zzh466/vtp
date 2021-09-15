import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'landing-page',
      component: require('@/components/LandingPage').default
    },
    {
      path: '/price',
      name: 'data-page',
      component: require('@/components/PriceData').default
    },
    {
      path: '*',
      redirect: '/'
    }
  ]
})
