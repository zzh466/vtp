#include <node.h>
#include <nan.h>
#include <cstring>
#include "wrap_trader_qdp.h"

Persistent<Function> WrapTrader::constructor;
int WrapTrader::s_uuid;
std::map<std::string, int> WrapTrader::event_map;
std::map<int, Persistent<Function>> WrapTrader::callback_map;
std::map<int, Persistent<Function>> WrapTrader::fun_rtncb_map;

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;

char* utf8ToGBK(char* strUTF) {
  int size;
  size = MultiByteToWideChar(CP_UTF8,0,strUTF,-1,NULL,0); 
  wchar_t* strUnicode = new wchar_t[size];
  MultiByteToWideChar (CP_UTF8,0,strUTF,-1,strUnicode,size);

  size= WideCharToMultiByte(CP_ACP,0,strUnicode,-1,NULL,0,NULL,NULL);
  char *strGBK = new char[size];
  WideCharToMultiByte(CP_ACP,0,strUnicode,-1,strGBK,size,NULL,NULL); 
  delete []strUnicode;
  return strGBK;
}

char* gbkToUTF8(char* strGBK) {
    int size;
    size = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
    wchar_t* strUnicode = new wchar_t[size];
    MultiByteToWideChar(CP_ACP, 0, strGBK, -1, strUnicode, size);

    size = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char *strUTF = new char[size];
    WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUTF, size, NULL, NULL);
    delete []strUnicode;
    return strUTF;
}

void logger_cout(const char *content)
{
    using namespace std;
    if (islog)
    {
        cout << content << endl;
    }
}

WrapTrader::WrapTrader(void)
{
    logger_cout("wrap_trader------>object start init");
    uvTrader = new uv_trader();
    logger_cout("wrap_trader------>object init successed");
}

WrapTrader::~WrapTrader(void)
{
    if (uvTrader)
    {
        delete uvTrader;
    }
    logger_cout("wrap_trader------>object destroyed");
}

void WrapTrader::Init(Isolate *isolate)
{
    // Prepare constructor template
    Local<Context> context = isolate->GetCurrentContext();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "WrapTrader").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getTradingDay", GetTradingDay);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
    NODE_SET_PROTOTYPE_METHOD(tpl, "on", On);
    NODE_SET_PROTOTYPE_METHOD(tpl, "disposed", Disposed);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqUserLogin", ReqUserLogin);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqAuthenticate", ReqAuthenticate);

    NODE_SET_PROTOTYPE_METHOD(tpl, "reqUserLogout", ReqUserLogout);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqQryInstrument", ReqQryInstrument);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqQryTradingAccount", ReqQryTradingAccount);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqOrderInsert", ReqOrderInsert);
    NODE_SET_PROTOTYPE_METHOD(tpl, "reqOrderAction", ReqOrderAction);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqQryDepthMarketData", ReqQryDepthMarketData);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqQrySettlementInfo", ReqQrySettlementInfo);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqSettlementInfoConfirm", ReqSettlementInfoConfirm);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqQryInstrumentCommissionRate", ReqQryInstrumentCommissionRate);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqQryInvestorPositionDetail", ReqQryInvestorPositionDetail);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqQrySettlementInfoConfirm", ReqQrySettlementInfoConfirm);

    constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
}

void WrapTrader::New(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    if (event_map.size() == 0)
        initEventMap();

    if (args.IsConstructCall())
    {
        // Invoked as constructor: `new MyObject(...)`
        WrapTrader *wTrader = new WrapTrader();
        wTrader->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    }
    else
    {
        // Invoked as plain function `MyObject(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = {Number::New(isolate, 0)};
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<Context> context = isolate->GetCurrentContext();
        Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
        args.GetReturnValue().Set(instance);
    }
}

void WrapTrader::NewInstance(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    const unsigned argc = 1;
    Local<Value> argv[argc] = {Number::New(isolate, 0)};
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(instance);
}

void WrapTrader::initEventMap()
{
    event_map["connect"] = T_ON_CONNECT;
    event_map["disconnected"] = T_ON_DISCONNECTED;
    event_map["rspAuthenticate"] = T_ON_RSPAUTHENTICATE;
    event_map["rspUserLogin"] = T_ON_RSPUSERLOGIN;
    event_map["rspUserLogout"] = T_ON_RSPUSERLOGOUT;
    event_map["rspInfoConfirm"] = T_ON_RSPINFOCONFIRM;
    event_map["rspInsert"] = T_ON_RSPINSERT;
    event_map["errInsert"] = T_ON_ERRINSERT;
    event_map["rspAction"] = T_ON_RSPACTION;
    event_map["errAction"] = T_ON_ERRACTION;
    event_map["rqOrder"] = T_ON_RQORDER;
    event_map["rtnOrder"] = T_ON_RTNORDER;
    event_map["rqTrade"] = T_ON_RQTRADE;
    event_map["rtnTrade"] = T_ON_RTNTRADE;
    event_map["rqInvestorPosition"] = T_ON_RQINVESTORPOSITION;
    event_map["rqInvestorPositionDetail"] = T_ON_RQINVESTORPOSITIONDETAIL;
    event_map["rqTradingAccount"] = T_ON_RQTRADINGACCOUNT;
    event_map["rqInstrument"] = T_ON_RQINSTRUMENT;
    event_map["rqDdpthmarketData"] = T_ON_RQDEPTHMARKETDATA;
    event_map["rqSettlementInfo"] = T_ON_RQSETTLEMENTINFO;
    event_map["rSettlementInfoConfirm"] = T_ON_RSETTLEMENTINFOCONFIRM;
    event_map["rqInstrumentCommissionRate"] = T_ON_RQINSTRUMENTCOMMISSIONRATE;
    event_map["rqSettlementInfoConfirm"] = T_ON_RQSETTLEMENTINFOCONFIRM;
    event_map["rspError"] = T_ON_RSPERROR;
    event_map["rtnErrorConditionalOrder"] = T_ON_RTNERRORCONDITIONALORDER;
}

void WrapTrader::GetTradingDay(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    WrapTrader *wTrader = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    const char *tradingDay = wTrader->uvTrader->GetTradingDay();
    logger_cout("TradingDay is :");
    logger_cout(tradingDay);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, tradingDay).ToLocalChecked());
}

void WrapTrader::On(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        logger_cout("Wrong arguments->event name or function");
        isolate->ThrowException(
            Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->event name or function").ToLocalChecked()));
        return;
    }

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());

    Local<String> eventName = args[0]->ToString(context).ToLocalChecked();
    Local<Function> cb = Local<Function>::Cast(args[1]);

    String::Utf8Value eNameAscii(isolate, eventName);

    std::map<std::string, int>::iterator eIt = event_map.find((std::string)*eNameAscii);
    if (eIt == event_map.end())
    {
        logger_cout("System has not register this event");
        isolate->ThrowException(
            Exception::TypeError(String::NewFromUtf8(isolate, "System has no register this event").ToLocalChecked()));
        return;
    }

    std::map<int, Persistent<Function>>::iterator
        cIt = callback_map.find(eIt->second);
    if (cIt != callback_map.end())
    {
        logger_cout("Callback is defined before");
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Callback is defined before").ToLocalChecked()));
        return;
    }

    callback_map[eIt->second].Reset(isolate, cb);
    obj->uvTrader->On(*eNameAscii, eIt->second, FunCallback);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec on").ToLocalChecked());
}

