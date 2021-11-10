var ctp = require('../../build/Debug/ctp.node');
ctp.settings({ log: true });

// simnow hanzhe
var ctp1_TradeAddress = "tcp://180.168.146.187:10201";
var m_BrokerId = "9999";
var m_UserId = "187849";
var m_InvestorId = "187849";
var m_PassWord = "hzlianhd520!";
var m_TradingDay = "20210805";
var m_AccountId = "187849";
var m_CurrencyId = "CNY";
var m_AppId = "simnow_client_test";
var m_AuthCode = "0000000000000000";

console.log("try.js starting");

var login = false;
var trader = ctp.createTrader();

trader.on("connect", function (result) {
    console.log("in js code: ----> on connected , result=", result);
    trader.reqAuthenticate(m_BrokerId, m_AccountId, m_AuthCode, m_AppId, function (result) {
        console.log("in js code: reqAuthenticate result=", result);
    });
});
trader.on("rspAuthenticate", function (result) {
    console.log("in js code: ----> on rspAuthenticate , result=", result);
    trader.reqUserLogin(m_BrokerId, m_AccountId, m_PassWord, function (result) {
        console.log("in js code: reqUserlogin result=", result);
    });
})

trader.on("rspUserLogin", function (requestId, isLast, field, info) {
    console.log("rspUserLogin: requestId", requestId);
    console.log("rspUserLogin: isLast", isLast);
    console.log("rspUserLogin: field", JSON.stringify(field));
    console.log("rspUserLogin: info", JSON.stringify(info));
    login = true;

    // 空字符串 查询所有
    var instrumentId = 'jm2201';
    trader.reqQryInstrumentCommissionRate(m_BrokerId, m_InvestorId, instrumentId, function (field) {
        console.log('reqQryInstrumentCommissionRate is callback');
        console.log(field);
    })
});

trader.on('rspError', function (requestId, isLast, field) {
    console.log(JSON.stringify(field));
});

trader.on('rqInstrumentCommissionRate', function (requestId, isLast, field, info) {
    console.log('rqInstrumentCommissionRate');
    console.log(JSON.stringify(requestId));
    console.log(JSON.stringify(isLast));
    console.log("rqInstrumentCommissionRate: field", JSON.stringify(field));
    console.log("rqInstrumentCommissionRate: info", JSON.stringify(info));
})

trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
    console.log("in js code:", 'connect return val is ' + result);
});

console.log("in js code:", 'continute');
