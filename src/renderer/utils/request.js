
import { ipcRenderer } from 'electron';
import { baseURL, speak } from './utils';
// const baseURL = '192.168.0.18:8082/vtpmanagerapi'
export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res, config);
        return res;
    })
} 
let count = 0
let shutdown = false;
   //因为会出现行情重复提醒情况 所以要做过滤
const big_vix_arr = [];
const volume_ratio_arr =[]
const all_arr = []
const together_arr = [];
const expireTime = 10 *60*1000
function checkExpire(arr, instrument){
    if(arr.includes(instrument) || together_arr.includes(instrument)){
        return true
    }
    arr.push(instrument)
    together_arr.push(instrument)
    setTimeout(()=>{ 
        const index = together_arr.indexOf(instrument);
        if(index >-1){
            together_arr.splice(index,1)
        }
      }, 60 * 1000)
    setTimeout(()=>{ 
      const index = arr.indexOf(instrument);
      if(index >-1){
        arr.splice(index,1)
      }
    }, expireTime)
}
export class TraderSocket{
    constructor(id,acountId){
        this.id = id;
        this.acountId = acountId;
        this.task = [];
        this.initTask = []
        this.reconnect();
        
        this.ready=false;
        this.reconnectFlag = Date.now();
    }
    onmessage(fn){
        this.onmessagefn = fn;
        // this.ws.onmessage = this.onmessageResolve.bind(this);
    }
    onActiveInstrument(fn){
        this.onActiveInstrumentFn = fn;
    }
    onmessageResolve(msg){
        console.log(msg)
        msg = msg.data.split('@');
        switch(msg[0]){
            case 'heart':
                //heartbeat
                this.reconnectFlag = Date.now();
                break;
            case 'BroadcastOpenInterest': 
                if(this.onmessagefn){
                    this.onmessagefn(msg[1]);
                    // this.onActiveInstrumentFn(msg[1].split(":")[0])
                }   
              break;
            case "UpdateForceLiquidationThreshold":
                window._$store.commit('changeThr', msg[1]);
                break;
            case 'UpdateKeymap':
                break
            case 'LockUser':
                window._$store.commit('lock-user');
                break;
            case 'UnLockUser':
                window._$store.commit('unlock-user');
                break
            case 'BroadcastBigMarketX': 
               
                if(checkExpire(all_arr, msg[1]))return
                
                ipcRenderer.send('info-log', `收到大行情信息 ${msg[1]}`)
                if(this.onActiveInstrumentFn){
                    this.onActiveInstrumentFn(msg[1])
                }
                break;
            case 'BroadcastIndicatorX':
               

                const _Msg = msg[1].split('-')
                const instrument = _Msg[0]
                const type = _Msg[1]
                let notifi;
                switch(type){
                    case 'big_vix':
                        if(checkExpire(big_vix_arr, instrument))return
                    
                        notifi = '有异动'
                        break
                    case 'volume_ratio':
                        if(checkExpire(volume_ratio_arr, instrument))return
                        notifi = '有大盘口'
                        break
                }
                ipcRenderer.send('info-log', `收到异动信息 ${msg[1]}`)
                if(this.onActiveInstrumentFn){
                    this.onActiveInstrumentFn(instrument, true, notifi)
                }
                // ipcRenderer.send('broadcast-indicator', msg[1])
                break;
            case "BroadcastForceSleep":
                
                if(this.closeTrade){
                    this.closeTrade(msg[1])
                }
                break;
                
            case "FutureAccountBusy":
                ipcRenderer.send('info-log','stock发送指令关闭客户端')
                this.shutdownClient();
                break
            case "ShutDownClient":
                ipcRenderer.send('info-log','stock发送指令关闭客户端')
                this.shutdownClient();
                break;
            case "MarketCloseImmediately":
                speak('还有一分钟就收盘啦，请注意平仓')
            
        }
    }
    reconnect(){
        if(shutdown) return;
        let reconnecting = 0;
        let now = Date.now();
        if(count && now - this.reconnectFlag < 30 * 60 * 1000){
            reconnecting = 1
        }
        const ws = new WebSocket(`ws://${baseURL}/ws/${this.id}/${this.acountId}/${reconnecting}`);
        let timerId =0;
        this.ws =ws ;
        count++
        ws.onopen =  (e) =>{
            
            this.task = this.initTask.concat(this.task)
            this.ready=true;
            const keepAlive =()=>  {
                const timeout = 15 * 1000;
                
                this.ws.send('im keep alive');
                console.log('客户端（client）：keep alive')
                timerId = setTimeout(keepAlive, timeout);
            }
            keepAlive()
            ipcRenderer.send('info-log', `${this.acountId}socket第${count}次链接`)
            console.log(`客户端（client）：与服务器连接 ${count}`)
            if(this.task.length){
                this.task.forEach(e => {
                    this.ws.send(e)
                });
                this.task = [];
            }
        }
        ws.onerror=(e) =>{
            ipcRenderer.send('err-log', `${this.acountId}socket已断开${JSON.stringify(e)}`)
            console.log('客户端（client）：与服务器的连接已断开'+ e);
            this.ws.close();
        }
        ws.onclose = ()=>{
            if (timerId) {
                clearTimeout(timerId);
            }
            this.ready=false;
            ipcRenderer.send('err-log', `${this.acountId}socket已关闭`)
            
            this.ws = null;
            console.log('客户端（client）已关闭');
            setTimeout( ()=>{
              
                this.reconnect()
            }, 2000)
        }
     
        this.ws.onmessage = this.onmessageResolve.bind(this);
        
    }
    shutdownClient(){
        shutdown = true;
        this.ws.close();
        ipcRenderer.send('close-main');
    }
    send(msg){
        console.log(msg)
        msg = `NotifyOpenInterest@${msg}`
        if(this.ready){
            this.ws.send(msg)
        }else {
            this.task.push(msg)
        }
    }
}