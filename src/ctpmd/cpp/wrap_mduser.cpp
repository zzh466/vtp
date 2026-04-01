#include <node.h>
#include <nan.h>
#include "wrap_mduser.h"
#include <cstring>

Persistent<Function> WrapMdUser::constructor;
int WrapMdUser::s_uuid;
std::map<std::string, int> WrapMdUser::event_map;
std::map<int, Persistent<Function>> WrapMdUser::callback_map;
std::map<int, Persistent<Function>> WrapMdUser::fun_rtncb_map;

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

WrapMdUser::WrapMdUser() {
	logger_cout("wrap_mduser------>object start init");
    uvMdUser = new uv_mduser();
    logger_cout("wrap_mduser------>object init successed");
}

WrapMdUser::~WrapMdUser() {
    if(uvMdUser) {
	    delete uvMdUser;
    }
	logger_cout("wrape_mduser------>object destroyed");
}

void WrapMdUser::Init(Isolate *isolate) {
	// Prepare constructor template
    Local<Context> context = isolate->GetCurrentContext();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "WrapMdUser").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

	NODE_SET_PROTOTYPE_METHOD(tpl, "on", On);
	NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
	NODE_SET_PROTOTYPE_METHOD(tpl, "reqUserLogin", ReqUserLogin);
	NODE_SET_PROTOTYPE_METHOD(tpl, "reqUserLogout", ReqUserLogout);
	NODE_SET_PROTOTYPE_METHOD(tpl, "subscribeMarketData", SubscribeMarketData);
	NODE_SET_PROTOTYPE_METHOD(tpl, "unSubscribeMarketData", UnSubscribeMarketData);
	NODE_SET_PROTOTYPE_METHOD(tpl, "disconnect", Disconnect);

	constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
}

void WrapMdUser::New(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    if (event_map.size() == 0)
        initEventMap();

    if (args.IsConstructCall())
    {
        // Invoked as constructor: `new MyObject(...)`
        WrapMdUser *wMdUser = new WrapMdUser();
        wMdUser->Wrap(args.This());
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

void WrapMdUser::NewInstance(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    const unsigned argc = 1;
    Local<Value> argv[argc] = {Number::New(isolate, 0)};
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(instance);
}

void WrapMdUser::initEventMap() {
	event_map["connect"] = T_ON_CONNECT;
	event_map["disconnect"] = T_ON_DISCONNECT;
	event_map["rspUserLogin"] = T_ON_RSPUSERLOGIN;
	event_map["rspUserLogout"] = T_ON_RSPUSERLOGOUT;
	event_map["rspSubMarketData"] = T_ON_RSPSUBMARKETDATA;
	event_map["rspUnSubMarketData"] = T_ON_RSPUNSUBMARKETDATA;
	event_map["rtnDepthMarketData"] = T_ON_RTNDEPTHMARKETDATA;
	event_map["rspError"] = T_ON_RSPERROR;
}

void WrapMdUser::GetTradingDay(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    WrapMdUser *wMdUser = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
    const char *tradingDay = wMdUser->uvMdUser->GetTradingDay();
    logger_cout("TradingDay is :");
    logger_cout(tradingDay);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, tradingDay).ToLocalChecked());
}

void WrapMdUser::On(const FunctionCallbackInfo<Value> &args)
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

    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());

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
    obj->uvMdUser->On(*eNameAscii, eIt->second, FunCallback);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec on").ToLocalChecked());
}

void WrapMdUser::Connect(const FunctionCallbackInfo<Value> &args)
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
    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
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
    obj->uvMdUser->Connect(&pConnectField, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec connect").ToLocalChecked());
}

void WrapMdUser::ReqUserLogin(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    std::string log = "wrap_mduser ReqUserLogin------>";
    if (args[0]->IsUndefined() || args[1]->IsUndefined() || args[2]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    int uuid = -1;
    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
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

    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    // strcpy(req.BrokerID, ((std::string)*brokerUtf8).c_str());
    strcpy(req.UserID, ((std::string)*userIdUtf8).c_str());
    strcpy(req.Password, ((std::string)*pwdUtf8).c_str());
    logger_cout(
        log.append(" ").append((std::string)*brokerUtf8).append("|").append((std::string)*userIdUtf8).append("|").append((std::string)*pwdUtf8).c_str());
    obj->uvMdUser->ReqUserLogin(&req, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec reqUserlogin").ToLocalChecked());
}

void WrapMdUser::ReqUserLogout(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_mduser ReqUserLogout------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
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

    CThostFtdcUserLogoutField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, ((std::string)*brokerAscii).c_str());
    strcpy(req.UserID, ((std::string)*userIdAscii).c_str());
    logger_cout(log.append(" ").append((std::string)*brokerAscii).append("|").append((std::string)*userIdAscii).c_str());
    obj->uvMdUser->ReqUserLogout(&req, FunRtnCallback, uuid);
    return;
}

void WrapMdUser::SubscribeMarketData(const FunctionCallbackInfo<Value> &args)
{
	Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    std::string log = "wrap_mduser SubscribeMarketData------>";

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        std::string _head = std::string(log);
        logger_cout(_head.append(" Wrong arguments").c_str());
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
    if (!args[1]->IsUndefined() && !(args[1]->IsFunction()))
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[1]));
        std::string _head = std::string(log);
        logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
    }

    Local<String> instrumentId = args[0]->ToString(context).ToLocalChecked();
    String::Utf8Value instrumentIdAscii(isolate, instrumentId);
	
    logger_cout(log.append(" ").c_str());

    char** idArray = new char*[1];
    idArray[0] = *instrumentIdAscii;
    obj->uvMdUser->SubscribeMarketData(idArray, 1, FunRtnCallback, uuid);
    delete idArray;
    return;
}

