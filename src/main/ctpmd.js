
var ctpmd = require('../../build/Release/ctpmd.node');
ctpmd.settings({ log: true });
var events = require('events');

var ctpMdUser = ctpmd.createMdUser();
class CtpMd {
    constructor(args){
       const emitter = new  events.EventEmitter();
       this.emitter = emitter
        this.arr = []
        const {
             brokerId,
        
          futureUserId,
        
          futureUserPwd,
            quotAddr
        } = args
        ctpMdUser.on("connect", function (result) {
            console.log("in js code: ----> on connected , result=", result);
            ctpMdUser.reqUserLogin(brokerId, futureUserId, futureUserPwd, function (result) {
                console.log("in js code: reqUserlogin result=", result);
            });
        });

        ctpMdUser.on("rspUserLogin", function (requestId, isLast, field, info) {
            emitter.emit('login');
            console.log("in js code: ----> rspUserLogin: requestId", requestId);
            console.log("in js code: ----> rspUserLogin: isLast", isLast);
            console.log("in js code: ----> rspUserLogin: field", JSON.stringify(field));
            console.log("in js code: ----> rspUserLogin: info", JSON.stringify(info));
           
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

        ctpMdUser.connect(quotAddr, undefined, 2, 0, function (result) {
            console.log("in js code:", 'connect return val is ' + result);
        });
    }
    subscribeMarketData(instrumentID){
        if(this.arr.includes(instrumentID))return;
        this.arr.push(instrumentID);
        ctpMdUser.subscribeMarketData(instrumentID, function (field) {
            console.log('rspSubMarketData is callback');
            console.log(field);
        })
    }
    on(event, fn){
         this.emitter.on(event, fn.bind(this));
    }
}
export default CtpMd