// var path = require('path')
// var nodePath = path.join(__dirname, '/build/Debug/ctp.node');
// var ctp = require(nodePath);

var ctp = require('../../build/Debug/ctp.node');
ctp.settings({log: true});

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

// prod syc
// var ctp1_TradeAddress = "tcp://101.230.209.180:53305";
// var m_BrokerId = "2016";
// var m_UserId = "01020351";
// var m_InvestorId = "01020351";
// var m_PassWord = "tianyuan630528!";
// var m_TradingDay = "20210805";
// var m_AccountId = "01020351";
// var m_CurrencyId = "CNY";
// var m_AppId = "simnow_client_test";
// var m_AuthCode = "0000000000000000";

console.log("try.js starting");

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

// trader.on("rspUserLogin", function (requestId, isLast, field, info) {
//   // console.log("in js code: requestId", requestId);
//   // console.log("in js code: isLast", isLast);
//   // console.log("in js code: field", JSON.stringify(field));
//   console.log("in js code: info", JSON.stringify(info));
//   trader.reqQryTradingAccount(m_BrokerId, m_AccountId, function(result){
//     console.log("in js code:", 'reqQryTradingAccount return val is '+result);
//   });
// });

// trader.on('rqTradingAccount',function(requestId, isLast, field, info){
//   console.log("in js code:", 'rqTradingAccount callback');
//   //console.log("in js code:", JSON.stringify(field));
//   console.log("in js code:", JSON.stringify(info));

//   trader.reqQrySettlementInfo(m_BrokerId, m_AccountId, function(result,iRequestID){
//     console.log('settlementinfo return val is '+result);
//   });
// });
trader.on("rspUserLogin", function (requestId, isLast, field, info) {
  console.log("rspUserLogin: requestId", requestId);
  console.log("rspUserLogin: isLast", isLast);
  console.log("rspUserLogin: field", JSON.stringify(field));
  console.log("rspUserLogin: info", JSON.stringify(info));
  trader.reqQrySettlementInfo(m_BrokerId, m_AccountId, function(result,iRequestID){
    console.log('settlementinfo return val is '+result);
  });
});

trader.on('rqSettlementInfo',function(requestId, isLast, field, info){
  console.log("rqSettlementInfo: requestId", requestId);
  console.log("rqSettlementInfo: isLast", isLast);
  console.log("rqSettlementInfo: field", JSON.stringify(field));
  console.log('rqsettlementinfo callback');
});

trader.on('rtnOrder',function(field){
  console.log(field);
});
trader.on('rspError',function(requestId, isLast, field){
  console.log(JSON.stringify(field));
});

trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
  console.log("in js code:", 'connect return val is '+result);
});

console.log("in js code:", 'continute');
