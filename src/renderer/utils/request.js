
import { ipcRenderer } from 'electron';
import { baseURL } from './utils';
export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res);
        return res;
    })
} 
let count = 0
export class TraderSocket{
    constructor(){
        this.reconnect();
        this.task = [];
        this.ready=false;
    }
    onmessage(fn){
        this.onmessagefn = fn;
        this.ws.onmessage = fn;
    }
    reconnect(){
        const ws = new WebSocket(`ws://${baseURL}/ws`);
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
        if(this.onmessagefn){
            this.ws.onmessage = this.onmessagefn;
        }
    }
    send(msg){
        console.log(msg)
        if(this.ready){
            this.ws.send(msg)
        }else {
            this.task.push(msg)
        }
    }
}