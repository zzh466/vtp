
import { ipcRenderer } from 'electron';

export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res);
        return res;
    })
} 

export class TraderSocket{
    constructor(){
        const ws = new WebSocket("ws://192.168.0.118:8080/vtpmanagerapi/ws");
        this.ws =ws ;
        ws.onopen = function (e) {
            console.log('客户端（client）：与服务器的连接已打开')
        }
    }
    onmessage(fn){
        this.ws.onmessage = fn;
    }
    send(msg){
        
        this.ws.send(msg)
    }
}