void WrapMdUser::UnSubscribeMarketData(const FunctionCallbackInfo<Value> &args) {

}

void WrapMdUser::Disconnect(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    WrapMdUser *obj = ObjectWrap::Unwrap<WrapMdUser>(args.Holder());
    obj->uvMdUser->Disconnect();
    logger_cout("wrap_mduser Disposed------>wrap disposed");
    return;
}

void WrapMdUser::FunCallback(CbRtnField *data) {
    std::string log = "jasonhan FunCallback";
    logger_cout(log.c_str());
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
		case T_ON_DISCONNECT:
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
		case T_ON_RSPSUBMARKETDATA:
		{
            std::string logx1 = "Jason uv_mduser FunCallback T_ON_RSPSUBMARKETDATA";
            logger_cout(logx1.c_str());
			Local<Value> argv[4];
            pkg_cb_rspsubmarketdata(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
		}
		case T_ON_RTNDEPTHMARKETDATA:
		{
			Local<Value> argv[1];
			pkg_cb_rtndepthmarketdata(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);
            break;
		}
		case T_ON_RSPERROR:
		{

		}
	}
}

void WrapMdUser::FunRtnCallback(int result, void* baton) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    LookupCtpApiBaton *tmp = static_cast<LookupCtpApiBaton *>(baton);
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
void WrapMdUser::pkg_cb_userlogin(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField != NULL)
    {
        CThostFtdcRspUserLoginField *pRspUserLogin = static_cast<CThostFtdcRspUserLoginField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);

        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "LoginTime").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->LoginTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->UserID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SystemName").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->SystemName).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(),
                     Number::New(isolate, pRspUserLogin->FrontID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pRspUserLogin->SessionID));
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "MaxOrderRef").ToLocalChecked(),
                     String::NewFromUtf8(isolate, pRspUserLogin->MaxOrderRef).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "SHFETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->SHFETime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "DCETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->DCETime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "CZCETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->CZCETime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "FFEXTime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->FFEXTime).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "INETime").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->INETime).ToLocalChecked());

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
void WrapMdUser::pkg_cb_userlogout(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField)
    {
        CThostFtdcUserLogoutField *pRspUserLogin = static_cast<CThostFtdcUserLogoutField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->BrokerID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(), String::NewFromUtf8(isolate, pRspUserLogin->UserID).ToLocalChecked());
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
void WrapMdUser::pkg_cb_rspsubmarketdata(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField != NULL) {
        std::string log = "jason wrap_mduser pkg_cb_rspsubmarketdata != null";
        logger_cout(log.c_str());

        CThostFtdcSpecificInstrumentField *pSpecificInstrument = static_cast<CThostFtdcSpecificInstrumentField*>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "reserve1").ToLocalChecked(), String::NewFromUtf8(isolate, pSpecificInstrument->reserve1).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pSpecificInstrument->InstrumentID).ToLocalChecked());   
        *(cbArray + 2) = jsonRtn;
    }
    else {
        std::string log = "jason wrap_mduser pkg_cb_rspsubmarketdata == null";
        logger_cout(log.c_str());

        *(cbArray + 2) = Object::New(isolate);
    }

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 3) = pkg_rspinfo(data->rspInfo);
    return;
}
void WrapMdUser::pkg_cb_unrspsubmarketdata(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);

    if (data->rtnField) {
        CThostFtdcSpecificInstrumentField *pSpecificInstrument = static_cast<CThostFtdcSpecificInstrumentField*>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pSpecificInstrument->InstrumentID).ToLocalChecked());
		*(cbArray + 2) = jsonRtn;
	}
	else {
		*(cbArray + 2) = Local<Value>::New(isolate, Undefined(isolate));
	}
	cbArray[3] = pkg_rspinfo(data->rspInfo);
}
void WrapMdUser::pkg_cb_rtndepthmarketdata(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    
    Local<Object> jsonRtn = Object::New(isolate);
    if (data->rtnField)
    {
        CThostFtdcDepthMarketDataField *pDepthMarketData = static_cast<CThostFtdcDepthMarketDataField *>(data->rtnField);
        jsonRtn = Object::New(isolate);
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->TradingDay).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->ExchangeID).ToLocalChecked());
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
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "InstrumentID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->InstrumentID).ToLocalChecked());
        jsonRtn->Set(context, String::NewFromUtf8(isolate, "ExchangeInstID").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->ExchangeInstID).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "BandingUpperPrice").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->BandingUpperPrice).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "BandingLowerPrice").ToLocalChecked(), String::NewFromUtf8(isolate, pDepthMarketData->BandingLowerPrice).ToLocalChecked());
    } else {
        jsonRtn = Object::New(isolate);
    }

    *cbArray = jsonRtn;
    return;
}  
void WrapMdUser::pkg_cb_rsperror(CbRtnField *data, Local<Value> *cbArray) {
	Isolate *isolate = Isolate::GetCurrent();

    *cbArray = Number::New(isolate, data->nRequestID);
    *(cbArray + 1) = Boolean::New(isolate, data->bIsLast);
    *(cbArray + 2) = pkg_rspinfo(data->rspInfo);
    return;
}
Local<Value> WrapMdUser::pkg_rspinfo(void *vpRspInfo) {
	Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (vpRspInfo)
    {
        CThostFtdcRspInfoField *pRspInfo = static_cast<CThostFtdcRspInfoField *>(vpRspInfo);
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
