#include <node.h>
#include <nan.h>
#include <cstring>

#include "uv_wrap_trader_pet.h"

Persistent<Function> UVWrapTraderPet::constructor;
int UVWrapTraderPet::s_uuid;
std::map<std::string, int> UVWrapTraderPet::event_map;
std::map<int, Persistent<Function>> UVWrapTraderPet::callback_map;
std::map<int, Persistent<Function>> UVWrapTraderPet::fun_rtncb_map;

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

UVWrapTraderPet::UVWrapTraderPet(void)
{
    logger_cout("wrap_trader------>object start init");
    uvTrader = new uv_trader_pet();
    logger_cout("wrap_trader------>object init successed");
}

UVWrapTraderPet::~UVWrapTraderPet(void)
{
    if (uvTrader)
    {
        delete uvTrader;
    }
    logger_cout("wrap_trader------>object destroyed");
}

void UVWrapTraderPet::Init(Isolate *isolate)
{
    // Prepare constructor template
    Local<Context> context = isolate->GetCurrentContext();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "UVWrapTraderPet").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
    // NODE_SET_PROTOTYPE_METHOD(tpl, "reqUserLogin", ReqUserLogin);

    constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
}

void UVWrapTraderPet::New(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();

    if (event_map.size() == 0)
        initEventMap();

    if (args.IsConstructCall())
    {
        // Invoked as constructor: `new MyObject(...)`
        UVWrapTraderPet *wTrader = new UVWrapTraderPet();
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

void UVWrapTraderPet::NewInstance(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    const unsigned argc = 1;
    Local<Value> argv[argc] = {Number::New(isolate, 0)};
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(instance);
}

void UVWrapTraderPet::initEventMap()
{
    event_map["rspUserLogin"] = T_ON_RSPUSERLOGIN;
    event_map["rspUserLogout"] = T_ON_RSPUSERLOGOUT;
}

void UVWrapTraderPet::Connect(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args[0]->IsUndefined() || args[1]->IsUndefined())
    {
        logger_cout("Wrong arguments->front addr");
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments->front addr").ToLocalChecked()));
        return;
    }
    int uuid = -1;
    UVWrapTraderPet *obj = ObjectWrap::Unwrap<UVWrapTraderPet>(args.Holder());
    if (!args[2]->IsUndefined() && !args[2]->IsFunction())
    {
        uuid = ++s_uuid;
        fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[4]));
        logger_cout(to_string(uuid).append("|uuid").c_str());
    }

    String::Utf8Value hostAddrUtf8(isolate, args[0]->ToString(context).ToLocalChecked());
    unsigned int post = args[1].As<Number>()->Value();

    UvPetRegisterFrontReqInfo registerForntReqInfo;
    memset(&registerForntReqInfo, 0, sizeof(registerForntReqInfo));
    strcpy(registerForntReqInfo.host, ((std::string)*hostAddrUtf8).c_str());
    registerForntReqInfo.post = post;
    logger_cout(((std::string)*hostAddrUtf8).append("|hostAddrUtf8").c_str());
    logger_cout(to_string(post).append("|post").c_str());
    obj->uvTrader->Connect(&registerForntReqInfo, FunRtnCallback, uuid);
    return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec connect").ToLocalChecked());
}

// void UVWrapTraderPet::ReqUserLogin(const FunctionCallbackInfo<Value> &args)
// {
//     Isolate *isolate = args.GetIsolate();
//     Local<Context> context = isolate->GetCurrentContext();

//     std::string log = "pet_wrap_trader ReqUserLogin------>";
//     if (args[0]->IsUndefined() || args[1]->IsUndefined() || args[2]->IsUndefined() || args[3]->IsUndefined())
//     {
//         std::string _head = std::string(log);
//         logger_cout(_head.append(" Wrong arguments").c_str());
//         isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
//         return;
//     }

//     int uuid = -1;
//     UVWrapTraderPet *obj = ObjectWrap::Unwrap<UVWrapTraderPet>(args.Holder());
//     if (!args[4]->IsUndefined() && args[4]->IsFunction())
//     {
//         uuid = ++s_uuid;
//         fun_rtncb_map[uuid].Reset(isolate, Local<Function>::Cast(args[4]));
//         std::string _head = std::string(log);
//         logger_cout(_head.append(" uuid is ").append(to_string(uuid)).c_str());
//     }

