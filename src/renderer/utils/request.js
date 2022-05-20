
import { ipcRenderer } from 'electron';
import { baseURL } from './utils';
export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res, config);
        return res;
    })
} 
let count = 0
export class TraderSocket{
    constructor(id){
        this.id = id;
        this.reconnect();
        this.task = [];
        this.ready=false;
    }
    onmessage(fn){
        this.onmessagefn = fn;
        this.ws.onmessage = this.onmessageResolve.bind(this);
    }
    onmessageResolve(msg){
        console.log(msg)
        msg = msg.data.split('@');
        switch(msg[0]){
            case 'BroadcastOpenInterest': 
                if(this.onmessagefn){
                    this.onmessagefn(msg[1]);
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
            
        }
    }
    reconnect(){
        const ws = new WebSocket(`ws://${baseURL}/ws/${this.id}`);
        this.ws =ws ;
        count++
        ws.onopen =  (e) =>{
            this.ready=true;
            console.log('客户端（client）：与服务器连接')
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
            this.ready=false;
            ipcRenderer.send('err-log', `socket已关闭`)
            
            this.ws = null;
            setTimeout( ()=>{
              
                this.reconnect()
            }, 2000)
        }
     
        this.ws.onmessage = this.onmessageResolve.bind(this);
        
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