void WrapTrader::Connect(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args[0]->IsUndefined())
    {
        logger_cout("Wrong arguments->front addr");
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->front addr").ToLocalChecked()));
        return;
    }
    if (!args[2]->IsNumber() || !args[3]->IsNumber())
    {
        logger_cout("Wrong arguments->public or private topic type");
        isolate->ThrowException(
            Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->public or private topic type").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[4]->IsUndefined() && args[4]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[4]));
        logger_cout(to_string(uuid).append("|uuid").c_str());
    }

    Local<String> frontAddr = args[0]->ToString(context).ToLocalChecked();
    Local<String> szPath = args[1]->IsUndefined() ? String::NewFromUtf8(isolate, "t").ToLocalChecked() : args[0]->ToString(context).ToLocalChecked();
    String::Utf8Value addrUtf8(isolate, frontAddr);
    String::Utf8Value pathUtf8(isolate, szPath);
    int publicTopicType = args[2].As<Number>()->Value();
    int privateTopicType = args[3].As<Number>()->Value();

    UVConnectField pConnectField;
    memset(&pConnectField, 0, sizeof(pConnectField));
    strcpy(pConnectField.front_addr, ((std::string)*addrUtf8).c_str());
    strcpy(pConnectField.szPath, ((std::string)*pathUtf8).c_str());
    pConnectField.public_topic_type = publicTopicType;
    pConnectField.private_topic_type = privateTopicType;
    logger_cout(((std::string)*addrUtf8).append("|addrUtf8").c_str());
    logger_cout(((std::string)*pathUtf8).append("|pathUtf8").c_str());
    logger_cout(to_string(publicTopicType).append("|publicTopicType").c_str());
    logger_cout(to_string(privateTopicType).append("|privateTopicType").c_str());
    obj->uvTrader->Connect(&pConnectField, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec connect").ToLocalChecked());
}

void WrapTrader::ReqAuthenticate(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    std::string log = "wrap_trader ReqAuthenticate------>";
    if (args[0]->IsUndefined() || args[1]->IsUndefined() || args[2]->IsUndefined() || args[3]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    String::Utf8Value brokerUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value userIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());
    String::Utf8Value authCodeUtf8(isolate, args[2]->ToString(context).ToLocalChecked());
    String::Utf8Value appIDUtf8(isolate, args[3]->ToString(context).ToLocalChecked());

    CQdpFtdcAuthenticateField req;
    strcpy_s(req.BrokerID, ((std::string)*brokerUtf8).c_str());
    strcpy_s(req.UserID, ((std::string)*userIDUtf8).c_str());
    strcpy_s(req.AuthCode, ((std::string)*authCodeUtf8).c_str());
    strcpy_s(req.AppID, ((std::string)*appIDUtf8).c_str());

    int uuid = -1;
    if (!args[4]->IsUndefined() && args[4]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[4]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->Authenticate(&req, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec reqAuthenticate").ToLocalChecked());
}

void WrapTrader::ReqUserLogin(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    std::string log = "wrap_trader ReqUserLogin------>";
    if (args[0]->IsUndefined() || args[1]->IsUndefined() || args[2]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[3]->IsUndefined() && args[3]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[3]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<String> broker = args[0]->ToString(context).ToLocalChecked();
    Local<String> userId = args[1]->ToString(context).ToLocalChecked();
    Local<String> pwd = args[2]->ToString(context).ToLocalChecked();
    String::Utf8Value brokerUtf8(isolate, broker);
    String::Utf8Value userIdUtf8(isolate, userId);
    String::Utf8Value pwdUtf8(isolate, pwd);

    CQdpFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, ((std::string)*brokerUtf8).c_str());
    strcpy(req.UserID, ((std::string)*userIdUtf8).c_str());
    strcpy(req.Password, ((std::string)*pwdUtf8).c_str());
    logger_cout(
        log.append(" ").append((std::string)*brokerUtf8).append("|").append((std::string)*userIdUtf8).append("|").append((std::string)*pwdUtf8).c_str());
    obj->uvTrader->ReqUserLogin(&req, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec reqUserlogin").ToLocalChecked());
}

void WrapTrader::ReqUserLogout(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqUserLogout------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[2]->IsUndefined() && args[2]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[2]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<String> broker = args[0]->ToString(context).ToLocalChecked();
    Local<String> userId = args[1]->ToString(context).ToLocalChecked();
    String::Utf8Value brokerAscii(isolate, broker);
    String::Utf8Value userIdAscii(isolate, userId);

    CQdpFtdcReqUserLogoutField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, ((std::string)*brokerAscii).c_str());
    strcpy(req.UserID, ((std::string)*userIdAscii).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerAscii).append("|").append((std::string)*userIdAscii).c_str());
    obj->uvTrader->ReqUserLogout(&req, FunRtnCallback, uuid);
    return;
}

void WrapTrader::ReqQryInstrument(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQryInstrument------>";

    if (args[0]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.This());
    if (!args[1]->IsUndefined() && args[1]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[1]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<String> instrumentId = args[0]->ToString(context).ToLocalChecked();
    String::Utf8Value instrumentIdAscii(isolate, instrumentId);

    CQdpFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, ((std::string)*instrumentIdAscii).c_str());
    logger_cout(log.append(" ").append((std::string)*instrumentIdAscii).c_str());
    obj->uvTrader->ReqQryInstrument(&req, FunRtnCallback, uuid);
    return;
}

void WrapTrader::ReqQryTradingAccount(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    //std::string log = "wrap_trader ReqQryTradingAccount------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        //std::string _head = std::string(log);
        //logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[2]->IsUndefined() && args[2]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[2]));
        //std::string _head = std::string(log);
        //logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    Local<String> broker = args[0]->ToString(context).ToLocalChecked();
    Local<String> investorId = args[1]->ToString(context).ToLocalChecked();
    String::Utf8Value brokerAscii(isolate, broker);
    String::Utf8Value investorIdAscii(isolate, investorId);

    CQdpFtdcQryInvestorAccountField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, ((std::string)*brokerAscii).c_str());
    // strcpy(req.UserID, ((std::string)*investorIdAscii).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIdAscii).c_str());
    //logger_cout(log.append(" ").append((std::string)*brokerAscii).append("|").append((std::string)*investorIdAscii).c_str());
    obj->uvTrader->ReqQryInvestorAccount(&req, FunRtnCallback, uuid);
    return;
}

void WrapTrader::ReqOrderInsert(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqOrderInsert------>";

    if (args[0]->IsUndefined() || !args[0]->IsObject())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[1]->IsUndefined() && args[1]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[1]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    Local<Object> jsonObj = args[0]->ToObject(context).ToLocalChecked();
    Local<Value> brokerId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked()).ToLocalChecked();
    if (brokerId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->BrokerID").ToLocalChecked()));
        return;
    }
    String::Utf8Value brokerId_(isolate, brokerId->ToString(context).ToLocalChecked());
    Local<Value> investorId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked()).ToLocalChecked();
    if (investorId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->InvestorID").ToLocalChecked()));
        return;
    }
    String::Utf8Value investorId_(isolate, investorId->ToString(context).ToLocalChecked());
    Local<Value> userId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "UserID").ToLocalChecked()).ToLocalChecked();
    if (userId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->UserID").ToLocalChecked()));
        return;
    }
    String::Utf8Value userId_(isolate, userId->ToString(context).ToLocalChecked());
    Local<Value> instrumentId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked()).ToLocalChecked();
    if (instrumentId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->InstrumentID").ToLocalChecked()));
        return;
    }
    String::Utf8Value instrumentId_(isolate, instrumentId->ToString(context).ToLocalChecked());
    /*Local<Value> priceType = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "priceType").ToLocalChecked()).ToLocalChecked();
    if (priceType->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->priceType").ToLocalChecked()));
        return;
    }
    String::Utf8Value priceType_(isolate, priceType->ToString(context).ToLocalChecked());*/
    Local<Value> direction = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "Direction").ToLocalChecked()).ToLocalChecked();
    if (direction->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->Direction").ToLocalChecked()));
        return;
    }
    String::Utf8Value direction_(isolate, direction->ToString(context).ToLocalChecked());
    Local<Value> combOffsetFlag = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "CombOffsetFlag").ToLocalChecked()).ToLocalChecked();
    if (combOffsetFlag->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->CombOffsetFlag").ToLocalChecked()));
        return;
    }
    String::Utf8Value combOffsetFlag_(isolate, combOffsetFlag->ToString(context).ToLocalChecked());
    /*Local<Value> combHedgeFlag = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "combHedgeFlag").ToLocalChecked()).ToLocalChecked();
    if (combHedgeFlag->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->combHedgeFlag").ToLocalChecked()));
        return;
    }
    String::Utf8Value combHedgeFlag_(isolate, combHedgeFlag->ToString(context).ToLocalChecked());*/
    Local<Value> vlimitPrice = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked()).ToLocalChecked();
    if (vlimitPrice->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->LimitPrice").ToLocalChecked()));
        return;
    }
    double limitPrice = vlimitPrice.As<Number>()->Value();
    Local<Value> vvolumeTotalOriginal = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "VolumeTotalOriginal").ToLocalChecked()).ToLocalChecked();
    if (vvolumeTotalOriginal->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->VolumeTotalOriginal").ToLocalChecked()));
        return;
    }
    int32_t volumeTotalOriginal = vvolumeTotalOriginal.As<Number>()->Value();
    /*Local<Value> timeCondition = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "timeCondition").ToLocalChecked()).ToLocalChecked();
    if (timeCondition->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->timeCondition").ToLocalChecked()));
        return;
    }
    String::Utf8Value timeCondition_(isolate, timeCondition->ToString(context).ToLocalChecked());*/
    /*Local<Value> volumeCondition = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "volumeCondition").ToLocalChecked()).ToLocalChecked();
    if (volumeCondition->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->volumeCondition").ToLocalChecked()));
        return;
    }
    String::Utf8Value volumeCondition_(isolate, volumeCondition->ToString(context).ToLocalChecked());*/
    /*Local<Value> vminVolume = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "minVolume").ToLocalChecked()).ToLocalChecked();
    if (vminVolume->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->minVolume").ToLocalChecked()));
        return;
    }
    int32_t minVolume = vminVolume.As<Number>()->Value();*/
    /*Local<Value> forceCloseReason = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "forceCloseReason").ToLocalChecked()).ToLocalChecked();
    if (forceCloseReason->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->forceCloseReason").ToLocalChecked()));
        return;
    }
    String::Utf8Value forceCloseReason_(isolate, forceCloseReason->ToString(context).ToLocalChecked());*/
    /*Local<Value> visAutoSuspend = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "isAutoSuspend").ToLocalChecked()).ToLocalChecked();
    if (visAutoSuspend->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->isAutoSuspend").ToLocalChecked()));
        return;
    }
    int32_t isAutoSuspend = visAutoSuspend.As<Number>()->Value();*/
    /*Local<Value> vuserForceClose = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "userForceClose").ToLocalChecked()).ToLocalChecked();
    if (vuserForceClose->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->userForceClose").ToLocalChecked()));
        return;
    }
    int32_t userForceClose = vuserForceClose.As<Number>()->Value();*/

    CQdpFtdcInputOrderField req;
    memset(&req, 0, sizeof(req));
    log.append(" ");

    // Local<Value> orderRef = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "OrderRef").ToLocalChecked()).ToLocalChecked();
    // if (!orderRef->IsUndefined())
    // {
    //     String::Utf8Value orderRef_(isolate, orderRef->ToString(context).ToLocalChecked());
    //     strcpy(req.OrderRef, ((std::string)*orderRef_).c_str());
    //     log.append("orderRef:").append((std::string)*orderRef_).append("|");
    // }
    /* Local<Value> contingentCondition = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "ContingentCondition").ToLocalChecked()).ToLocalChecked();
    if (!contingentCondition->IsUndefined())
    {
        String::Utf8Value contingentCondition_(isolate, contingentCondition->ToString(context).ToLocalChecked());
        req.ContingentCondition = ((std::string)*contingentCondition_)[0];
        // log.append("contingentCondition:").append(req.ContingentCondition).append("|");

        Local<Value> vStopPrice = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked()).ToLocalChecked();
        if (!vStopPrice->IsUndefined())
        {
            req.StopPrice = vStopPrice.As<Number>()->Value();
        } else {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->StopPrice").ToLocalChecked()));
            return;
        }
    } else {
        req.ContingentCondition = THOST_FTDC_CC_Immediately;
    } */
    Local<Value> timeCondition = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked()).ToLocalChecked();
    if (!timeCondition->IsUndefined())
    {
        String::Utf8Value timeCondition_(isolate, timeCondition->ToString(context).ToLocalChecked());
        req.TimeCondition = ((std::string)*timeCondition_)[0];
        // log.append("timeCondition:").append(((std::string)*timeCondition_)[0]).append("|");
    } else {
        req.TimeCondition = QDP_FTDC_TC_GFD;
    }
    Local<Value> orderPriceType = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked()).ToLocalChecked();
    if (!orderPriceType->IsUndefined())
    {
        String::Utf8Value orderPriceType_(isolate, orderPriceType->ToString(context).ToLocalChecked());
        req.OrderPriceType = ((std::string)*orderPriceType_)[0];
        // log.append("orderPriceType:").append(((std::string)*orderPriceType_)[0]).append("|");
    } else {
        req.OrderPriceType = QDP_FTDC_OPT_LimitPrice;
    }
    strcpy(req.BrokerID, ((std::string)*brokerId_).c_str());
    strcpy(req.InvestorID, ((std::string)*investorId_).c_str());
    strcpy(req.UserID, ((std::string)*userId_).c_str());
    strcpy(req.InstrumentID, ((std::string)*instrumentId_).c_str());
    req.Direction = ((std::string)*direction_)[0];
    // req.CombOffsetFlag[0] = ((std::string)*combOffsetFlag_)[0];
    // memset(req.CombHedgeFlag, 0, sizeof(req.CombHedgeFlag));
    /* req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; */
    //req.CombHedgeFlag[0] = ((std::string)*combHedgeFlag_)[0];
    req.LimitPrice = limitPrice;
    // req.VolumeTotalOriginal = volumeTotalOriginal;
    memset(req.GTDDate, 0, sizeof(req.GTDDate));
    req.VolumeCondition = QDP_FTDC_VC_AV;
    //req.VolumeCondition = ((std::string)*volumeCondition_)[0];
    req.MinVolume = 1;
    
    // memset(&(req.StopPrice), 0, sizeof(req.StopPrice));
    req.ForceCloseReason = QDP_FTDC_FCR_NotForceClose;
    req.IsAutoSuspend = 0;
    memset(req.BusinessUnit, 0, sizeof(req.BusinessUnit));
    // req.UserForceClose = 0;
    // req.IsSwapOrder = 0;
    //logger_cout(log.append("brokerID:").append((std::string)*brokerId_).append("|").append("investorID:").append((std::string)*investorId_).append("|").append("instrumentID:").append((std::string)*instrumentId_).append("|").append("direction:").append((std::string)*direction_).append("|").append("comboffsetFlag:").append((std::string)*combOffsetFlag_).append("|").append("limitPrice:").append(to_string(limitPrice)).append("|").append("volumnTotalOriginal:").append(to_string(volumeTotalOriginal)).append("|").c_str());
    obj->uvTrader->ReqOrderInsert(&req, FunRtnCallback, uuid);
    return;
}