//     Local<String> userName = args[0]->ToString(context).ToLocalChecked();
//     Local<String> userPwd = args[1]->ToString(context).ToLocalChecked();
//     Local<String> vacctnumber = args[2]->ToString(context).ToLocalChecked();
//     Local<String> mac = args[3]->ToString(context).ToLocalChecked();
//     String::Utf8Value userNameUtf8(isolate, userName);
//     String::Utf8Value userPwdUtf8(isolate, userPwd);
//     String::Utf8Value vacctnumberUtf8(isolate, vacctnumber);
//     String::Utf8Value macUtf8(isolate, mac);

//     UvPetLoginReqInfo req;
//     memset(&req, 0, sizeof(req));
//     strcpy(req.user, ((std::string)*userNameUtf8).c_str());
//     strcpy(req.pwd, ((std::string)*userPwdUtf8).c_str());
//     strcpy(req.vprt, ((std::string)*vacctnumberUtf8).c_str());
//     strcpy(req.mac, ((std::string)*macUtf8).c_str());
//     logger_cout(
//         log.append(" ").append((std::string)*userNameUtf8).append("|").append((std::string)*userPwdUtf8).append("|").append((std::string)*vacctnumberUtf8).c_str());
//     obj->uvTrader->OnLogin(&req, FunRtnCallback, uuid);
//     return args.GetReturnValue().Set(String::NewFromUtf8(isolate, "finish exec reqUserlogin").ToLocalChecked());
// }

void UVWrapTraderPet::FunCallback(UVPetRtnInfo *data)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    std::map<int, Persistent<Function>>::iterator cIt = callback_map.find(data->eFlag);
    if (cIt == callback_map.end())
        return;

    switch (data->eFlag)
    {
        case T_ON_RSPUSERLOGIN:
        {
            Local<Value> argv[4];
            pkg_cb_userlogin(data, argv);
            Local<Function> fn = Local<Function>::New(isolate, cIt->second);
            fn->Call(context, isolate->GetCurrentContext()->Global(), 4, argv);
            break;
        }
        
    }
}

void UVWrapTraderPet::FunRtnCallback(int result, void *baton)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    LookupPetApiBaton *tmp = static_cast<LookupPetApiBaton *>(baton);
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

void UVWrapTraderPet::pkg_cb_userlogin(UVPetRtnInfo *data, Local<Value> *cbArray)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (data->rtnField != NULL)
    {
        // CThostFtdcRspUserLoginField *pRspUserLogin = static_cast<CThostFtdcRspUserLoginField *>(data->rtnField);
        Local<Object> jsonRtn = Object::New(isolate);

        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "TradingDay").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->TradingDay).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "LoginTime").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->LoginTime).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "BrokerID").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->BrokerID).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "UserID").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->UserID).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "SystemName").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->SystemName).ToLocalChecked());
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "FrontID").ToLocalChecked(),
        //              Number::New(isolate, pRspUserLogin->FrontID));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "SessionID").ToLocalChecked(), Number::New(isolate, pRspUserLogin->SessionID));
        // jsonRtn->Set(context, String::NewFromUtf8(isolate, "MaxOrderRef").ToLocalChecked(),
        //              String::NewFromUtf8(isolate, pRspUserLogin->MaxOrderRef).ToLocalChecked());
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

Local<Value> UVWrapTraderPet::pkg_rspinfo(void *vpRspInfo)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> jsonInfo = Object::New(isolate);
    if (vpRspInfo)
    {
        UVPetResqInfo *pRspInfo = static_cast<UVPetResqInfo *>(vpRspInfo);
        jsonInfo = Object::New(isolate);
        jsonInfo->Set(context, String::NewFromUtf8(isolate, "ErrorID").ToLocalChecked(), Number::New(isolate, pRspInfo->ErrorID));
        jsonInfo->Set(context, String::NewFromUtf8(isolate, "ErrorMsg").ToLocalChecked(), String::NewFromUtf8(isolate, gbkToUTF8(pRspInfo->ErrorMsg)).ToLocalChecked());
    }
    return jsonInfo;
}
