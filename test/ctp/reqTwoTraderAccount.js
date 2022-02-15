var ctp = require('../../build/Debug/ctp.node');
var ctp_2 = require('../../build/Debug/ctp.node');
ctp.settings({ log: true });

// simnow hanzhe
var ctpTradeAddress = "tcp://180.168.146.187:10201";
var m_BrokerId = "9999";
var m_AppId = "simnow_client_test";
var m_AuthCode = "0000000000000000";

var m_UserId_1 = "187849";
var m_InvestorId_1 = "187849";
var m_PassWord_1 = "hzlianhd520!";
var m_AccountId_1 = "187849";

var m_UserId_2 = "187849";
var m_InvestorId_2 = "187849";
var m_PassWord_2 = "hzlianhd520!";
var m_AccountId_2 = "187849";

var user_info_arr = [[m_UserId_1, m_InvestorId_1, m_PassWord_1, m_AccountId_1],
                [m_UserId_2, m_InvestorId_2, m_PassWord_2, m_AccountId_2]];

console.log("try.js starting");

var login = false;
var trader_1 = ctp.createTrader();
var trader_2 = ctp_2.createTrader();

trader_1.on("connect", function (result) {
    console.log("in js code: ----> on connected , result=", result);
    trader_1.reqAuthenticate(m_BrokerId, m_AccountId_1, m_AuthCode, m_AppId, function (result) {
        console.log("in js code: reqAuthenticate result=", result);
    });
});
// trader_2.on("connect", function (result) {
//     console.log("in js code: ----> on connected , result=", result);
//     trader_2.reqAuthenticate(m_BrokerId, m_AccountId_2, m_AuthCode, m_AppId, function (result) {
//         console.log("in js code: reqAuthenticate result=", result);
//     });
// });

trader_1.on("rspAuthenticate", function (result) {
    console.log("in js code: ----> on rspAuthenticate , result=", result);
    trader_1.reqUserLogin(m_BrokerId, m_AccountId_1, m_PassWord_1, function (result) {
        console.log("in js code: reqUserlogin result=", result);
    });
})
// trader_2.on("rspAuthenticate", function (result) {
//     console.log("in js code: ----> on rspAuthenticate , result=", result);
//     trader_2.reqUserLogin(m_BrokerId, m_AccountId_2, m_PassWord_2, function (result) {
//         console.log("in js code: reqUserlogin result=", result);
//     });
// })

trader_1.on("rspUserLogin", function (requestId, isLast, field, info) {
    console.log("rspUserLogin: requestId", requestId);
    console.log("rspUserLogin: isLast", isLast);
    console.log("rspUserLogin: field", JSON.stringify(field));
    console.log("rspUserLogin: info", JSON.stringify(info));
    login = true;

    trader_1.reqQryTradingAccount(m_BrokerId, m_InvestorId_1, function (field) {
        console.log('reqQryTradingAccount is callback');
        console.log(field);
    })
});
// trader_2.on("rspUserLogin", function (requestId, isLast, field, info) {
//     console.log("rspUserLogin: requestId", requestId);
//     console.log("rspUserLogin: isLast", isLast);
//     console.log("rspUserLogin: field", JSON.stringify(field));
//     console.log("rspUserLogin: info", JSON.stringify(info));
//     login = true;

//     trader_2.reqQryTradingAccount(m_BrokerId, m_InvestorId_2, function (field) {
//         console.log('reqQryTradingAccount is callback');
//         console.log(field);
//     })
// });

trader_1.on('rspError', function (requestId, isLast, field) {
    console.log(JSON.stringify(field));
});
// trader_2.on('rspError', function (requestId, isLast, field) {
//     console.log(JSON.stringify(field));
// });

trader_1.on('rqTradingAccount', function (requestId, isLast, field, info) {
    console.log('rqTradingAccount');
    console.log(JSON.stringify(requestId));
    console.log(JSON.stringify(isLast));
    console.log("rqTradingAccount: field", JSON.stringify(field));
    console.log("rqTradingAccount: info", JSON.stringify(info));
})
// trader_2.on('rqTradingAccount', function (requestId, isLast, field, info) {
//     console.log('rqTradingAccount');
//     console.log(JSON.stringify(requestId));
//     console.log(JSON.stringify(isLast));
//     console.log("rqTradingAccount: field", JSON.stringify(field));
//     console.log("rqTradingAccount: info", JSON.stringify(info));
// })

trader_1.connect(ctpTradeAddress, undefined, 2, 0, function (result) {
    console.log("in js code:", 'connect return val is ' + result);
});
// trader_2.connect(ctpTradeAddress, undefined, 2, 0, function (result) {
//     console.log("in js code:", 'connect return val is ' + result);
// });

console.log("in js code:", 'continute');
