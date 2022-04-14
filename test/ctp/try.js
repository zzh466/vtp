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

// var ctp1_TradeAddress = "tcp://180.168.146.187:10201";
// var m_BrokerId = "9999";
// var m_UserId = "136380";
// var m_InvestorId = "136380";
// var m_PassWord = "jpf000jpf";
// var m_TradingDay = "20210805";
// var m_AccountId = "136380";
// var m_CurrencyId = "CNY";
// var m_AppId = "simnow_client_test";
// var m_AuthCode = "0000000000000000";

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
  login = true;
  // trader.reqQrySettlementInfo(m_BrokerId, m_AccountId, function (result, iRequestID) {
  //   console.log('settlementinfo return val is ' + result);
  // });

  trader.reqUserLogout(m_BrokerId, m_UserId, function (field) {
      console.log('reqUserLogout is callback');
      console.log(field);
  })

  // var instrumentID = "";
  //   trader.reqQryInstrument(instrumentID, function (field) {
  //     console.log('reqQryInstrument is callback');
  //     console.log(field);
  //   })
});

trader.on('rspUserLogout', function (requestId, isLast, field, info) {
  console.log("rspUserLogout: requestId", requestId);
  console.log("rspUserLogout: isLast", isLast);
  console.log("rspUserLogout: field", JSON.stringify(field));
  console.log('rspUserLogout callback');
})

var t_bInsertOrder = false;
trader.on('rqSettlementInfo', function (requestId, isLast, field, info) {
  console.log("rqSettlementInfo: requestId", requestId);
  console.log("rqSettlementInfo: isLast", isLast);
  console.log("rqSettlementInfo: field", JSON.stringify(field));
  console.log('rqsettlementinfo callback');

  // if (!t_bInsertOrder) {
  //   t_bInsertOrder = true;

  //   var instrumentID = "j2201";
  //   var orderRef = "1632460886";
  //   var direction = "0";
  //   var combOffsetFlag = "0";
  //   var limitPrice = "4840.1999999"
  //   var volumeTotalOriginal = "1";
  //   var requestID = "110";
  //   var exchangeID = "DCE";
  //   var insertOrder = {
  //     "BrokerID": m_BrokerId,
  //     "InvestorID": m_InvestorId,
  //     "InstrumentID": instrumentID,
  //     "OrderRef": orderRef,
  //     "UserID": m_UserId,
  //     //"OrderPriceType": "",
  //     "Direction": direction,
  //     "CombOffsetFlag": combOffsetFlag,
  //     //"CombHedgeFlag": "",
  //     "LimitPrice": limitPrice,
  //     "VolumeTotalOriginal": volumeTotalOriginal,
  //     //"TimeCondition": "",
  //     //"GTDDate": "",
  //     //"VolumeCondition": "",
  //     //"MinVolume": "",
  //     //"ContingentCondition": "",
  //     //"StopPrice": "",
  //     //"ForceCloseReason": "",
  //     //"IsAutoSuspend": "",
  //     //"BusinessUnit": "",
  //     "RequestID": requestID,
  //     //"UserForceClose": "",
  //     //"IsSwapOrder": "",
  //     "ExchangeID": exchangeID,
  //     "InvestUnitID": "",
  //     "AccountID": "",
  //     "CurrencyID": "",
  //     "ClientID": "",
  //     "IPAddress": "",
  //     "MacAddress": "",
  //  };

    // trader.reqOrderInsert(insertOrder, function (field) {
    //   console.log('ReqOrderInsert is callback');
    //   console.log(field);
    // })
  //}
});

// trader.on('rtnOrder', function (field) {
//   console.log('rtnOrder');
//   console.log(field);
// });

// trader.on('rtnTrade', function (field) {
//   console.log('rtnTrade');
//   console.log(field);
// })

trader.on('rspError', function (requestId, isLast, field) {
  console.log(JSON.stringify(field));
});

// trader.on('rqInstrument', function (requestId, isLast, field, info) {
//   console.log('rqInstrument');
//   console.log(JSON.stringify(requestId));
//   console.log(JSON.stringify(isLast));
//   console.log("rqInstrument: field", JSON.stringify(field));
//   console.log("rqInstrument: info", JSON.stringify(info));
// })

trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
  console.log("in js code:", 'connect return val is ' + result);
});

console.log("in js code:", 'continute');