void WrapTrader::ReqOrderAction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqOrderAction------>";

    if (args[0]->IsUndefined() || !args[0]->IsObject())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());

    if (!args[1]->IsUndefined() && args[1]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[1]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<Object> jsonObj = args[0]->ToObject(context).ToLocalChecked();
    Local<Value> vbrokerId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked()).ToLocalChecked();
    if (vbrokerId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->BrokerID").ToLocalChecked()));
        return;
    }
    String::Utf8Value brokerId_(isolate, vbrokerId->ToString(context).ToLocalChecked());
    Local<Value> vUserId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "UserID").ToLocalChecked()).ToLocalChecked();
    if (vUserId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->InvestorID").ToLocalChecked()));
        return;
    }
    String::Utf8Value userId_(isolate, vUserId->ToString(context).ToLocalChecked());
    Local<Value> vinvestorId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked()).ToLocalChecked();
    if (vinvestorId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->InvestorID").ToLocalChecked()));
        return;
    }
    String::Utf8Value investorId_(isolate, vinvestorId->ToString(context).ToLocalChecked());
    Local<Value> vinstrumentId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked()).ToLocalChecked();
    if (vinstrumentId->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->InstrumentID").ToLocalChecked()));
        return;
    }
    String::Utf8Value instrumentId_(isolate, vinstrumentId->ToString(context).ToLocalChecked());
    Local<Value> vactionFlag = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "ActionFlag").ToLocalChecked()).ToLocalChecked();
    if (vactionFlag->IsUndefined())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->actionFlag").ToLocalChecked()));
        return;
    }
    String::Utf8Value actionFlag_(isolate, vactionFlag->ToString(context).ToLocalChecked());

    CQdpFtdcOrderActionField req;
    memset(&req, 0, sizeof(req));

    log.append(" ");
    Local<Value> vorderRef = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "OrderRef").ToLocalChecked()).ToLocalChecked();
    // if (!vorderRef->IsUndefined())
    // {
    //     String::Utf8Value orderRef_(isolate, vorderRef->ToString(context).ToLocalChecked());
    //     strcpy(req.OrderRef, ((std::string)*orderRef_).c_str());
    //     log.append((std::string)*orderRef_).append("|");
    // }
    Local<Value> vfrontId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "FrontID").ToLocalChecked()).ToLocalChecked();
    if (!vfrontId->IsUndefined())
    {
        int32_t frontId = vfrontId.As<Number>()->Value();
        req.FrontID = frontId;
        log.append(to_string(frontId)).append("|");
    }
    Local<Value> vsessionId = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "SessionID").ToLocalChecked()).ToLocalChecked();
    if (!vsessionId->IsUndefined())
    {
        int32_t sessionId = vsessionId.As<Number>()->Value();
        req.SessionID = sessionId;
        log.append(to_string(sessionId)).append("|");
    }
    Local<Value> vexchangeID = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked()).ToLocalChecked();
    if (!vexchangeID->IsUndefined())
    {
        String::Utf8Value exchangeID_(isolate, vexchangeID->ToString(context).ToLocalChecked());
        strcpy(req.ExchangeID, ((std::string)*exchangeID_).c_str());
        log.append((std::string)*exchangeID_).append("|");
    }
    Local<Value> vorderSysID = jsonObj->Get(context, v8::String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked()).ToLocalChecked();
    if (!vorderSysID->IsUndefined())
    {
        String::Utf8Value orderSysID_(isolate, vorderSysID->ToString(context).ToLocalChecked());
        strcpy(req.OrderSysID, ((std::string)*orderSysID_).c_str());
        log.append((std::string)*orderSysID_).append("|");
    }

    strcpy(req.BrokerID, ((std::string)*brokerId_).c_str());
    strcpy(req.UserID, ((std::string)*userId_).c_str());
    strcpy(req.InvestorID, ((std::string)*investorId_).c_str());
    req.ActionFlag = ((std::string)*actionFlag_)[0];
    strcpy(req.InstrumentID, ((std::string)*instrumentId_).c_str());
    logger_cout(log.append((std::string)*brokerId_).append("|").append((std::string)*investorId_).append("|").append((std::string)*instrumentId_).append("|").append((std::string)*actionFlag_).append("|").c_str());

    obj->uvTrader->ReqOrderAction(&req, FunRtnCallback, uuid);
    return;
}

