
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
        const ws = new WebSocket(`ws://${baseURL}/ws`);
        this.ws =ws ;
        ws.onopen = function (e) {
            count = 0;
            console.log('客户端（client）：与服务器连接')
        }
        ws.onerror=function(e){
            ipcRenderer.send('err-log', `socket已断开${JSON.stringify(e)}`)
            console.log('客户端（client）：与服务器的连接已断开'+ e);
            count ++;
            if(count > 10) {
                this.ws = null;
                console.log('socket链接超时');
            };
            this.ws = new WebSocket(`ws://${baseURL}/ws`);
            if(this.onmessagefn){
                this.ws.onmessage = this.onmessagefn;
            }
        }
    }
    onmessage(fn){
        this.onmessagefn = fn;
        this.ws.onmessage = fn;
    }
    send(msg){
        if(this.ws){
            this.ws.send(msg)
        }
    }
}