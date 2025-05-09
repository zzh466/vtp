var ctp = require('../../build/Debug/ctpmini.node');
ctp.settings({ log: false });

// simnow hanzhe
// var ctp1_TradeAddress = "tcp://180.168.146.187:10201";
// var m_BrokerId = "9999";
// var m_UserId = "187849";
// var m_InvestorId = "187849";
// var m_PassWord = "hzlianhd520!";
// var m_TradingDay = "20210805";
// var m_AccountId = "187849";
// var m_CurrencyId = "CNY";
// var m_AppId = "simnow_client_test";
// var m_AuthCode = "0000000000000000";


// CTP MINI
var ctp1_TradeAddress = "tcp://58.240.234.72:21803";
var m_BrokerId = "9060";
var m_UserId = "199803";
var m_InvestorId = "199803";
var m_PassWord = "abc123";
var m_TradingDay = "20210805";
var m_AccountId = "199803";
var m_CurrencyId = "CNY";
var m_AppId = "client_lxz_2024083";
var m_AuthCode = "3KBA9KLGMWPJN6K7";

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

    // 为空时查询所有
    var instrumentID = "j";
    trader.reqQryInstrument(instrumentID, function (field) {
        console.log('reqQryInstrument is callback');
        console.log(field);
    })
});

trader.on('rspError', function (requestId, isLast, field) {
    console.log(JSON.stringify(field));
});

trader.on('rqInstrument', function (requestId, isLast, field, info) {
    console.log('rqInstrument');
    console.log(JSON.stringify(requestId));
    console.log(JSON.stringify(isLast));
    console.log("rqInstrument: field", JSON.stringify(field));
    console.log("rqInstrument: info", JSON.stringify(info));
})

trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
    console.log("in js code:", 'connect return val is ' + result);
});

console.log("in js code:", 'continute');