/* void WrapTrader::ReqQryDepthMarketData(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQryDepthMarketData------>";

    if (args[0]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    if (!args[1]->IsUndefined() && args[1]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[1]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<String> instrumentId = args[0]->ToString(context).ToLocalChecked();
    String::Utf8Value instrumentIdAscii(isolate, instrumentId);

    CThostFtdcQryDepthMarketDataField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.InstrumentID, ((std::string)*instrumentIdAscii).c_str());
    logger_cout(log.append(" ").append((std::string)*instrumentIdAscii).append("|").c_str());
    obj->uvTrader->ReqQryDepthMarketData(&req, FunRtnCallback, uuid);
    return;
} */

/* void WrapTrader::ReqQrySettlementInfo(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQrySettlementInfo------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    int funIndex = 2;
    if (!args[funIndex]->IsUndefined() && args[funIndex]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[funIndex]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    String::Utf8Value brokerIDUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value investorIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());

    CThostFtdcQrySettlementInfoField req = {0};
    strcpy(req.BrokerID, ((std::string)*brokerIDUtf8).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIDUtf8).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerIDUtf8).append("|").append(" ").append((std::string)*investorIDUtf8).append("|").c_str());

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->ReqQrySettlementInfo(&req, FunRtnCallback, uuid);
    return;
} */

/* void WrapTrader::ReqSettlementInfoConfirm(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqSettlementInfoConfirm------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    int funIndex = 2;
    if (!args[funIndex]->IsUndefined() && args[funIndex]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[funIndex]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    String::Utf8Value brokerIDUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value investorIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());

    CThostFtdcSettlementInfoConfirmField req = {0};
    strcpy(req.BrokerID, ((std::string)*brokerIDUtf8).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIDUtf8).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerIDUtf8).append("|").append(" ").append((std::string)*investorIDUtf8).append("|").c_str());

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->ReqSettlementInfoConfirm(&req, FunRtnCallback, uuid);
    return;
} */

/* void WrapTrader::ReqQryInstrumentCommissionRate(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQryInstrumentCommissionRate------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    int funIndex = 3;
    if (!args[funIndex]->IsUndefined() && args[funIndex]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[funIndex]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    String::Utf8Value brokerIDUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value investorIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());
    String::Utf8Value instrumentIdUtf8(isolate, args[2]->ToString(context).ToLocalChecked());

    CThostFtdcQryInstrumentCommissionRateField req = {0};
    strcpy(req.BrokerID, ((std::string)*brokerIDUtf8).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIDUtf8).c_str());
    strcpy(req.InstrumentID, ((std::string)*instrumentIdUtf8).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerIDUtf8).append("|").append(" ").append((std::string)*investorIDUtf8).append("|")
    .append(" ").append(req.InstrumentID).append("|").c_str());

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->ReqQryInstrumentCommissionRate(&req, FunRtnCallback, uuid);
    return;
} */

/* void WrapTrader::ReqQryInvestorPositionDetail(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQryInvestorPositionDetail------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    int funIndex = 2;
    if (!args[funIndex]->IsUndefined() && args[funIndex]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[funIndex]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    String::Utf8Value brokerIDUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value investorIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());

    CThostFtdcQryInvestorPositionDetailField req = {0};
    strcpy(req.BrokerID, ((std::string)*brokerIDUtf8).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIDUtf8).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerIDUtf8).append("|").append(" ").append((std::string)*investorIDUtf8).append("|").c_str());

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->ReqQryInvestorPositionDetail(&req, FunRtnCallback, uuid);
    return;
} */

/* void WrapTrader::ReqQrySettlementInfoConfirm(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_trader ReqQryInvestorPositionDetail------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    int funIndex = 2;
    if (!args[funIndex]->IsUndefined() && args[funIndex]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[funIndex]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }
    String::Utf8Value brokerIDUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    String::Utf8Value investorIDUtf8(isolate, args[1]->ToString(context).ToLocalChecked());

    CThostFtdcQrySettlementInfoConfirmField req = {0};
    strcpy(req.BrokerID, ((std::string)*brokerIDUtf8).c_str());
    strcpy(req.InvestorID, ((std::string)*investorIDUtf8).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerIDUtf8).append("|").append(" ").append((std::string)*investorIDUtf8).append("|").c_str());

    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->ReqQrySettlementInfoConfirm(&req, FunRtnCallback, uuid);
    return;
} */

void WrapTrader::Disposed(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    WrapTrader *obj = ObjectWrap::Unwrap<WrapTrader>(args.Holder());
    obj->uvTrader->Disconnect();
    // std::map<int, Persistent<Function>>::iterator
    //     callback_it = callback_map.begin();
    // while (callback_it != callback_map.end())
    // {
    //     //        callback_it->second.Dispose();
    //     callback_it++;
    // }
    
    //event_map.clear();
    //callback_map.clear();
    //fun_rtncb_map.clear();
    // delete obj->uvTrader;
    // obj->uvTrader = NULL;
    logger_cout("wrap_trader Disposed------>wrap disposed");
    return;
}

