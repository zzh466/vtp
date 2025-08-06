// var ctpmini = require('../../build/Release/ctpmini/ctpmini.node');
var ctp = require('../../build/Release/ctp.node');
var events = require('events');
ctp.settings({ log: 
false
});
// ctpmini.settings({ log: 
//     true
//     });

import { errorLog, infoLog} from './log';
import axios from './request';
// simnow hanzhe
const eventType = new Set();
function recordAction(url, data, time){
    let _data = {}
    for(let key in data){
        _data[key.charAt(0).toLowerCase() + key.slice(1)] = data[key]
    }
    const current = new Date()
    if(time){
        _data.quotTm = +new Date(current.toLocaleDateString() +' '+ time)
    }
    _data.localTm = +current
    url = url + (data.UserID || data.InvestorID);
    console.log(url, _data)
    axios({
        method: 'POST',
        data: _data,
        url
    })
}

class Trade {
    constructor(options){
      
        this.init(options)
      
       
        // this.getInstrumentList = instruments.map(id => ({id}));
        this.requestID = Math.floor(Math.random() * 100) + 1;
        this.orderRef =  Math.floor(Math.random() * 100) + 1;
        this.emitter = new  events.EventEmitter();
        this.login()
        this.confirmTime = false;
        // this.next();
        if(options.tradeProxyCode === 1){
            this.chainOn('rqSettlementInfoConfirm', 'reqQrySettlementInfoConfirm', function(isLast,field){
                console.log(isLast,field, '3333333333333333333333333' );
                this.confirmTime = field.ConfirmTime;
                if(!field.ConfirmTime){
                    // this.chainOn('rqSettlementInfo', 'reqQrySettlementInfo', function(_,info){
                    //     this.emitter.emit('settlement-info', _,info)
                    // }, '');
                    this.chainOn('rSettlementInfoConfirm', 'reqSettlementInfoConfirm', function(){
                        this.confirmTime = true;
                      })
                }    
            });
        }
        
    }
    init({
        ctp1_TradeAddress = "tcp://180.168.146.187:10201",
        m_BrokerId = "9999",
        m_UserId = "136380",
        m_InvestorId = "136380",
        m_PassWord = "jpf000jpf",
        m_TradingDay = "20210805",
        m_AccountId = "136380",
        m_CurrencyId = "CNY",
        m_AppId = "simnow_client_test",
        m_AuthCode = "0000000000000000",
        instruments,
        userId,
        idol,
        tradeProxyCode = 1
    }){
        console.log(arguments)
        this.m_BrokerId =m_BrokerId;
        this.m_UserId = m_UserId;
        this.m_InvestorId = m_InvestorId;
        this.instruments = instruments;
        this.m_AppId = m_AppId;
        this.m_AuthCode= m_AuthCode;
        this.ctp1_TradeAddress = ctp1_TradeAddress;
        this.m_PassWord = m_PassWord;
        this.m_AccountId = m_AccountId;
        this.tasks =[];
        this.userId = userId
        this.needrecord = idol;
        let _trader;
        switch(tradeProxyCode){
            case 1:
             _trader  = ctp.createTrader();
            break;
            case 2:
            // _trader  = ctpmini.createTrader() ;  
            break;
            case 6:
            break;
        }
        
        
        this._trader = _trader;
    }
    // getInstrument(insId){
    //     return new Promise(resolve => {
    //         const item = this.getInstrumentList.find(({id}) => id===insId);
    //         if( item.field.PriceTick) {
    //             item.resolve = null;
    //             const {PriceTick, ExchangeID} = item.field
    //             resolve({PriceTick, ExchangeID});
    //             return;
    //         }else {
    //             item.resolve = resolve
    //         }
    //         this.chainSend('reqQryInstrument', insId, function (field) {
    //             // console.log('reqQryInstrument is callback');
    //             // console.log(field);
    //         })
            
           
    //     })
    // }
    login(){
        const _trader = this._trader;
        this.login = new Promise((resolve, reject) => {
            _trader.on("connect",  (result)=> {
                
                console.log("in js code: ----> on connected , result=", result);
                this.emitter.emit('connect')
                this.startTrader = false;
                _trader.reqAuthenticate(this.m_BrokerId, this.m_AccountId, this.m_AuthCode, this.m_AppId, function (result) {
                    console.log("in js code: reqAuthenticate result=", result);
                });
            });
            _trader.on("rspAuthenticate",  (result)=> {
                console.log("in js code: ----> on rspAuthenticate , result=", result);
                _trader.reqUserLogin(this.m_BrokerId, this.m_AccountId, this.m_PassWord, function (result) {
                    console.log("in js code: reqUserlogin result=", result);
                });
            })
            
            _trader.on("rspUserLogin",  (requestId, isLast, field, info) =>{
                console.log("rspUserLogin: requestId", requestId);
                console.log("rspUserLogin: isLast", isLast);
                console.log("rspUserLogin: field", JSON.stringify(field));
                console.log("rspUserLogin: info", JSON.stringify(info));
                info = info || {};
                if(info.ErrorID){
                    this.emitter.emit('error', info.ErrorMsg);
                    errorLog(`登陆失败，${info.ErrorMsg}`)
                    reject(info.ErrorMsg)
                    return
                }
               
                // 拿到计算手续费和持仓
                this.haslogin = true;
                resolve()
                // Ϊ��ʱ����ѯ���к�Լ��Ϣ
                
            });
            
            _trader.on('rspError',  (requestId, isLast, field, info) =>{
                console.log(JSON.stringify(field));
                errorLog(`错误，${info && info.ErrorMsg}`)
                this.emitter.emit('error',field);
                reject()
            });
            
            // _trader.on('rqInstrument',  (requestId, isLast, field, info) => {
              
            //     const {InstrumentID, PriceTick, ExchangeID} = field;
            
            //     const item = this.getInstrumentList.find(({id}) => id===InstrumentID);
            //     if(item){
            //         const { resolve } = item;
            //         item.field = field;
            //         if(resolve){
                
            //             resolve({PriceTick, ExchangeID});
            //         }
            //     }else {
            //         this.getInstrumentList.push({
            //             id: InstrumentID,
            //             field
            //         })
            //     }
                
            //     if(isLast){
            //         this.next()
            //         this.emitter.emit('instrument-finish',  this.getInstrumentList)
            //     }
                
            // })
            
            // _trader.on('rtnOrder',  (field) => {
            //     console.log('rtnOrder ---- receive' );
            //     this.emitter.emit('rtnOrder', field);
               
            //   });
              
            // _trader.on('rtnTrade',  (field) => {
            //     console.log('rtnTrade ---- receive' );
            //     this.emitter.emit('rtnTrade', field)
                
            //   })
           
            _trader.on('errInsert', (a,b) =>{
                errorLog(`报单错误，${JSON.stringify(b)}`)
                this.emitter.emit('error', b, true);
                console.log(a,b)
            })
         
            this.reconnect()
            _trader.on('rspUserLogout',  (requestId, isLast, field, info) =>{
                console.log('logout', this.shouldReconnect)
                if(this.shouldReconnect){
                    this.reconnect()
                }else {
                  
                    _trader.disposed()
                }
              })     
        })
    }
    send(event, ...args){
        console.log(event,...args, 'send')
        this._trader[event](...args)
    }
    on(event, fn){
        // console.log(`${event} ---- register`);
        const _trader = this._trader;
        if(eventType.has(event)){
            return
        }
        eventType.add(event)
        _trader.on(event, (...args) => {
            // console.log(`${event} ---- receive`);
            if(this.needrecord && this.startTrader){
                if(event === 'rtnTrade'){
                    recordAction(`/order/rtn/trade/${this.userId}/`, args[0])
                }else if(event === 'rtnOrder'){
                    recordAction(`/order/rtn/order/${this.userId}/`, args[0])
                }
            }
            this.emitter.emit(event, ...args);
        })
        this.emitter.on(event, fn.bind(this));
    }
    //ctp除了order trade其他的on必须等待一个完成后再调用另一个
    chainOn(event, func,fn, ...extend){
        this.login.then(()=>{
           
            const {_trader, tasks, m_BrokerId, m_InvestorId} = this;
         
            if(!eventType.has(event)){
                eventType.add(event)
                console.log(`${event} ---- register`, ...extend, 123132);
                _trader.on(event, (requestId, isLast, field, info) =>{
                    if(isLast){
                        this.next()
                    }
                    
                    console.log(`${event} ---- receive`, isLast);
                    fn.call(this,isLast,field)
                })
            }
           
         
            if(extend.length){
                this.chainSend(func, m_BrokerId, m_InvestorId,...extend, function (field) {
                    // console.log(`${func} is callback`);
                    console.log(arguments);
                })
            }else{
              
                this.chainSend(func, m_BrokerId, m_InvestorId, function (field) {
                    // console.log(`${func} is callback`);
                    console.log(arguments);
                })
            }
            
        })
        
    }
    next(){
        const { tasks} = this;
       
        var last = tasks.shift();
        const timeout = 1000
        // console.log(tasks, last, 123132)
       if(tasks.length){
            let task = tasks[0];
            //ctp一秒只能发一个请求
            // console.log(task)
            setTimeout(()=> {
                task()
            }, timeout)
       }else if(typeof last ==='function' ){
            tasks.push(setTimeout(()=>{
                this.next()
            }, timeout))
       }
    }
    chainSend(event, ...args){
        console.log('chainsend', event, this.haslogin)
        if(!this.haslogin)return;
        const { tasks} = this;
        console.log('task', tasks.length)
        const task =()=>{
            this.send(event,  ...args)
        }
        // task()
        if(!tasks.length){
            task()
        }
        tasks.push(task)
    }
    emitterOn(event, fn){
        this.emitter.on(event, fn.bind(this));
    }
    getKey(key){
        const value = this[key];
        this[key]++;
        return value.toString();
    }
    trade({instrumentID, direction, limitPrice, volumeTotalOriginal, combOffsetFlag, ExchangeID, ContingentCondition='1', StopPrice = 0, OrderPriceType ='2'}, time){
        // console.log(this.getInstrumentList, instrumentID)
        // const exchangeID =  this.getInstrumentList.find(({id}) => id===instrumentID).field.ExchangeID;
        let TimeCondition = '3';
        this.startTrader = true;
        if(OrderPriceType === '1'){
            TimeCondition = '1';
            limitPrice = 0;
        }
       
        var insertOrder = {
            "BrokerID": this.m_BrokerId,
            "InvestorID": this.m_InvestorId,
            "InstrumentID": instrumentID,
            "OrderRef": this.getKey('orderRef'),
            "UserID": this.m_UserId,
            OrderPriceType,
            "Direction": direction,
            "CombOffsetFlag": combOffsetFlag,
            "CombHedgeFlag": "1",
            "LimitPrice": limitPrice,
            "VolumeTotalOriginal": volumeTotalOriginal,
            TimeCondition,
            //"GTDDate": "",
            "VolumeCondition": "1",
            //"MinVolume": "",
            ContingentCondition,
            StopPrice,
            "ForceCloseReason": "0",
            //"IsAutoSuspend": "",
            //"BusinessUnit": "",
            "RequestID": this.getKey('requestID'),
            //"UserForceClose": "",
            //"IsSwapOrder": "",
            "ExchangeID": ExchangeID,
            "InvestUnitID": "",
            "AccountID": "",
            "CurrencyID": "",
            "ClientID": "",
            "IPAddress": "",
            "MacAddress": "",
          };
          console.log(insertOrder);
          if(this.needrecord){
            recordAction(`/order/insert/${this.userId}/`, insertOrder, time)
            }
            if(ExchangeID === 'CFFEX'){
                while(volumeTotalOriginal){
                    if(volumeTotalOriginal > 20){
                        insertOrder = {...insertOrder, VolumeTotalOriginal: 20, "OrderRef": this.getKey('orderRef')}
                        volumeTotalOriginal = volumeTotalOriginal- 20;
                    }else{
                        insertOrder = {...insertOrder, VolumeTotalOriginal: volumeTotalOriginal, "OrderRef": this.getKey('orderRef'),}
                        volumeTotalOriginal = 0
                    }
                  
                    this.send('reqOrderInsert', insertOrder, function (field) {
                        console.log('ReqOrderInsert is callback');
                        console.log(field);
                    })
                    
                }
            
            }else{
                this.send('reqOrderInsert', insertOrder, function (field) {
                    console.log('ReqOrderInsert is callback');
                    console.log(field);
                })
            }
           
            
        
    }
    cancel(arr){
        if(!arr.length) return Promise.resolve(false);
        this.startTrader = true;
        return new Promise(resolve =>{
            let count = 0
            let _combOffsetFlag = false;
            arr.forEach(({OrderRef, FrontID, SessionID, ExchangeID, OrderSysID, InstrumentID,combOffsetFlag, time }) => {
                const cancelOrder = {
                    "RequestID": this.getKey('requestID'),
                    "BrokerID": this.m_BrokerId,
                    "InvestorID": this.m_InvestorId,
                    OrderActionRef: this.requestID.toString(),
                    OrderRef,
                    FrontID,
                    SessionID,
                    ExchangeID,
                    OrderSysID,
                    ActionFlag: '0',
                    VolumeChange: 0,
                    UserID: this.m_UserId,
                    InstrumentID
                }
                if(combOffsetFlag !=='0'){
                    _combOffsetFlag = true
                }
                console.log(cancelOrder);
                if(this.needrecord){
                    recordAction(`/order/action/${this.userId}/`, cancelOrder, time)
                }
                this.send('reqOrderAction',cancelOrder, function(field){
                    count++
                    console.log('reqOrderAction is callback');
                    console.log(field);
                    if(count === arr.length){
                        resolve(_combOffsetFlag)
                    }
                    
                })
            });
        })
        
    }
    logout(){
        this.haslogin = false;
        this.send('reqUserLogout', this.m_BrokerId, this.m_UserId, function (result) {
            console.log("in js code: reqUserlogout result=", result);
        });
    }
    reconnect(){
        this._trader.connect(this.ctp1_TradeAddress, undefined, 2, 0, function (result) {
            console.log("in js code:", 'connect return val is ' + result);
        });
    }
}



export default Trade