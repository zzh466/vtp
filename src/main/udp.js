import dgram from 'dgram'
import  events  from 'events';
export default class udpClient {
    constructor(){
        this.emitter = new  events.EventEmitter();
        this.client = new dgram.createSocket('udp4');
    }
    connect({host, port}, func){
        this.host  = host;
        this.port = port;
        func();
        const client = this.client;
        client.on("message",(msg,rinfo)=>{
            console.log('msg', msg)
            this.emitter.emit('data', msg)
            
        });
        client.on("close",()=>{
            this.emitter.emit('close')
        });
        client.on("error",(err)=>{
            this.emitter.emit('error', err)
            console.error("客户端错误："+err.message);
          });
    }
    on(event, func){
        this.emitter.on(event, func)
    }
    destroy(){
        this.client.close();
    }
    write(msg){
        console.log(msg)
        this.client.send(msg, this.port, this.host, (err)=>{
            
            console.log('udp', err)
        })
    }
    setKeepAlive(){

    }
 }