void WrapTrader::FunCallback(CbRtnField *data)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    std::map<int, Persistent<Function>>::iterator cIt = callback_map.find(data->eFlag);
    if (cIt == callback_map.end())
        return;

    switch (data->eFlag)
    {
        case T_ON_CONNECT:
        {
            Local<Value> argv[1] = {Undefined(isolate)};
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);
            break;
        }
        case T_ON_DISCONNECTED:
        {
            Local<Value> argv[1] = {Integer::New(isolate, data->nReason)};
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);
            break;
        }
        case T_ON_RSPAUTHENTICATE:
        {
            Local<Value> argv[1] = {Integer::New(isolate, data->nReason)};
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);
            break;
        }
        case T_ON_RSPUSERLOGIN:
        {
            Local<Value> argv[4];
            pkg_cb_userlogin(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
        case T_ON_RSPUSERLOGOUT:
        {
            Local<Value> argv[4];
            pkg_cb_userlogout(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
            //        case T_ON_RSPINFOCONFIRM: {
            //            Local <Value> argv[4];
            //            pkg_cb_confirm(data, argv);
            //            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            //			fn->Call(isolate->GetCurrentContext()->Global(), 4, argv);
            //            break;
            //        }
        case T_ON_RSPINSERT:
        {
            Local<Value> argv[4];
            pkg_cb_orderinsert(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
        case T_ON_ERRINSERT:
        {
            Local<Value> argv[2];
            pkg_cb_errorderinsert(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 2, argv);
            break;
        }
        case T_ON_RSPACTION:
        {
            Local<Value> argv[4];
            pkg_cb_orderaction(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
        case T_ON_ERRACTION:
        {
            Local<Value> argv[2];
            pkg_cb_errorderaction(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 2, argv);

            break;
        }
        case T_ON_RQORDER:
        {
            Local<Value> argv[4];
            pkg_cb_rspqryorder(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
        case T_ON_RTNORDER:
        {
            Local<Value> argv[1];
            pkg_cb_rtnorder(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);

            break;
        }
        case T_ON_RQTRADE:
        {
            Local<Value> argv[4];
            pkg_cb_rqtrade(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);

            break;
        }
        case T_ON_RTNTRADE:
        {
            Local<Value> argv[1];
            pkg_cb_rtntrade(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);

            break;
        }
            //        case T_ON_RQINVESTORPOSITION: {
            //            Local <Value> argv[4];
            //            pkg_cb_rqinvestorposition(data, argv);
            //            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            //			fn->Call(isolate->GetCurrentContext()->Global(), 4, argv);
            //
            //            break;
            //        }
        /* case T_ON_RQINVESTORPOSITIONDETAIL:
        {
            Local <Value> argv[4];
            pkg_cb_rqinvestorpositiondetail(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        case T_ON_RQTRADINGACCOUNT:
        {
            Local<Value> argv[4];
            pkg_cb_rqtradingaccount(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);

            break;
        }
        case T_ON_RQINSTRUMENT:
        {
            Local<Value> argv[4];
            pkg_cb_rqinstrument(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);

            break;
        }
        /* case T_ON_RQDEPTHMARKETDATA:
        {
            Local<Value> argv[4];
            pkg_cb_rqdepthmarketdata(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        /* case T_ON_RQSETTLEMENTINFO:
        {
            Local<Value> argv[4];
            pkg_cb_rqsettlementinfo(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        case T_ON_RSPERROR:
        {
            Local<Value> argv[3];
            pkg_cb_rsperror(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 3, argv);

            break;
        }
        /* case T_ON_RSETTLEMENTINFOCONFIRM:
        {
            Local<Value> argv[4];
            pkg_cb_rsettlementinfoconfirm(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        /* case T_ON_RQINSTRUMENTCOMMISSIONRATE:
        {
            Local<Value> argv[4];
            pkg_cb_rqinstrumentcommissionrate(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        /* case T_ON_RQSETTLEMENTINFOCONFIRM:
        {
            Local<Value> argv[4];
            pkg_cb_rqsettlementinfoconfirm(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        } */
        /* case T_ON_RTNERRORCONDITIONALORDER:
        {
            Local<Value> argv[1];
            pkg_cb_rtnerrorconditionalorder(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);
            break;
        } */
    }
}

void WrapTrader::FunRtnCallback(int result, void *baton)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    LookupQdpApiBaton *tmp = static_cast<LookupQdpApiBaton *>(baton);
    if (tmp->uuid != -1)
    {
        std::map<int, Persistent<Function>>::iterator
        it = fun_rtncb_map.find(tmp->uuid);

        const unsigned argc = 2;
        Local<Value> argv[argc] = {Integer::New(isolate, tmp->nResult), Integer::New(isolate, tmp->iRequestID)};

        Local<Function> fn = Local<Function>::New(isolate, it->second);
        fn->Call(context, isolate->GetCurrentContext()->Global(), argc, argv);
        it->second.Reset();
        fun_rtncb_map.erase(tmp->uuid);
    }
}

void WrapTrader::pkg_cb_userlogin(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField != NULL)
    {
        CQdpFtdcRspUserLoginField *pRspUserLogin = static_cast<CQdpFtdcRspUserLoginField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);

        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LoginTime").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->LoginTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MaxOrderLocalID").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->MaxOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingSystemName").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->TradingSystemName).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "DataCenterID").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->DataCenterID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PrivateFlowSize").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->PrivateFlowSize));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserFlowSize").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->UserFlowSize));             
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pRspUserLogin->SessionID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->FrontID));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "SHFETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->SHFETime).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "DCETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->DCETime).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "CZCETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->CZCETime).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "FFEXTime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->FFEXTime).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "INETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->INETime).ToLocalChecked());

        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Object::New(isolate);
    }

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_userlogout(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField)
    {
        CQdpFtdcReqUserLogoutField *pRspUserLogin = static_cast<CQdpFtdcReqUserLogoutField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, pRspUserLogin->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, pRspUserLogin->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LogoutReason").ToLocalChecked(),
                    Number::New(isolate, pRspUserLogin->LogoutReason));
        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Object::New(isolate);
    }

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_orderinsert(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    if (data->rtnField)
    {
        CQdpFtdcInputOrderField *pInputOrder = static_cast<CQdpFtdcInputOrderField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pInputOrder->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->OrderPriceType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->Direction).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pInputOrder->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Number::New(isolate, pInputOrder->Volume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->TimeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "GTDDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->GTDDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeCondition").ToLocalChecked(),
                     String::NewFromUtf8(isolate, charto_string(pInputOrder->VolumeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinVolume").ToLocalChecked(), Number::New(isolate, pInputOrder->MinVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked(), Number::New(isolate, pInputOrder->StopPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ForceCloseReason").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->ForceCloseReason).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsAutoSuspend").ToLocalChecked(), Number::New(isolate, pInputOrder->IsAutoSuspend));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessUnit").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->BusinessUnit).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserCustom").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->UserCustom).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BranchID").ToLocalChecked(), Number::New(isolate, pInputOrder->BranchID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pInputOrder->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessType").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->BusinessType).c_str()).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderRef").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->OrderRef).ToLocalChecked());
        //var charval = String.fromCharCode(asciival);
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "CombOffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->CombOffsetFlag).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "CombHedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->CombHedgeFlag).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTotalOriginal").ToLocalChecked(), Number::New(isolate, pInputOrder->VolumeTotalOriginal));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "ContingentCondition").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, charto_string(pInputOrder->ContingentCondition).c_str()).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "RequestID").ToLocalChecked(), Number::New(isolate, pInputOrder->RequestID));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserForceClose").ToLocalChecked(), Number::New(isolate, pInputOrder->UserForceClose));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsSwapOrder").ToLocalChecked(), Number::New(isolate, pInputOrder->IsSwapOrder));
        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_errorderinsert(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField)
    {
        CQdpFtdcInputOrderField *pInputOrder = static_cast<CQdpFtdcInputOrderField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pInputOrder->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->OrderPriceType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->Direction).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), 
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pInputOrder->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Number::New(isolate, pInputOrder->Volume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->TimeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "GTDDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->GTDDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeCondition").ToLocalChecked(),
                     String::NewFromUtf8(isolate, charto_string(pInputOrder->VolumeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinVolume").ToLocalChecked(), Number::New(isolate, pInputOrder->MinVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked(), Number::New(isolate, pInputOrder->StopPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ForceCloseReason").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->ForceCloseReason).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsAutoSuspend").ToLocalChecked(), Number::New(isolate, pInputOrder->IsAutoSuspend));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessUnit").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->BusinessUnit).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserCustom").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrder->UserCustom).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BranchID").ToLocalChecked(), Number::New(isolate, pInputOrder->BranchID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pInputOrder->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessType").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrder->BusinessType).c_str()).ToLocalChecked());
        *cbArray = jsonRtn;
    }
    else
    {
        *cbArray = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 1) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_orderaction(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    if (data->rtnField)
    {
        CQdpFtdcOrderActionField *pInputOrderAction = static_cast<CQdpFtdcOrderActionField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInputOrderAction->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderActionLocalID").ToLocalChecked(), Number::New(isolate, pInputOrderAction->UserOrderActionLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pInputOrderAction->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActionFlag").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pInputOrderAction->ActionFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pInputOrderAction->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeChange").ToLocalChecked(), Number::New(isolate, pInputOrderAction->VolumeChange));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pInputOrderAction->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(), Number::New(isolate, pInputOrderAction->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pInputOrderAction->SessionID));
        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_errorderaction(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField)
    {
        CQdpFtdcOrderActionField *pOrderAction = static_cast<CQdpFtdcOrderActionField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrderAction->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderActionLocalID").ToLocalChecked(), Number::New(isolate, pOrderAction->UserOrderActionLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pOrderAction->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActionFlag").ToLocalChecked(),
                    String::NewFromUtf8(isolate, charto_string(pOrderAction->ActionFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pOrderAction->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeChange").ToLocalChecked(), Number::New(isolate, pOrderAction->VolumeChange));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pOrderAction->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(), Number::New(isolate, pOrderAction->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pOrderAction->SessionID));
        *cbArray = jsonRtn;
    }
    else
    {
        *cbArray = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 1) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_rspqryorder(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    if (data->rtnField)
    {
        CQdpFtdcOrderField *pOrder = static_cast<CQdpFtdcOrderField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pOrder->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OrderPriceType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->Direction).c_str()).ToLocalChecked()); //var charval = String.fromCharCode(asciival);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pOrder->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Number::New(isolate, pOrder->Volume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinVolume").ToLocalChecked(), Number::New(isolate, pOrder->MinVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->TimeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "GTDDate").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->GTDDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->VolumeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked(), Number::New(isolate, pOrder->StopPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ForceCloseReason").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->ForceCloseReason).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsAutoSuspend").ToLocalChecked(), Number::New(isolate, pOrder->IsAutoSuspend));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessUnit").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->BusinessUnit).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ParticipantID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ParticipantID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClientID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ClientID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SeatID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->SeatID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InsertTime").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InsertTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderLocalID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->OrderLocalID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSource").ToLocalChecked(), Number::New(isolate, pOrder->OrderSource));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderStatus").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OrderStatus).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CancelTime").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->CancelTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CancelUserID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->CancelUserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTraded").ToLocalChecked(), Number::New(isolate, pOrder->VolumeTraded));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeRemain").ToLocalChecked(), Number::New(isolate, pOrder->VolumeRemain));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(), Number::New(isolate, pOrder->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pOrder->SessionID));
        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_rtnorder(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CQdpFtdcOrderField *pOrder = static_cast<CQdpFtdcOrderField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pOrder->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OrderPriceType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->Direction).c_str()).ToLocalChecked()); //var charval = String.fromCharCode(asciival);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pOrder->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Number::New(isolate, pOrder->Volume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinVolume").ToLocalChecked(), Number::New(isolate, pOrder->MinVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->TimeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "GTDDate").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->GTDDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->VolumeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked(), Number::New(isolate, pOrder->StopPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ForceCloseReason").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->ForceCloseReason).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsAutoSuspend").ToLocalChecked(), Number::New(isolate, pOrder->IsAutoSuspend));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessUnit").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->BusinessUnit).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ParticipantID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ParticipantID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClientID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->ClientID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SeatID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->SeatID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InsertTime").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->InsertTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderLocalID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->OrderLocalID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSource").ToLocalChecked(), Number::New(isolate, pOrder->OrderSource));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderStatus").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pOrder->OrderStatus).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CancelTime").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->CancelTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CancelUserID").ToLocalChecked(), String::NewFromUtf8(isolate, pOrder->CancelUserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTraded").ToLocalChecked(), Number::New(isolate, pOrder->VolumeTraded));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeRemain").ToLocalChecked(), Number::New(isolate, pOrder->VolumeRemain));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(), Number::New(isolate, pOrder->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pOrder->SessionID));
    }
    else {
       jsonRtn = Object::New(isolate);
    }

    *cbArray = jsonRtn;
    return;
}

