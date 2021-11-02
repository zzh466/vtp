
var ctp = require('../../build/Release/ctp.node');
var events = require('events');
ctp.settings({ log: true });

// simnow hanzhe



class Trade {
    constructor({
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
    }){
        this.m_BrokerId =m_BrokerId;
        this.m_UserId = m_UserId;
        this.m_InvestorId = m_InvestorId;
        const _trader = ctp.createTrader();
        this._trader = _trader;
        this.getInstrumentList = [];
        this.tasks =[];
        this.requestID = Math.floor(Math.random() * 100) + 1;
        this.orderRef =  Math.floor(Math.random() * 100) + 1;
        this.emitter = new  events.EventEmitter();
        this.login = new Promise((resolve, reject) => {
            _trader.on("connect", function (result) {
                console.log("in js code: ----> on connected , result=", result);
                _trader.reqAuthenticate(m_BrokerId, m_AccountId, m_AuthCode, m_AppId, function (result) {
                    console.log("in js code: reqAuthenticate result=", result);
                });
            });
            _trader.on("rspAuthenticate", function (result) {
                console.log("in js code: ----> on rspAuthenticate , result=", result);
                _trader.reqUserLogin(m_BrokerId, m_AccountId, m_PassWord, function (result) {
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
                    reject(info.ErrorMsg)
                    return
                }
               
                // 拿到计算手续费和持仓
               
                resolve()
                // Ϊ��ʱ����ѯ���к�Լ��Ϣ
                
            });
            
            _trader.on('rspError', function (requestId, isLast, field) {
                console.log(JSON.stringify(field));
                this.emitter.emit('error',field);
                reject()
            });
            
            _trader.on('rqInstrument',  (requestId, isLast, field, info) => {
               
                const {InstrumentID, PriceTick} = field;
                console.log(field)
                const item = this.getInstrumentList.find(({id}) => id===InstrumentID);
                const { resolve } = item;
                item.field = field;
                resolve(PriceTick);
            })
            
            // _trader.on('rtnOrder',  (field) => {
            //     console.log('rtnOrder ---- receive' );
            //     this.emitter.emit('rtnOrder', field);
               
            //   });
              
            // _trader.on('rtnTrade',  (field) => {
            //     console.log('rtnTrade ---- receive' );
            //     this.emitter.emit('rtnTrade', field)
                
            //   })
           
            _trader.on('errInsert', function(a,b){
                this.emitter.emit('error', a);
                console.log(a,b)
            })
         
            _trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
                console.log("in js code:", 'connect return val is ' + result);
            });            
        })
        let a =  {};
        a.valueOf = undefined
        this.chainOn('rqSettlementInfoConfirm', 'reqQrySettlementInfoConfirm', function(isLast,field){
           
            if(!field.ConfirmTime)    
                this.chainOn('rqSettlementInfo', 'reqQrySettlementInfo', function(_,info){
                    this.emitter.emit('settlement-info', _,info)
                });
            
        });
    }
    getInstrument(insId){
        return new Promise(resolve => {
            this.login.then(() => {
                const item = this.getInstrumentList.find(({id}) => id===insId);
                if(item && item.field.PriceTick) {
                    item.resolve = null;
                    resolve(item.field.PriceTick);
                    return;
                }
                this.getInstrumentList.push({
                    resolve,
                    id: insId
                })
                this._trader.reqQryInstrument(insId, function (field) {
                    // console.log('reqQryInstrument is callback');
                    // console.log(field);
                })
             
            })
           
        })
    }
    on(event, fn){
        console.log(`${event} ---- register`);
        const _trader = this._trader;
        _trader.on(event, (...args) => {
            console.log(`${event} ---- receive`);
            this.emitter.emit(event, ...args);
        })
        this.emitter.on(event, fn.bind(this));
    }
    //ctp除了order trade其他的on必须等待一个完成后再调用另一个
    chainOn(event, func,fn){
        this.login.then(()=>{
           
            const {_trader, tasks, m_BrokerId, m_InvestorId} = this;
            console.log(`${event} ---- register`, tasks);
            _trader.on(event, (requestId, isLast, field, info) =>{

                if(isLast){
                    this.next()
                    if(!tasks.length){
                        tasks.push(setTimeout(()=>{
                            this.next()
                        }, 1000))
                    }
                 
                }
                
                console.log(`${event} ---- receive`);
                fn.call(this,isLast,field)
            })
            if(!tasks.length){
               
                _trader[func](m_BrokerId, m_InvestorId, function (field) {
                    console.log(`${func} is callback`);
                    console.log(arguments);
                })
            }
            tasks.push({
            event,
            func
            })
            
        })
        
    }
    next(){
        const {_trader, tasks, m_BrokerId, m_InvestorId} = this;
        tasks.shift();
       if(tasks.length){
            let _func = tasks[0].func;
            //ctp一秒只能发一个请求
            setTimeout(()=> {
                _trader[_func](m_BrokerId, m_InvestorId, function (field) {
                    console.log(`${_func} is callback`);
                    console.log(arguments);
                })
            }, 1000)
       }
    }
    emitterOn(event, fn){
        this.emitter.on(event, fn.bind(this));
    }
    getKey(key){
        const value = this[key];
        this[key]++;
        return value.toString();
    }
    trade({instrumentID, direction, limitPrice, volumeTotalOriginal, combOffsetFlag}){
        console.log(this.getInstrumentList, instrumentID)
        const exchangeID =  this.getInstrumentList.find(({id}) => id===instrumentID).field.ExchangeID;

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
            //"ContingentCondition": "",
            //"StopPrice": "",
            //"ForceCloseReason": "",
            //"IsAutoSuspend": "",
            //"BusinessUnit": "",
            "RequestID": this.getKey('requestID'),
            //"UserForceClose": "",
            //"IsSwapOrder": "",
            "ExchangeID": exchangeID,
            "InvestUnitID": "",
            "AccountID": "",
            "CurrencyID": "",
            "ClientID": "",
            "IPAddress": "",
            "MacAddress": "",
          };
          console.log(insertOrder);
          this._trader.reqOrderInsert(insertOrder, function (field) {
            console.log('ReqOrderInsert is callback');
            console.log(field);
          })
    }
    cancel(arr){
        arr.forEach(({OrderRef, FrontID, SessionID, ExchangeID, OrderSysID, InstrumentID}) => {
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
            console.log(cancelOrder);
            this._trader.reqOrderAction(cancelOrder, function(field){
                console.log('reqOrderAction is callback');
                console.log(field);
            })
        });
    }
}



export default Trade