
var ctp = require('../../build/Release/ctp.node');
var events = require('events');
ctp.settings({ log: 
false
});

import { errorLog, infoLog} from './log';
// simnow hanzhe



class Trade {
    constructor(options){
      
        this.init(options)
        const _trader = ctp.createTrader();
        this._trader = _trader;
       
        // this.getInstrumentList = instruments.map(id => ({id}));
        this.requestID = Math.floor(Math.random() * 100) + 1;
        this.orderRef =  Math.floor(Math.random() * 100) + 1;
        this.emitter = new  events.EventEmitter();
      
        this.login = new Promise((resolve, reject) => {
            _trader.on("connect",  (result)=> {
                
                console.log("in js code: ----> on connected , result=", result);
                this.emitter.emit('connect')
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
            
            _trader.on('rspError', function (requestId, isLast, field, info) {
                console.log(JSON.stringify(field));
                errorLog(`错误，${info.ErrorMsg}`)
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
        this.next();
        this.chainOn('rqSettlementInfoConfirm', 'reqQrySettlementInfoConfirm', function(isLast,field){
            console.log(field, '3333333333333333333333333' );
            if(!field.ConfirmTime)    
                this.chainOn('rqSettlementInfo', 'reqQrySettlementInfo', function(_,info){
                    this.emitter.emit('settlement-info', _,info)
                });
            
        });
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
        instruments
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
    send(event, ...args){
        console.log(event,...args)
        this._trader[event](...args)
    }
    on(event, fn){
        // console.log(`${event} ---- register`);
        const _trader = this._trader;
        _trader.on(event, (...args) => {
            // console.log(`${event} ---- receive`);
            this.emitter.emit(event, ...args);
        })
        this.emitter.on(event, fn.bind(this));
    }
    //ctp除了order trade其他的on必须等待一个完成后再调用另一个
    chainOn(event, func,fn, ...extend){
        this.login.then(()=>{
           
            const {_trader, tasks, m_BrokerId, m_InvestorId} = this;
            // console.log(`${event} ---- register`, tasks);
            _trader.on(event, (requestId, isLast, field, info) =>{
                if(isLast){
                    this.next()
                }
                
                // console.log(`${event} ---- receive`);
                fn.call(this,isLast,field)
            })
         
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
        if(!this.haslogin)return;
        const { tasks} = this;
        const task =()=>{
            this.send(event,  ...args)
        }
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
    trade({instrumentID, direction, limitPrice, volumeTotalOriginal, combOffsetFlag, ExchangeID, contingentCondition: ContingentCondition,   stopPrice: StopPrice,}){
        // console.log(this.getInstrumentList, instrumentID)
        // const exchangeID =  this.getInstrumentList.find(({id}) => id===instrumentID).field.ExchangeID;

        var insertOrder = {
            "BrokerID": this.m_BrokerId,
            "InvestorID": this.m_InvestorId,
            "InstrumentID": instrumentID,
            "OrderRef": this.getKey('orderRef'),
            "UserID": this.m_UserId,
            //"OrderPriceType": "",
            "Direction": direction,
            "CombOffsetFlag": combOffsetFlag,
            //"CombHedgeFlag": "",
            "LimitPrice": limitPrice,
            "VolumeTotalOriginal": volumeTotalOriginal,
            //"TimeCondition": "",
            //"GTDDate": "",
            //"VolumeCondition": "",
            //"MinVolume": "",
            ContingentCondition,
            StopPrice,
            //"ForceCloseReason": "",
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
          this.send('reqOrderInsert', insertOrder, function (field) {
            console.log('ReqOrderInsert is callback');
            console.log(field);
          })
    }
    cancel(arr){
        if(!arr.length) return Promise.resolve(false);
        return new Promise(resolve =>{
            let count = 0
            let _combOffsetFlag = false;
            arr.forEach(({OrderRef, FrontID, SessionID, ExchangeID, OrderSysID, InstrumentID,combOffsetFlag }) => {
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