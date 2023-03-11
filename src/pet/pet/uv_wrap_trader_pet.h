#ifndef UV_WRAP_TRADER_PET_H
#define UV_WRAP_TRADER_PET_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <node.h>
#include <uv.h>
#include <node_object_wrap.h>

#include "interface/iudge_client_api.h"
#include "nlohmann/json.hpp"

#include "uv_trader_pet.h"
#include "uv_wraper_pet_struct.h"

// #define EventUrl_ServPushMsg "ClientCommentRequest"
// #define RequestUrl_PushConfirm "/external/confirmReceiveSubscribe"
// #define RequestUrl_PlaceOrder "/external/order/placeOrder"
// #define RequestUrl_CancelOrder "/external/order/cancelInstructOrder"
// #define RequestUrl_GetFund "/external/broker/getBrokeracct"
// #define RequestUrl_GetPosi "/external/broker/getBrokeracctstock"
// #define RequestUrl_GetOrders "/external/order/getOrder"
// #define RequestUrl_Subscribe "/external/subscribeSubject"

#define PORT 9125

#define HOST "pet.iudge.cn"
#define USER "50096"
#define PASS "251800da8d338eb82819105d5f3c7629"
#define VPRT "N166sim1"

using namespace v8;

extern bool islog;
extern void logger_cout(const char *content);
extern std::string to_string(int val);
extern std::string charto_string(char val);

class UVWrapTraderPet : public node::ObjectWrap
{
public:
	UVWrapTraderPet(void);
	~UVWrapTraderPet(void);

	static void Init(Isolate *isolate);
	static void NewInstance(const FunctionCallbackInfo<Value> &args);
	
	///注册事件
	static void On(const FunctionCallbackInfo<Value> &args);
	///连接前置机
	static void Connect(const FunctionCallbackInfo<Value> &args);
	///用户登录请求
	static void ReqUserLogin(const FunctionCallbackInfo<Value> &args);

private:
	static void initEventMap();
	static void New(const FunctionCallbackInfo<Value> &args);

	static void pkg_cb_userlogin(UVPetRtnInfo *data, Local<Value> *cbArray);
	static Local<Value> pkg_rspinfo(void *vpRspInfo);

	uv_trader_pet *uvTrader;
	static int s_uuid;
	static void FunCallback(UVPetRtnInfo *data);
	static void FunRtnCallback(int result, void *baton);
	static Persistent<Function> constructor;
	static std::map<std::string, int> event_map;
	static std::map<int, Persistent<Function>> callback_map;
	static std::map<int, Persistent<Function>> fun_rtncb_map;
};

#endif