void WrapTrader::pkg_cb_rqtrade(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CQdpFtdcTradeField *pTrade = static_cast<CQdpFtdcTradeField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ParticipantID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ParticipantID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SeatID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->SeatID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClientID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClientID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pTrade->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->Direction).c_str()).ToLocalChecked()); //var charval = String.fromCharCode(asciival);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradePrice").ToLocalChecked(), Number::New(isolate, pTrade->TradePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeVolume").ToLocalChecked(), Number::New(isolate, pTrade->TradeVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradeTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClearingPartID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClearingPartID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeAmnt").ToLocalChecked(), Number::New(isolate, pTrade->TradeAmnt));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pTrade->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->TradeType).c_str()).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_rtntrade(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CQdpFtdcTradeField *pTrade = static_cast<CQdpFtdcTradeField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ParticipantID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ParticipantID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SeatID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->SeatID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClientID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClientID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserOrderLocalID").ToLocalChecked(), Number::New(isolate, pTrade->UserOrderLocalID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->Direction).c_str()).ToLocalChecked()); //var charval = String.fromCharCode(asciival);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OffsetFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->HedgeFlag).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradePrice").ToLocalChecked(), Number::New(isolate, pTrade->TradePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeVolume").ToLocalChecked(), Number::New(isolate, pTrade->TradeVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradeTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClearingPartID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClearingPartID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeAmnt").ToLocalChecked(), Number::New(isolate, pTrade->TradeAmnt));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RecNum").ToLocalChecked(), Number::New(isolate, pTrade->RecNum));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->TradeType).c_str()).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }

    *cbArray = jsonRtn;
    return;
}

