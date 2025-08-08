import net from 'net';
import Trade from './trade';
import axios from './request';
import cppmsg, { msg } from 'cppmsg';
import { errorLog, infoLog ,devLog} from './log';
import { Buffer } from 'buffer';
import events from 'events'
import {BrowserWindow } from 'electron'
// import {puppetIp} from '../renderer/utils/utils'
 let puppetIp 
import  {  
    orderData, tradeData, inderOrderData, tradingAccountData, cancelOrder, 
    acctounRsqData, respInfoVOData, commissionRateData, commissionRateRsqData,
    settlementInfo, rspSettlementInfo, settlementInfoConfirm, 
    qrySettlementConfirmInfo, qryInvestorPositionDetai}  from '../ctp/dataStruct';
const headMsg = new cppmsg.msg([
    ['length', 'int32'],
    ['head', 'int32']
  ]); 
const headMsg2 = new cppmsg.msg([
['length','int32'],
['encrygyKey','int64'],
['head', 'int32']
]) 
const orderMsg = new cppmsg.msg(orderData)
const tardeMsg = new cppmsg.msg(tradeData)
const insertMsg = new cppmsg.msg(inderOrderData)
const accountRspmsg = new cppmsg.msg(acctounRsqData)
const accountMsg = new cppmsg.msg(tradingAccountData)
const respInfomsg = new cppmsg.msg(respInfoVOData)
const cancelMsg = new cppmsg.msg(cancelOrder)
const commissionRateMsg = new cppmsg.msg(commissionRateData)
const commissionRateRsaMsg = new cppmsg.msg(commissionRateRsqData)
const settlementInfoConfirmMsg = new cppmsg.msg(settlementInfoConfirm)
const rspSettlementInfoMsg = new cppmsg.msg(rspSettlementInfo)
const dettalMsg = new cppmsg.msg(qryInvestorPositionDetai)
console.log(orderMsg.dsLen,tardeMsg.dsLen, accountMsg.dsLen)
let list = [];
let timer = null;
let count = 0;
class pupTrade {
    constructor(){
        this.encrygyKey = ''
        this.port = ''
        this.emitter = new  events.EventEmitter();
        this.cacheArr = [];
        this.relogining = false;
        this.reconnect = false;
    }
    parseData(data){
       
        let error;
        let respInfo;
        while(data.length > 8){
          
            const {head, length} = headMsg.decodeMsg(data.slice(0,8))
            // console.log('receive =---head', head, length,data.length)
            if(data.length < length){
                       
                return data
            }
            switch(head){
                case 1:             
                    
                    this.emitter.emit('connect')
                   
                    break
                case 2:
                    
                    errorLog('傀儡机消息头错误')
                    if(!this.relogining){
                        this.relogining = true;
                        this.emitter.emit('relogin')
                    }
                    break;
                case 33: 
                    
                     error = respInfomsg.decodeMsg(data.slice(8 +insertMsg.dsLen))
                    this.emitter.emit('error', error.info, true);
                    break
                case 35: 
                
                
                     error = respInfomsg.decodeMsg(data.slice(8 +cancelMsg.dsLen))
                    this.emitter.emit('error', error.info, true);
                    break
                case 38:
             
                    console.log(data.length)
                    const orderData = orderMsg.decodeMsg(data.slice(8))
                    console.log(orderData)
                    this.emitter.emit('rtnOrder', orderData)

                    break
                case 39:
                 
                    const tradeData = tardeMsg.decodeMsg(data.slice(8))
               
                    this.emitter.emit('rtnTrade', tradeData)
            
                    break
                case 41:
                 
                 
                    const  settlementInfoConfirmData = settlementInfoConfirmMsg.decodeMsg(data.slice(8, settlementInfoConfirmMsg.dsLen + 8 ))
        
                    console.log(settlementInfoConfirmData)
                    this.emitter.emit('rqSettlementInfoConfirm',undefined, true, settlementInfoConfirmData)
                    break
                case 42:
                    this.emitter.emit('rSettlementInfoConfirm',undefined, true, {})
                    console.log('42---', length)
                    break
                case 43:
                 
                    const  settlementInfoData = rspSettlementInfoMsg.decodeMsg(data.slice(8, rspSettlementInfoMsg.dsLen + 8 ))
                    respInfo= respInfomsg.decodeMsg(data.slice(rspSettlementInfoMsg.dsLen + 8))
                    if(respInfo.isLast){
                        console.log(respInfo.isLast, 111111111)
                    }
                   
                    this.emitter.emit('rqSettlementInfo', respInfo.requestId,respInfo.isLast, settlementInfoData)
                    break
                case 57:
                 
                    const  detaialPosition = dettalMsg.decodeMsg(data.slice(8, dettalMsg.dsLen + 8 ))
                   
                    respInfo= respInfomsg.decodeMsg(data.slice(dettalMsg.dsLen + 8))
                    // console.log(detaialPosition, respInfo)
                
                    //韩喆那边发出顺序不对
                   
                    this.emitter.emit('rqInvestorPositionDetail', respInfo.requestId,respInfo.isLast, detaialPosition)
                    
                    
                    break
                case 49:
                
                    const commissionData = commissionRateRsaMsg.decodeMsg(data.slice(8, commissionRateRsaMsg.dsLen + 8 ))
                    respInfo= respInfomsg.decodeMsg(data.slice(commissionRateRsaMsg.dsLen + 8))
                    // console.log('commissionData', commissionData, respInfo , data.length)
                    this.emitter.emit('rqInstrumentCommissionRate', respInfo.requestId, respInfo.isLast, commissionData, respInfo.info)
                    break
                case 50:
                    const accountData = accountMsg.decodeMsg(data.slice(8, accountMsg.dsLen + 8))
                    const {info, requestId, isLast} = respInfomsg.decodeMsg(data.slice(accountMsg.dsLen + 8))
                   
                    this.emitter.emit('rqTradingAccount', requestId , isLast,accountData, info)
                    break;
                
                default:
                    return ''
            }
            data = data.slice(length)
           
        }
       return data;
    }
    connect(callback){
    
        let tcp_client = new net.Socket();
        console.log('online tcp connect', puppetIp, this.port, 12312)
        infoLog(`傀儡机链接  ${puppetIp} ${ this.port}`)
        this.tcp_client = tcp_client;
        tcp_client.connect({
            host:puppetIp,
            port: this.port
        })
        this.relogining = false;
        tcp_client.setKeepAlive(true, 5*1000);
        tcp_client.on('close',(hadError ) =>{
            
            infoLog(`傀儡机断开 原因 ${hadError}`)
            if(this.reconnect){
                setTimeout(()=> this.connect(), 1000)
                this.reconnect = false;
            }
            // 
            this.emitter.emit('disconnected')
            this.tcp_client = null;
          })
        tcp_client.on('error',(error ) =>{
         
                const window=  BrowserWindow.getAllWindows();
                if(window.length){
                    window[0].webContents.send('error-msg', {msg:'当前账户连接服务器异常请联系管理员'});
                    errorLog(`傀儡机链接异常 ${JSON.stringify(error)}`)
                }
               this.emitter.emit('error', error)
            
            
        })
        tcp_client.on('data', (data) => {
            if(callback){
                callback()
                callback= null
            }
            // console.log(data.length)
            const cacheArr= this.cacheArr;
            if(cacheArr.length){
                cacheArr.push(data)
                const length = cacheArr.reduce((a,b)=> a + b.length, 0);
               
                if(length < 8) return
                
                data = Buffer.concat(cacheArr, length)
                this.cacheArr = []
            }
           
            
            data = this.parseData(data)
            if(data.length){
                this.cacheArr.push(data);
            }
        })
    }
    on(event, fn){
        // console.log(event, 'event')
        this.emitter.on(event, fn)
    }
    sendmsg(message, headcode){
       
        const length = headMsg2.dsLen+ message.length;
        console.log('send =---head', headcode, length, message.length,this.encrygyKey)
        const head = headMsg2.encodeMsg2({
            length,
            encrygyKey: this.encrygyKey,
            head: headcode
        })
    
        message = Buffer.concat([head, message], length)
        if(this.tcp_client){
            this.tcp_client.write(message)
        }else{
            const window=  BrowserWindow.getAllWindows();
            if(window.length){
                window[0].webContents.send('error-msg', {msg:'正在重连服务器，请稍后'});
            }
        }
       
    }
    reqOrderInsert(orderData, callback){
        console.log('insert', orderData)
        const message = insertMsg.encodeMsg2(orderData)
       
        this.sendmsg(message, 33)
        callback()
    }
    reqQryTradingAccount(BrokerID, InvestorID){
        // console.log('account', BrokerID, InvestorID)
        const message = accountRspmsg.encodeMsg2({
            BrokerID,
            InvestorID,
            CurrencyID: '',
            BizType: '',
            AccountID: ''
        })
        this.sendmsg(message, 50)
    }
    reqOrderAction(cancelData, callback){
        console.log(cancelData)
        const message = cancelMsg.encodeMsg2(cancelData)
        this.sendmsg(message, 34)
        callback()
    }
    reqQryInstrumentCommissionRate(BrokerID, InvestorID,InstrumentID){
        const data = {
            BrokerID,
            InvestorID,
            InstrumentID
        
        };
       
        const message = commissionRateMsg.encodeMsg2(data)
        // console.log('commission', message.toString())
        this.sendmsg(message, 49 )
    }
    reqQrySettlementInfoConfirm(BrokerID, InvestorID){
        const msg = new cppmsg.msg(qrySettlementConfirmInfo)
        const data = {
            BrokerID,
            InvestorID,
        
        };
        const message = msg.encodeMsg2(data)

        this.sendmsg(message, 41 )

    }
    reqQryInvestorPositionDetail(BrokerID, InvestorID){
        const data = {
            BrokerID,
            InvestorID,
            InstrumentID: ''
        };
        const message = commissionRateMsg.encodeMsg2(data)
        // devLog(message)
        this.sendmsg(message, 57 )
    }
    reqQrySettlementInfo(BrokerID, InvestorID, TradingDay){
        const msg = new cppmsg.msg(settlementInfo)
        const data = {
            BrokerID,
            InvestorID,
            TradingDay
        };
        const message = msg.encodeMsg2(data)

        this.sendmsg(message, 43 )
    }
    reqSettlementInfoConfirm(BrokerID, InvestorID, TradingDay){
       
        const data = {
            BrokerID,
            InvestorID,
        
        };
        const message = settlementInfoConfirmMsg.encodeMsg2(data)

        this.sendmsg(message, 42 )
    }

}

