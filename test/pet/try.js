var pet = require('../../build/Debug/pet.node');
pet.settings({ log: true });

var HOST="pet.iudge.cn"
var PORT=9125
var USER="54522"
var PASS="Abcd@1234"
var VPRT="N234SIM01"
var MAC="B06EBF818B0C"

console.log("try.js starting");

var login = false;
var trader = pet.createTrader();

// trader.on("rspUserLogin", function (requestId, isLast, field, info) {
//   console.log("rspUserLogin: requestId", requestId);
//   console.log("rspUserLogin: isLast", isLast);
//   console.log("rspUserLogin: field", JSON.stringify(field));
//   console.log("rspUserLogin: info", JSON.stringify(info));
//   login = true;
// });

trader.connect(HOST, PORT, function (result) {
  console.log("in js code: reqUserlogin result=", result);
});

console.log("in js code:", 'continute');
