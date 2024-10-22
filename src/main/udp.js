import dgram from 'dgram'
import  events  from 'events';
import cppmsg, { msg } from 'cppmsg';
export default class udpClient {
    constructor(){
        this.emitter = new  events.EventEmitter();
        this.client = new dgram.createSocket('udp4');
        this.tasks =  [];
        this.sleepTime = null
    }
    connect({host, port}, func){
        // if(this.timer){
        //     clearInterval(this.timer)
        //     this.timer= null
        // }
        this.host  = host;
        this.port = port;
        func();
        const client = this.client;
        client.on("message",(msg,rinfo)=>{
            // console.log('msg', msg)
            this.emitter.emit('data', msg)
            
        });
        client.on("close",()=>{
            clearInterval(this.timer)
            this.timer= null
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
        const _msg = new Buffer(msg.length)
        msg.copy(_msg)
        this.client.send(_msg, this.port, this.host, (err)=>{
            
            console.log('udp', err)
        })
        // if(!this.sleepTime){
        //     // console.log(msg) 
        //     this.next(_msg)
          
        // }else{
        //     // console.log(this.tasks)
        //     // console.log(msg , 'in')
            
        //     this.tasks.push(_msg)
        //     // console.log(this.tasks)
        // }
       
    }
    next(msg){
        console.log(this.tasks, 11111)
        if(!msg){
            if(!this.tasks.length)return
            msg = this.tasks.shift()
        }
        console.log(msg)
        this.client.send(msg, this.port, this.host, (err)=>{
            
            // console.log('udp', err)
        })
        this.sleepTime = setTimeout(()=>{
            this.next()
            this.sleepTime = null
        }, 0)
      

    }

    setKeepAlive(){
        const heartbeatMsg = new cppmsg.msg([
            ['size', 'int32'],
            ['iCmdID', 'int32'],
            ['time', 'int64']
          ])
        this.timer = setInterval(()=>{
            this.write(heartbeatMsg.encodeMsg2({
                size: 8,
                iCmdID: 91,
                time: +new Date()
            }))
        }, 10*1000)
    }
 }