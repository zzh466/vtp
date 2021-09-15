<template>
  <div>
      {{
        priceData.UpdateTime
      }}
  </div>
</template>

<script>
import { ipcRenderer } from 'electron';

  export default {
    mounted(){
        console.log(this.$route.query.id)
        ipcRenderer.send('register-event', this.$route.query.id);
        ipcRenderer.on(`receive-${this.$route.query.id}`, (event, arg) => {
            this.priceData = arg;
        })
    },
    data () {
      return {
          priceData: {

          }
      }
    }
  }
</script>