void WrapTrader::pkg_cb_rqtradingaccount(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CQdpFtdcRspInvestorAccountField *pTradingAccount = static_cast<CQdpFtdcRspInvestorAccountField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pTradingAccount->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pTradingAccount->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AccountID").ToLocalChecked(), String::NewFromUtf8(isolate, pTradingAccount->AccountID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreBalance").ToLocalChecked(), Number::New(isolate, pTradingAccount->PreBalance));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreAvailable").ToLocalChecked(), Number::New(isolate, pTradingAccount->Available));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Deposit").ToLocalChecked(), Number::New(isolate, pTradingAccount->Deposit));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Withdraw").ToLocalChecked(), Number::New(isolate, pTradingAccount->Withdraw));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Margin").ToLocalChecked(), Number::New(isolate, pTradingAccount->Margin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Premium").ToLocalChecked(), Number::New(isolate, pTradingAccount->Premium));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Fee").ToLocalChecked(), Number::New(isolate, pTradingAccount->Fee));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrozenMargin").ToLocalChecked(), Number::New(isolate, pTradingAccount->FrozenMargin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrozenPremium").ToLocalChecked(), Number::New(isolate, pTradingAccount->FrozenPremium));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrozenFee").ToLocalChecked(), Number::New(isolate, pTradingAccount->FrozenFee));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseProfit").ToLocalChecked(), Number::New(isolate, pTradingAccount->CloseProfit));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PositionProfit").ToLocalChecked(), Number::New(isolate, pTradingAccount->PositionProfit));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Available").ToLocalChecked(), Number::New(isolate, pTradingAccount->Available));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Balance").ToLocalChecked(), Number::New(isolate, pTradingAccount->Balance));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LongMargin").ToLocalChecked(), Number::New(isolate, pTradingAccount->LongMargin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ShortMargin").ToLocalChecked(), Number::New(isolate, pTradingAccount->ShortMargin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LongFrozenMargin").ToLocalChecked(), Number::New(isolate, pTradingAccount->LongFrozenMargin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ShortFrozenMargin").ToLocalChecked(), Number::New(isolate, pTradingAccount->ShortFrozenMargin));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "DynamicRights").ToLocalChecked(), Number::New(isolate, pTradingAccount->DynamicRights));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Risk").ToLocalChecked(), Number::New(isolate, pTradingAccount->Risk));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OtherFee").ToLocalChecked(), Number::New(isolate, pTradingAccount->OtherFee));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Mortgage").ToLocalChecked(), Number::New(isolate, pTradingAccount->Mortgage));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Currency").ToLocalChecked(), String::NewFromUtf8(isolate, pTradingAccount->Currency).ToLocalChecked());
        *(cbArray + 2) = jsonRtn;
    }
    else {
       jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}

void WrapTrader::pkg_cb_rqinstrument(CbRtnField *data, Local<Value> *cbArray) {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField != NULL){
        CQdpFtdcRspInstrumentField *pInstrument = static_cast<CQdpFtdcRspInstrumentField*>(data->rtnField);
        jsonRtn = Object::New(isolate);
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->ExchangeID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "ProductID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->ProductID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ProductName").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->ProductName).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentName").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pInstrument->InstrumentName)).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "DeliveryYear").ToLocalChecked(), Number::New(isolate, pInstrument->DeliveryYear));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "DeliveryMonth").ToLocalChecked(), Number::New(isolate, pInstrument->DeliveryMonth));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MaxLimitOrderVolume").ToLocalChecked(), Number::New(isolate, pInstrument->MaxLimitOrderVolume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinLimitOrderVolume").ToLocalChecked(), Number::New(isolate, pInstrument->MinLimitOrderVolume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "MaxMarketOrderVolume").ToLocalChecked(), Number::New(isolate, pInstrument->MaxMarketOrderVolume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinMarketOrderVolume").ToLocalChecked(), Number::New(isolate, pInstrument->MinMarketOrderVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeMultiple").ToLocalChecked(), Number::New(isolate, pInstrument->VolumeMultiple));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "PriceTick").ToLocalChecked(), Number::New(isolate, pInstrument->PriceTick));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Currency").ToLocalChecked(), Number::New(isolate, pInstrument->Currency));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LongPosLimit").ToLocalChecked(), Number::New(isolate, pInstrument->LongPosLimit));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ShortPosLimit").ToLocalChecked(), Number::New(isolate, pInstrument->ShortPosLimit));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LowerLimitPrice").ToLocalChecked(), Number::New(isolate, pInstrument->LowerLimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UpperLimitPrice").ToLocalChecked(), Number::New(isolate, pInstrument->UpperLimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreSettlementPrice").ToLocalChecked(), Number::New(isolate, pInstrument->PreSettlementPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentStatus").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrument->InstrumentStatus).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CreateDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->CreateDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->OpenDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExpireDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->ExpireDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "StartDelivDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->StartDelivDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "EndDelivDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->EndDelivDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "BasisPrice").ToLocalChecked(), Number::New(isolate, pInstrument->BasisPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsTrading").ToLocalChecked(), Number::New(isolate, pInstrument->IsTrading));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UnderlyingInstrID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrument->UnderlyingInstrID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UnderlyingMultiple").ToLocalChecked(), Number::New(isolate, pInstrument->UnderlyingMultiple));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PositionType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrument->PositionType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StrikePrice").ToLocalChecked(), Number::New(isolate, pInstrument->StrikePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OptionsType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrument->OptionsType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ProductClass").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrument->ProductClass).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OptionsMode").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrument->OptionsMode).c_str()).ToLocalChecked());
	}
    
	*cbArray = Number::New(isolate, data->nRequestID);
	*(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
	return;
}

/* void WrapTrader::pkg_cb_rqdepthmarketdata(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    if (data->rtnField)
    {
        CThostFtdcDepthMarketDataField *pDepthMarketData = static_cast<CThostFtdcDepthMarketDataField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeInstID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->ExchangeInstID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LastPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->LastPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreSettlementPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->PreSettlementPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreClosePrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->PreClosePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreOpenInterest").ToLocalChecked(), Number::New(isolate, pDepthMarketData->PreOpenInterest));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->OpenPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "HighestPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->HighestPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LowestPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->LowestPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Number::New(isolate, pDepthMarketData->Volume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Turnover").ToLocalChecked(), Number::New(isolate, pDepthMarketData->Turnover));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenInterest").ToLocalChecked(), Number::New(isolate, pDepthMarketData->OpenInterest));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClosePrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->ClosePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->SettlementPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UpperLimitPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->UpperLimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LowerLimitPrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->LowerLimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "PreDelta").ToLocalChecked(), Number::New(isolate, pDepthMarketData->PreDelta));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CurrDelta").ToLocalChecked(), Number::New(isolate, pDepthMarketData->CurrDelta));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UpdateTime").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->UpdateTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UpdateMillisec").ToLocalChecked(), Number::New(isolate, pDepthMarketData->UpdateMillisec));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidPrice1").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidPrice1));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidVolume1").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidVolume1));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskPrice1").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskPrice1));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskVolume1").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskVolume1));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidPrice2").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidPrice2));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidVolume2").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidVolume2));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskPrice2").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskPrice2));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskVolume2").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskVolume2));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidPrice3").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidPrice3));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidVolume3").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidVolume3));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskPrice3").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskPrice3));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskVolume3").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskVolume3));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidPrice4").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidPrice4));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidVolume4").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidVolume4));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskPrice4").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskPrice4));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskVolume4").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskVolume4));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidPrice5").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidPrice5));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BidVolume5").ToLocalChecked(), Number::New(isolate, pDepthMarketData->BidVolume5));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskPrice5").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskPrice5));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AskVolume5").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AskVolume5));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AveragePrice").ToLocalChecked(), Number::New(isolate, pDepthMarketData->AveragePrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActionDay").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->ActionDay).ToLocalChecked());
        *(cbArray + 2) = jsonRtn;
    }
    else
    {
        *(cbArray + 2) = Local<Value>::New(isolate, Undefined(isolate));
    }
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

/* void WrapTrader::pkg_cb_rqsettlementinfo(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField != NULL)
    {
        CThostFtdcSettlementInfoField *pSettlementInfo = static_cast<CThostFtdcSettlementInfoField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfo->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementID").ToLocalChecked(), Int32::New(isolate, pSettlementInfo->SettlementID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfo->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfo->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SequenceNo").ToLocalChecked(), Int32::New(isolate, pSettlementInfo->SequenceNo));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Content").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pSettlementInfo->Content)).ToLocalChecked());
    }

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

/* void WrapTrader::pkg_cb_rsettlementinfoconfirm(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm = static_cast<CThostFtdcSettlementInfoConfirmField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ConfirmDate").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->ConfirmDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ConfirmTime").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->ConfirmTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementID").ToLocalChecked(), Number::New(isolate, pSettlementInfoConfirm->SettlementID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AccountID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->AccountID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CurrencyID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->CurrencyID).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Int32::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

/* void WrapTrader::pkg_cb_rqinstrumentcommissionrate(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate = static_cast<CThostFtdcInstrumentCommissionRateField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrumentCommissionRate->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorRange").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->InvestorRange));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrumentCommissionRate->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrumentCommissionRate->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenRatioByMoney").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->OpenRatioByMoney));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenRatioByVolume").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->OpenRatioByVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseRatioByMoney").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->CloseRatioByMoney));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseRatioByVolume").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->CloseRatioByVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseTodayRatioByMoney").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->CloseTodayRatioByMoney));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseTodayRatioByVolume").ToLocalChecked(), Number::New(isolate, pInstrumentCommissionRate->CloseTodayRatioByVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrumentCommissionRate->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BizType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInstrumentCommissionRate->BizType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestUnitID").ToLocalChecked(), String::NewFromUtf8(isolate, pInstrumentCommissionRate->InvestUnitID).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

/* void WrapTrader::pkg_cb_rqinvestorpositiondetail(CbRtnField* data, Local<Value>*cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
	    CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail = static_cast<CThostFtdcInvestorPositionDetailField*>(data->rtnField);
		jsonRtn = Object::New(isolate);
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->InstrumentID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->BrokerID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->InvestorID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "HedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInvestorPositionDetail->HedgeFlag).c_str()).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInvestorPositionDetail->Direction).c_str()).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenDate").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->OpenDate).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->TradeID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "Volume").ToLocalChecked(), Int32::New(isolate, pInvestorPositionDetail->Volume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "OpenPrice").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->OpenPrice));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->TradingDay).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementID").ToLocalChecked(), Int32::New(isolate, pInvestorPositionDetail->SettlementID));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradeType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pInvestorPositionDetail->TradeType).c_str()).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "CombInstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->CombInstrumentID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pInvestorPositionDetail->ExchangeID).ToLocalChecked());
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseProfitByDate").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->CloseProfitByDate));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseProfitByTrade").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->CloseProfitByTrade));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "PositionProfitByDate").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->PositionProfitByDate));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "PositionProfitByTrade").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->PositionProfitByTrade));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "Margin").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->Margin));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchMargin").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->ExchMargin));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "MarginRateByMoney").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->MarginRateByMoney));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "MarginRateByVolume").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->MarginRateByVolume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "LastSettlementPrice").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->LastSettlementPrice));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementPrice").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->SettlementPrice));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseVolume").ToLocalChecked(), Int32::New(isolate, pInvestorPositionDetail->CloseVolume));
		jsonRtn->Set(context, String::NewFromUtf8(isolate, "CloseAmount").ToLocalChecked(), Number::New(isolate, pInvestorPositionDetail->CloseAmount));
	} else {  
		jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

/* void WrapTrader::pkg_cb_rqsettlementinfoconfirm(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm = static_cast<CThostFtdcSettlementInfoConfirmField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ConfirmDate").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->ConfirmDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ConfirmTime").ToLocalChecked(), String::NewFromUtf8(isolate, pSettlementInfoConfirm->ConfirmTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementID").ToLocalChecked(), Number::New(isolate, pSettlementInfoConfirm->SettlementID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AccountID").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pSettlementInfoConfirm->AccountID)).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CurrencyID").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pSettlementInfoConfirm->CurrencyID)).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }
    
    *cbArray = Int32::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = jsonRtn;
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
} */

