
var ctp = require('../../build/Debug/ctp.node');
ctp.settings({ log: true });

// simnow hanzhe


console.log("try.js starting");

class Trade {
    constructor({
        ctp1_TradeAddress = "tcp://180.168.146.187:10201",
        m_BrokerId = "9999",
        m_UserId = "187849",
        m_InvestorId = "187849",
        m_PassWord = "hzlianhd520!",
        m_TradingDay = "20210805",
        m_AccountId = "187849",
        m_CurrencyId = "CNY",
        m_AppId = "simnow_client_test",
        m_AuthCode = "0000000000000000",
    }){
        const _trader = ctp.createTrader();
        this.trader = _trader;
        this.getInstrumentList = []
        this.login = new Promise((resolve, reject) => {
            _trader.on("connect", function (result) {
                console.log("in js code: ----> on connected , result=", result);
                trader.reqAuthenticate(m_BrokerId, m_AccountId, m_AuthCode, m_AppId, function (result) {
                    console.log("in js code: reqAuthenticate result=", result);
                });
            });
            _trader.on("rspAuthenticate", function (result) {
                console.log("in js code: ----> on rspAuthenticate , result=", result);
                trader.reqUserLogin(m_BrokerId, m_AccountId, m_PassWord, function (result) {
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
            
            trader.on('rqInstrument', function (requestId, isLast, field, info) {
               
                const {InstrumentID, PriceTick} = field;
                const item = this.getInstrumentList.find(id=> id===InstrumentID);
                const { resolve } = item;
                item.PriceTick = PriceTick;
                resolve(PriceTick);
            })
            
            _trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
                console.log("in js code:", 'connect return val is ' + result);
            });
            
        })
    }
    getInstrument(id){
        return new Promise(resolve => {
            this.login.then(() => {
                const item = this.getInstrumentList.find(id=> id===InstrumentID);
                if(item && item.PriceTick) {
                    resolve(item.PriceTick);
                    return;
                }
                trader.reqQryInstrument(id, function (field) {
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
}



export default Trade