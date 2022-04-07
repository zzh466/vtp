var ctp = require('../../build/Debug/ctp.node');
ctp.settings({ log: true });

// simnow hanzhe
var ctp1_TradeAddress = "tcp://180.168.146.187:10202";
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

// simnow yy
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

  var instrumentID = "j2205";
  var exchangeID = "DCE";
  // var instrumentID = "IC2112";
  // var exchangeID = "CFFEX";
  var orderRef = "1632460888";
  // �������� TFtdcDirectionType
  ///�� #define THOST_FTDC_D_Buy '0'
  ///�� #define THOST_FTDC_D_Sell '1'
  var direction = "1";
  // ƽ����� TFtdcOffsetFlagType
  ///���� #define THOST_FTDC_OF_Open '0'
  ///ƽ�� #define THOST_FTDC_OF_Close '1'
  ///ǿƽ #define THOST_FTDC_OF_ForceClose '2'
  ///ƽ�� #define THOST_FTDC_OF_CloseToday '3'
  ///ƽ�� #define THOST_FTDC_OF_CloseYesterday '4'
  ///ǿ�� #define THOST_FTDC_OF_ForceOff '5'
  ///����ǿƽ #define THOST_FTDC_OF_LocalForceClose '6'
  var combOffsetFlag = "0";
  var limitPrice = 4075.0
  // ����
  var volumeTotalOriginal = 1;
  var requestID = "110";

  var contingentCondition = '5';
  var stopPrice = 4080.0;
  var orderPriceType = '2';
  var timeCondition = '3';

  var insertOrder = {
    "BrokerID": m_BrokerId,
    "InvestorID": m_InvestorId,
    "InstrumentID": instrumentID,
    "OrderRef": orderRef,
    "UserID": m_UserId,
    "OrderPriceType": orderPriceType,
    "Direction": direction,
    "CombOffsetFlag": combOffsetFlag,
    //"CombHedgeFlag": "",
    "LimitPrice": limitPrice,
    "VolumeTotalOriginal": volumeTotalOriginal,
    "TimeCondition": timeCondition,
    //"GTDDate": "",
    //"VolumeCondition": "",
    //"MinVolume": "",
    "ContingentCondition": contingentCondition,
    "StopPrice": stopPrice,
    //"ForceCloseReason": "",
    //"IsAutoSuspend": "",
    //"BusinessUnit": "",
    "RequestID": requestID,
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


  console.log("--------------------------------------");

  trader.reqOrderInsert(insertOrder, function (field) {
    console.log('ReqOrderInsert is callback');
    console.log(field);
  })
});

trader.on('rtnOrder', function (field) {
  console.log('rtnOrder');
  console.log(field);
});

trader.on('rtnTrade', function (field) {
  console.log('rtnTrade');
  console.log(field);
})

trader.on('rtnErrorConditionalOrder', function (field) {
  console.log('rtnErrorConditionalOrder');
  console.log(field);
})

trader.on('rspError', function (requestId, isLast, field) {
  console.log(JSON.stringify(field));
});

trader.connect(ctp1_TradeAddress, undefined, 2, 0, function (result) {
  console.log("in js code:", 'connect return val is ' + result);
});

console.log("in js code:", 'continute');
