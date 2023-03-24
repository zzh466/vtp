
import { ipcRenderer } from 'electron';
import { baseURL } from './utils';
// const baseURL = '192.168.0.18:8082/vtpmanagerapi'
export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res, config);
        return res;
    })
} 
let count = 0
let shutdown = false;
export class TraderSocket{
    constructor(id,acountId){
        this.id = id;
        this.acountId = acountId;
        this.reconnect();
        this.task = [];
        this.ready=false;
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
            case 'BroadcastBigMarket': 
                if(this.onActiveInstrumentFn){
                    this.onActiveInstrumentFn(msg[1])
                }
                break;
            case 'BroadcastIndicator':
                
                ipcRenderer.send('broadcast-indicator', msg[1])
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
            
        }
    }
    reconnect(){
        if(shutdown) return;
        
        const ws = new WebSocket(`ws://${baseURL}/ws/${this.id}/${this.acountId}`);
        let timerId =0;
        this.ws =ws ;
        count++
        ws.onopen =  (e) =>{
            this.ready=true;
            const keepAlive =()=>  {
                const timeout = 15000;
                
                this.ws.send('im keep alive');
                console.log('客户端（client）：keep alive')
                timerId = setTimeout(keepAlive, timeout);
            }
            keepAlive()
            ipcRenderer.send('info-log', `socket第${count}次链接}`)
            console.log(`客户端（client）：与服务器连接 ${count}`)
            if(this.task.length){
                this.task.forEach(e => {
                    this.send(e)
                });
                this.task = [];
            }
        }
        ws.onerror=(e) =>{
            ipcRenderer.send('err-log', `socket已断开${JSON.stringify(e)}`)
            console.log('客户端（client）：与服务器的连接已断开'+ e);
            this.ws.close();
        }
        ws.onclose = ()=>{
            if (timerId) {
                clearTimeout(timerId);
            }
            this.ready=false;
            ipcRenderer.send('err-log', `socket已关闭`)
            
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