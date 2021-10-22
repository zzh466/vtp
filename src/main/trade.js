
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
        this.trader = _trader;
        this.getInstrumentList = [];
        this.requestID = 100;
        this.orderRef = 188888;
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
            
            _trader.on("rspUserLogin", function (requestId, isLast, field, info) {
                console.log("rspUserLogin: requestId", requestId);
                console.log("rspUserLogin: isLast", isLast);
                console.log("rspUserLogin: field", JSON.stringify(field));
                console.log("rspUserLogin: info", JSON.stringify(info));
                resolve()
                // Ϊ��ʱ����ѯ���к�Լ��Ϣ
                
            });
            
            _trader.on('rspError', function (requestId, isLast, field) {
                console.log(JSON.stringify(field));
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

            _trader.on('rtnOrder', function (field) {
                console.log('rtnOrder');
                console.log(field);
              });
              
            _trader.on('rtnTrade', function (field) {
                console.log('rtnTrade');
                console.log(field);
              })
              
            _trader.on('errInsert', function(a,b){
                console.log(a,b)
            })
            _trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
                console.log("in js code:", 'connect return val is ' + result);
            });
            
        })
    }
    getInstrument(id){
        return new Promise(resolve => {
            this.login.then(() => {
                const item = this.getInstrumentList.find(({id}) => id===id);
                console.log(item)
                if(item && item.field.PriceTick) {
                   
                    item.resolve = null;
                    resolve(item.field.PriceTick);
                    return;
                }
                this.trader.reqQryInstrument(id, function (field) {
                    // console.log('reqQryInstrument is callback');
                    // console.log(field);
                })
                this.getInstrumentList.push({
                    resolve,
                    id
                })
            })
           
        })
    }
    on(event, fn){
        this.emitter.on(event, fn.bind(this));
    }
    trade({instrumentID, direction, limitPrice, volumeTotalOriginal, combOffsetFlag}){
        console.log(this.getInstrumentList)
        const exchangeID =  this.getInstrumentList.find(({id}) => id===instrumentID).field.ExchangeID;

        var insertOrder = {
            "BrokerID": this.m_BrokerId,
            "InvestorID": this.m_InvestorId,
            "InstrumentID": instrumentID,
            "OrderRef": this.orderRef.toString(),
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
            "RequestID": this.requestID.toString(),
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
          this.requestID++;
          this.orderRef++;
          console.log(insertOrder);
          this.trader.reqOrderInsert(insertOrder, function (field) {
            console.log('ReqOrderInsert is callback');
            console.log(field);
          })
    }
}



export default Trade