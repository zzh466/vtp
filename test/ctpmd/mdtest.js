var ctpmd = require('../../build/Release/ctpmd.node');
ctpmd.settings({ log: true });

var ctp1_TradeAddress = "tcp://140.207.230.227:41213";
var m_BrokerId = "5040";
var m_UserId = "28000099";
var m_InvestorId = "28000099";
var m_PassWord = "991530";
var m_AccountId = "28000099";
var m_AppId = "client_ls_v21.4.5";
var m_AuthCode = "WFD5SYJ7X4WTECL7";


console.log("try.js starting");

var login = false;
var ctpMdUser = ctpmd.createMdUser();

ctpMdUser.on("connect", function (result) {
    console.log("in js code: ----> on connected , result=", result);
    ctpMdUser.reqUserLogin(m_BrokerId, m_AccountId, m_PassWord, function (result) {
        console.log("in js code: reqUserlogin result=", result);
    });
});

ctpMdUser.on("rspUserLogin", function (requestId, isLast, field, info) {
    console.log("in js code: ----> rspUserLogin: requestId", requestId);
    console.log("in js code: ----> rspUserLogin: isLast", isLast);
    console.log("in js code: ----> rspUserLogin: field", JSON.stringify(field));
    console.log("in js code: ----> rspUserLogin: info", JSON.stringify(info));
    login = true;

    ctpMdUser.subscribeMarketData("ag2512", function (field) {
        console.log('rspSubMarketData is callback');
        console.log(field);
    })
});

ctpMdUser.on('rspError', function (requestId, isLast, field) {
    console.log(JSON.stringify(field));
});

ctpMdUser.on("rspSubMarketData", function (requestId, isLast, field, info) {
    console.log("in js code: ----> rspSubMarketData");
    console.log("in js code: ----> rspSubMarketData: requestId", requestId);
    console.log("in js code: ----> rspSubMarketData: isLast", isLast);
    console.log("in js code: ----> rspSubMarketData: field", JSON.stringify(field));
    console.log("in js code: ----> rspSubMarketData: info", JSON.stringify(info));
})

ctpMdUser.on('rtnDepthMarketData', function (quot) {
    console.log('in js code: ----> rtnDepthMarketData');
    console.log(JSON.stringify(quot));
})

ctpMdUser.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
    console.log("in js code:", 'connect return val is ' + result);
});

console.log("in js code:", 'continute');

// function checkOrderStatus() {
//     console.log(1);
// }
 
// const timer = setInterval(() => {
//     checkOrderStatus()
// }, 1000)
 
// 退出的时候一定要清空
//clearInterval(timer)
