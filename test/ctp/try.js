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

ctp_info_1 = {
"TradeAddress": ctp1_TradeAddress,
"BrokerId": m_BrokerId,
"UserId": m_UserId,
"InvestorId": m_InvestorId,
"PassWord": m_PassWord,
"TradingDay": m_TradingDay,
"AccountId": m_AccountId,
"CurrencyId": m_CurrencyId,
"AppId": m_AppId,
"AuthCode": m_AuthCode,
}

var ctp1_TradeAddress_2 = "tcp://180.168.146.187:10201";
var m_BrokerId_2 = "9999";
var m_UserId_2 = "136380";
var m_InvestorId_2 = "136380";
var m_PassWord_2 = "jpf000jpf";
var m_TradingDay_2 = "20210805";
var m_AccountId_2 = "136380";
var m_CurrencyId_2 = "CNY";
var m_AppId_2 = "simnow_client_test";
var m_AuthCode_2 = "0000000000000000";

ctp_info_2 = {
"TradeAddress": ctp1_TradeAddress_2,
"BrokerId": m_BrokerId_2,
"UserId": m_UserId_2,
"InvestorId": m_InvestorId_2,
"PassWord": m_PassWord_2,
"TradingDay": m_TradingDay_2,
"AccountId": m_AccountId_2,
"CurrencyId": m_CurrencyId_2,
"AppId": m_AppId_2,
"AuthCode": m_AuthCode_2,
}

// var ctp1_TradeAddress = "tcp://180.169.95.252:41205";
// var m_BrokerId = "5040";
// var m_UserId = "91700058";
// var m_InvestorId = "91700058";
// var m_PassWord = "sxy123456";
// var m_TradingDay = "20210805";
// var m_AccountId = "91700058";
// var m_CurrencyId = "CNY";
// var m_AppId = "client_syc_v21.4.1";
// var m_AuthCode = "3DJRGBBOBJMVQQTT";

now_user_index = 0;
ctp_info_arr = [ctp_info_1, ctp_info_2];

console.log("try.js starting");

var login = false;
var trader = ctp.createTrader();

trader.on("connect", function (result) {
  console.log("in js code: ----> on connected , result=", result);
  trader.reqAuthenticate(ctp_info_arr[now_user_index].BrokerId, ctp_info_arr[now_user_index].AccountId, 
    ctp_info_arr[now_user_index].AuthCode, ctp_info_arr[now_user_index].AppId, function (result) {
    console.log("in js code: reqAuthenticate result=", result);
  });
});
trader.on("rspAuthenticate", function (result) {
  console.log("in js code: ----> on rspAuthenticate , result=", result);
  trader.reqUserLogin(ctp_info_arr[now_user_index].BrokerId, ctp_info_arr[now_user_index].AccountId, 
    ctp_info_arr[now_user_index].PassWord, function (result) {
    console.log("in js code: reqUserlogin result=", result);
  });
})

trader.on("rspUserLogin", function (requestId, isLast, field, info) {
  console.log("rspUserLogin: requestId", requestId);
  console.log("rspUserLogin: isLast", isLast);
  console.log("rspUserLogin: field", JSON.stringify(field));
  console.log("rspUserLogin: info", JSON.stringify(info));
  login = true;

  trader.reqQryTradingAccount(ctp_info_arr[now_user_index].BrokerId, ctp_info_arr[now_user_index].AccountId, function(result){
    console.log("in js code:", 'reqQryTradingAccount return val is '+result);
  });
});

trader.on('rspUserLogout', function (requestId, isLast, field, info) {
  console.log("rspUserLogout: requestId", requestId);
  console.log("rspUserLogout: isLast", isLast);
  console.log("rspUserLogout: field", JSON.stringify(field));
  console.log('rspUserLogout callback');

  if (now_user_index) {
    now_user_index = 0;
  } else {
    now_user_index = 1;
  }
  trader.connect(ctp_info_arr[now_user_index].TradeAddress, undefined, 2, 0, function (result) {
    console.log("in js code:", 'connect2 return val is ' + result);
  });
})

var t_bInsertOrder = false;
trader.on('rqSettlementInfo', function (requestId, isLast, field, info) {
  console.log("rqSettlementInfo: requestId", requestId);
  console.log("rqSettlementInfo: isLast", isLast);
  console.log("rqSettlementInfo: field", JSON.stringify(field));
  console.log('rqsettlementinfo callback');
});

trader.on('rspError', function (requestId, isLast, field) {
  console.log(JSON.stringify(field));
});

trader.on('rtnOrder', function (field) {
  console.log('rtnOrder');
  console.log(field);
});

trader.on('rqTradingAccount', function (requestId, isLast, field, info) {
  console.log("rqTradingAccount");
  console.log("rqTradingAccount: requestId", JSON.stringify(requestId));
  console.log("rqTradingAccount: isLast", JSON.stringify(isLast));
  console.log("rqTradingAccount: field", JSON.stringify(field));
  console.log("rqTradingAccount: info", JSON.stringify(info));

  trader.reqUserLogout(ctp_info_arr[now_user_index].BrokerId, ctp_info_arr[now_user_index].UserId, function (field) {
      console.log('reqUserLogout is callback');
      console.log(field);
  })
})

trader.connect(ctp_info_arr[now_user_index].TradeAddress, undefined, 2, 0, function (result) {
  console.log("in js code:", 'connect1 return val is ' + result);
});

console.log("in js code:", 'continute');