class onlineTrade extends Trade{
    constructor(args){
        super(args)
        console.log(args, 111)
    }
    init(args){
        this.tasks =[];
        this.relogining = false;
        this.id = args.id
        this.m_BrokerId =args.m_BrokerId;
        this.m_UserId = args.m_UserId;
        this.m_InvestorId = args.m_InvestorId;
        this._trader = new pupTrade()
        this._trader.on('connect', ()=>{
            this.emitter.emit('connect')
        })
        this._trader.on('error', (info,skip)=>{
            this.emitter.emit('error',  info,skip)
        })
        this._trader.on('relogin', ()=>{
            console.log('relogin')
            this.relogin()
        })
        
    }
    relogin(){
        this.getPuppetMsg().then(()=>{
            
            infoLog('傀儡机 重连')
            console.log(this._trader.tcp_client, 'tcp')
            if(this._trader.tcp_client){
                this._trader.reconnect = true;
                this._trader.tcp_client.destroy();
            }else{
                this._trader.connect()
            }
          
        })
    }
    getPuppetMsg(){ 
        //return axios({
        //     url: '/future/puppet',
        //     method: 'GET'
        // }).then(({data}) => {
        //     console.log(data,12133)
        //     if(data.code === 'REQ_SUCCESS'){
        //         const list = data.puppetInfoVOList
               
        //         const account = list.find(({futureId}) => futureId === this.id)
        //         if(account){
        //             console.log(account)
        //             const { encrygyKey, port} = account
        //             this._trader.encrygyKey =encrygyKey;
        //             this._trader.port = port;
        //             infoLog(`傀儡机信息 ${encrygyKey} ${port}`)
        //             console.log(`傀儡机信息 ${encrygyKey} ${port}`)
        //         }else{
        //             const window=  BrowserWindow.getAllWindows();
        //             if(window.length){
        //                 window[0].webContents.send('error-msg', {msg:'服务器信息不存在请联系管理员'});
        //                 errorLog('获取傀儡机信息不存在')
        //             }
        //             return Promise.reject()
        //         }
        //     }else{
        //         const window=  BrowserWindow.getAllWindows();
        //         if(window.length){
        //             window[0].webContents.send('error-msg', {msg:'获取服务器信息异常请联系管理员'});
        //             errorLog('获取傀儡机信息异常')
        //         }
        //         return Promise.reject()
        //     }
        // })
        return Promise.all([axios({
            url: '/future/puppet',
            method: 'GET'
        }),axios({
            url: 'property/info/proxy_mac_list',
            method: 'GET'
        }).then(e => e.data) 
    ]).then(([{data}, {propertyValue}]) => {
            console.log(typeof(propertyValue),12133)
            
            propertyValue= JSON.parse(propertyValue)
            if(data.code === 'REQ_SUCCESS'){
                const list = data.puppetInfoVOList
               
                const account = list.find(({futureId}) => futureId === this.id)
                const  ipData= propertyValue[this.m_UserId];
                console.log(ipData, 'ipdata')
                if(account && ipData){
                    console.log(account)
                    const { encrygyKey, port} = account
                    this._trader.encrygyKey =encrygyKey;
                    this._trader.port = ipData.port;
                    puppetIp = ipData.ip;
                    infoLog(`傀儡机信息 ${encrygyKey} ${port}`)
                    console.log(`傀儡机信息 ${encrygyKey} ${port}`)
                }else{
                    const window=  BrowserWindow.getAllWindows();
                    if(window.length){
                        window[0].webContents.send('error-msg', {msg:'服务器信息不存在请联系管理员'});
                        errorLog('获取傀儡机信息不存在')
                    }
                    return Promise.reject()
                }
            }else{
                const window=  BrowserWindow.getAllWindows();
                if(window.length){
                    window[0].webContents.send('error-msg', {msg:'获取服务器信息异常请联系管理员'});
                    errorLog('获取傀儡机信息异常')
                }
                return Promise.reject()
            }
        })
    }
    login(){
        this.login = this.getPuppetMsg().then(()=> {
          
            return new Promise((resolve, relect) => {
                console.log('login success')
                this._trader.connect(()=>{
                    console.log(111)
                    resolve();
                    this.haslogin = true;
                })
            })
        })
    }

  
    logout(){

    }

}

export default onlineTrade