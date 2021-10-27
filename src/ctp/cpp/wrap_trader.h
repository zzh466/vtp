#ifndef WRAP_TRADER_H
#define WRAP_TRADER_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <node.h>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include <uv.h>
#include <node_object_wrap.h>
#include "uv_trader.h"

using namespace v8;

extern bool islog;
extern void logger_cout(const char *content);
extern std::string to_string(int val);
extern std::string charto_string(char val);

class WrapTrader : public node::ObjectWrap
{
public:
	WrapTrader(void);
	~WrapTrader(void);

	static void Init(Isolate *isolate);
	static void NewInstance(const FunctionCallbackInfo<Value> &args);
	static void GetTradingDay(const FunctionCallbackInfo<Value> &args);
	static void On(const FunctionCallbackInfo<Value> &args);
	static void Connect(const FunctionCallbackInfo<Value> &args);

	static void ReqAuthenticate(const FunctionCallbackInfo<Value> &args);

	static void ReqUserLogin(const FunctionCallbackInfo<Value> &args);
	static void ReqUserLogout(const FunctionCallbackInfo<Value> &args);
	///Ͷ���߽�����ȷ��
	static void ReqSettlementInfoConfirm(const FunctionCallbackInfo<Value>& args);
	///�����ѯ��Լ
	static void ReqQryInstrument(const FunctionCallbackInfo<Value>& args);
	///�����ѯ�ʽ��˻�
	static void ReqQryTradingAccount(const FunctionCallbackInfo<Value> &args);
	///�����ѯͶ���ֲ߳�
	//static void ReqQryInvestorPosition(const FunctionCallbackInfo<Value>& args);
	///�ֲ���ϸ
	//static void ReqQryInvestorPositionDetail(const FunctionCallbackInfo<Value>& args);
	///����¼������
	static void ReqOrderInsert(const FunctionCallbackInfo<Value> &args);
	///������������
	static void ReqOrderAction(const FunctionCallbackInfo<Value> &args);
	///�����ѯ��Լ��֤����
	//static void ReqQryInstrumentMarginRate(const FunctionCallbackInfo<Value>& args);
	///�����ѯ����
	static void ReqQryDepthMarketData(const FunctionCallbackInfo<Value> &args);
	///�����ѯͶ���߽�����
	static void ReqQrySettlementInfo(const FunctionCallbackInfo<Value>& args);
	///ɾ���ӿڶ���
	static void Disposed(const FunctionCallbackInfo<Value> &args);

private:
	static void initEventMap();
	static void New(const FunctionCallbackInfo<Value> &args);

	static void pkg_cb_userlogin(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_userlogout(CbRtnField *data, Local<Value> *cbArray);
	//static void pkg_cb_confirm(CbRtnField* data, Local<Value>*cbArray);
	static void pkg_cb_orderinsert(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_errorderinsert(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_orderaction(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_errorderaction(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rspqryorder(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rtnorder(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rqtrade(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rtntrade(CbRtnField *data, Local<Value> *cbArray);
	//static void pkg_cb_rqinvestorposition(CbRtnField* data, Local<Value>*cbArray);
	//static void pkg_cb_rqinvestorpositiondetail(CbRtnField* data, Local<Value>*cbArray);
	static void pkg_cb_rqtradingaccount(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rqinstrument(CbRtnField* data, Local<Value>*cbArray);
	static void pkg_cb_rqdepthmarketdata(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rqsettlementinfoconfirm(CbRtnField *data, Local<Value> *cbArray);
	static void pkg_cb_rqsettlementinfo(CbRtnField* data, Local<Value>*cbArray);
	static void pkg_cb_rsperror(CbRtnField *data, Local<Value> *cbArray);
	static Local<Value> pkg_rspinfo(void *vpRspInfo);

	uv_trader *uvTrader;
	static int s_uuid;
	static void FunCallback(CbRtnField *data);
	static void FunRtnCallback(int result, void *baton);
	static Persistent<Function> constructor;
	static std::map<std::string, int> event_map;
	static std::map<int, Persistent<Function>> callback_map;
	static std::map<int, Persistent<Function>> fun_rtncb_map;
};

#endif