void WrapTrader::pkg_cb_rsperror(CbRtnField *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = pkg_rspinfo(data->rspInfo);
    return;
}

/* void WrapTrader::pkg_cb_rtnerrorconditionalorder(CbRtnField *data, Local<Value> *cbArray) 
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CThostFtdcErrorConditionalOrderField *pTrade = static_cast<CThostFtdcErrorConditionalOrderField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestorID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InvestorID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "reserve1").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->reserve1).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderRef").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->OrderRef).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderPriceType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OrderPriceType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "Direction").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->Direction).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CombOffsetFlag").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->CombOffsetFlag).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CombHedgeFlag").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->CombHedgeFlag).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LimitPrice").ToLocalChecked(), Number::New(isolate, pTrade->LimitPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTotalOriginal").ToLocalChecked(), Number::New(isolate, pTrade->VolumeTotalOriginal));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TimeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->TimeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "GTDDate").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->GTDDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->VolumeCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MinVolume").ToLocalChecked(), Number::New(isolate, pTrade->MinVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ContingentCondition").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->ContingentCondition).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StopPrice").ToLocalChecked(), Number::New(isolate, pTrade->StopPrice));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ForceCloseReason").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->ForceCloseReason).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsAutoSuspend").ToLocalChecked(), Number::New(isolate, pTrade->IsAutoSuspend));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BusinessUnit").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->BusinessUnit).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RequestID").ToLocalChecked(), Number::New(isolate, pTrade->RequestID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderLocalID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->OrderLocalID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ExchangeID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ParticipantID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ParticipantID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClientID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClientID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "reserve2").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->reserve2).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TraderID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TraderID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstallID").ToLocalChecked(), Number::New(isolate, pTrade->InstallID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSubmitStatus").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OrderSubmitStatus).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "NotifySequence").ToLocalChecked(), Number::New(isolate, pTrade->NotifySequence));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SettlementID").ToLocalChecked(), Number::New(isolate, pTrade->SettlementID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->OrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderSource").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OrderSource).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderStatus").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OrderStatus).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "OrderType").ToLocalChecked(), String::NewFromUtf8(isolate, charto_string(pTrade->OrderType).c_str()).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTraded").ToLocalChecked(), Number::New(isolate, pTrade->VolumeTraded));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "VolumeTotal").ToLocalChecked(), Number::New(isolate, pTrade->VolumeTotal));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InsertDate").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InsertDate).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InsertTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InsertTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActiveTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ActiveTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SuspendTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->SuspendTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UpdateTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->UpdateTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CancelTime").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->CancelTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActiveTraderID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ActiveTraderID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ClearingPartID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ClearingPartID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SequenceNo").ToLocalChecked(), Number::New(isolate, pTrade->SequenceNo));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(), Number::New(isolate, pTrade->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pTrade->SessionID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserProductInfo").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->UserProductInfo).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "StatusMsg").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->StatusMsg).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserForceClose").ToLocalChecked(), Number::New(isolate, pTrade->UserForceClose));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ActiveUserID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ActiveUserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerOrderSeq").ToLocalChecked(), Number::New(isolate, pTrade->BrokerOrderSeq));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "RelativeOrderSysID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->RelativeOrderSysID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ZCETotalTradedVolume").ToLocalChecked(), Number::New(isolate, pTrade->ZCETotalTradedVolume));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ErrorID").ToLocalChecked(), Number::New(isolate, pTrade->ErrorID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ErrorMsg").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ErrorMsg).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IsSwapOrder").ToLocalChecked(), Number::New(isolate, pTrade->IsSwapOrder));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BranchID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->BranchID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InvestUnitID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InvestUnitID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "AccountID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->AccountID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CurrencyID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->CurrencyID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "reserve3").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->reserve3).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MacAddress").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->MacAddress).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeInstID").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->ExchangeInstID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "IPAddress").ToLocalChecked(), String::NewFromUtf8(isolate, pTrade->IPAddress).ToLocalChecked());
    }
    else {
       jsonRtn = Object::New(isolate);
    }

    *cbArray = jsonRtn;
    return;
} */

Local<Value> WrapTrader::pkg_rspinfo(void *vpRspInfo)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (vpRspInfo)
    {
        CQdpFtdcRspInfoField *pRspInfo = static_cast<CQdpFtdcRspInfoField *>(vpRspInfo);
        Local<Object> jsonInfo = Object::New(isolate);
        jsonInfo->Set(context, String::NewFromUtf8(isolate, "ErrorID").ToLocalChecked(), Number::New(isolate, pRspInfo->ErrorID));
        jsonInfo->Set(context, String::NewFromUtf8(isolate, "ErrorMsg").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pRspInfo->ErrorMsg)).ToLocalChecked());
        return jsonInfo;
    }
    else
    {
        return Local<Value>::New(isolate, Undefined(isolate));
    }
}
