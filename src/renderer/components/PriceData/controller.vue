<template>
    <div class="controller">
        <span @mousedown="go('Rewind')" @mouseup="end">
        <i v-if="step > 8" class="el-icon-d-arrow-left"></i>
        <i v-else class="el-icon-arrow-left"></i>
        </span>
        <template>
        <i v-if='!pause' @click='start("Stop")' class="el-icon-video-pause"></i>
        <i v-else @click='start("Start")' class="el-icon-video-play"></i>
        </template>
        <span @mousedown="go('Forward')" @mouseup="end">
        <i v-if="step > 8" class="el-icon-d-arrow-right"></i>
        <i v-else class="el-icon-arrow-right"></i>
        </span>
    </div>
</template>
<script>
import {ipcRenderer} from 'electron'
export default {
    data() {
        return {
            pause: false,
            step: 4
        }   
    },
    methods: {
        send(msg){
            ipcRenderer.send('send-fake-trade-msg',`NotifyQuotDataHist${msg}`)
        },
        start(flag){
            this.send(flag);
            this.pause = !this.pause
        },
        go(direction){
           this.send(`${direction}@${this.step}`)
            this.timer = setInterval(()=>{
                if(this.step < 40){
                this.step = this.step + 4
                }
             this.send(`${direction}@${this.step}`)
            }, 1000)
            
        },
        end(){
            clearInterval(this.timer);
            this.step = 4
        }
    }
}
</script>
<style >
.controller{
    text-align: center;
    background-color: white;
    width: 80px;
}
</style>