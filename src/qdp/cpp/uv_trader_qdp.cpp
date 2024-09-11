#include <uv.h>
#include "uv_trader_qdp.h"
#include "wraper_struct_qdp.h"
#include "QdpFtdcTraderApi.h"
#include "QdpFtdcUserApiDataType.h"
#include <cstring>
#include <sstream>
#include <string>

std::string to_string(int val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

std::string charto_string(char val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

std::map<int, CbWrap *> uv_trader::cb_map;

uv_trader::uv_trader(void)
{
	iRequestID = 0;
	uv_async_init(uv_default_loop(), &async_t, NULL);
	logger_cout("uv_trader init");
}

uv_trader::~uv_trader(void)
{
	uv_close((uv_handle_t *)&async_t, NULL);
}

const char *uv_trader::GetTradingDay()
{
	return this->m_pApi->GetTradingDay();
}

int uv_trader::On(const char *eName, int cb_type, void (*callback)(CbRtnField *cbResult))
{
	std::string log = "uv_trader On------>";
	std::map<int, CbWrap *>::iterator it = cb_map.find(cb_type);
	if (it != cb_map.end())
	{
		logger_cout(log.append(" event id").append(to_string(cb_type)).append(" register repeat").c_str());
		return 1;
	}

	CbWrap *cb_wrap = new CbWrap();
	cb_wrap->callback = callback;
	cb_map[cb_type] = cb_wrap;
	logger_cout(log.append(" Event:").append(eName).append(" ID:").append(to_string(cb_type)).append(" register").c_str());
	return 0;
}

void uv_trader::Connect(UVConnectField *pConnectField, void (*callback)(int, void *), int uuid)
{
	UVConnectField *_pConnectField = new UVConnectField();
	memcpy(_pConnectField, pConnectField, sizeof(UVConnectField));
	logger_cout("trader Connect this -> invoke");
	this->invoke(_pConnectField, T_CONNECT_RE, callback, uuid);
}

void uv_trader::Disconnect()
{
	m_pApi->RegisterSpi(NULL);
	m_pApi->Release();
	m_pApi = NULL;

	std::map<int, CbWrap *>::iterator callback_it = cb_map.begin();
	while (callback_it != cb_map.end())
	{
		delete callback_it->second;
		callback_it++;
	}
	logger_cout("uv_trader Disconnect------>object destroyed");
}

void uv_trader::Authenticate(CQdpFtdcAuthenticateField *pReqAuthenticateField, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcAuthenticateField *_pReqAuthenticateField = new CQdpFtdcAuthenticateField();
	memcpy(_pReqAuthenticateField, pReqAuthenticateField, sizeof(CQdpFtdcAuthenticateField));
	logger_cout("trader ReqAuthenticate this -> invoke");
	this->invoke(_pReqAuthenticateField, T_AUTHENTICATE_RE, callback, uuid);
}

void uv_trader::ReqUserLogin(CQdpFtdcReqUserLoginField *pReqUserLoginField, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcReqUserLoginField *_pReqUserLoginField = new CQdpFtdcReqUserLoginField();
	memcpy(_pReqUserLoginField, pReqUserLoginField, sizeof(CQdpFtdcReqUserLoginField));
	this->invoke(_pReqUserLoginField, T_LOGIN_RE, callback, uuid);
}

void uv_trader::ReqUserLogout(CQdpFtdcReqUserLogoutField *pUserLogout, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcReqUserLogoutField *_pUserLogout = new CQdpFtdcReqUserLogoutField();
	memcpy(_pUserLogout, pUserLogout, sizeof(CQdpFtdcReqUserLogoutField));
	this->invoke(_pUserLogout, T_LOGOUT_RE, callback, uuid);
}

void uv_trader::ReqQryInstrument(CQdpFtdcQryInstrumentField *pQryInstrument, void(*callback)(int, void*), int uuid) {
	CQdpFtdcQryInstrumentField *_pQryInstrument = new CQdpFtdcQryInstrumentField();
	memcpy(_pQryInstrument, pQryInstrument, sizeof(CQdpFtdcQryInstrumentField));
	this->invoke(_pQryInstrument, T_INSTRUMENT_RE, callback, uuid);
}

void uv_trader::ReqQryInvestorAccount(CQdpFtdcQryInvestorAccountField *pQryTradingAccount, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcQryInvestorAccountField *_pQryTradingAccount = new CQdpFtdcQryInvestorAccountField();
	memcpy(_pQryTradingAccount, pQryTradingAccount, sizeof(CQdpFtdcQryInvestorAccountField));
	this->invoke(_pQryTradingAccount, T_TRADINGACCOUNT_RE, callback, uuid);
}

void uv_trader::ReqOrderInsert(CQdpFtdcInputOrderField *pInputOrder, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcInputOrderField *_pInputOrder = new CQdpFtdcInputOrderField();
	memcpy(_pInputOrder, pInputOrder, sizeof(CQdpFtdcInputOrderField));
	this->invoke(_pInputOrder, T_INSERT_RE, callback, uuid);
}

void uv_trader::ReqOrderAction(CQdpFtdcOrderActionField *pInputOrderAction, void (*callback)(int, void *), int uuid)
{
	CQdpFtdcOrderActionField *_pInputOrderAction = new CQdpFtdcOrderActionField();
	memcpy(_pInputOrderAction, pInputOrderAction, sizeof(CQdpFtdcOrderActionField));
	this->invoke(_pInputOrderAction, T_ACTION_RE, callback, uuid);
}

// void uv_trader::ReqQryDepthMarketData(CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, void (*callback)(int, void *), int uuid)
// {
// 	CThostFtdcQryDepthMarketDataField *_pQryDepthMarketData = new CThostFtdcQryDepthMarketDataField();
// 	memcpy(_pQryDepthMarketData, pQryDepthMarketData, sizeof(CThostFtdcQryDepthMarketDataField));
// 	this->invoke(_pQryDepthMarketData, T_DEPTHMARKETDATA_RE, callback, uuid);
// }

// void uv_trader::ReqQrySettlementInfo(CThostFtdcQrySettlementInfoField *pQrySettlementInfo, void(*callback)(int, void*), int uuid)
// {
// 	CThostFtdcQrySettlementInfoField *_pQrySettlementInfo = new CThostFtdcQrySettlementInfoField();
// 	memcpy(_pQrySettlementInfo, pQrySettlementInfo, sizeof(CThostFtdcQrySettlementInfoField));
// 	this->invoke(_pQrySettlementInfo, T_SETTLEMENTINFO_RE, callback, uuid);
// }

// void uv_trader::ReqSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, void(*callback)(int, void*), int uuid)
// {
// 	CThostFtdcSettlementInfoConfirmField* _pSettlementInfoConfirm = new CThostFtdcSettlementInfoConfirmField();
// 	memcpy(_pSettlementInfoConfirm, pSettlementInfoConfirm, sizeof(CThostFtdcSettlementInfoConfirmField));
// 	this->invoke(_pSettlementInfoConfirm, T_CONFIRM_RE, callback, uuid);
// }

// void uv_trader::ReqQryInstrumentCommissionRate(CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, void(*callback)(int, void*),int uuid)
// {
// 	CThostFtdcQryInstrumentCommissionRateField* _pQryInstrumentCommissionRate = new CThostFtdcQryInstrumentCommissionRateField();
// 	memcpy(_pQryInstrumentCommissionRate, pQryInstrumentCommissionRate, sizeof(CThostFtdcQryInstrumentCommissionRateField));
// 	this->invoke(_pQryInstrumentCommissionRate, T_INSTRUMENTCOMMISSIONRATE_RE, callback, uuid);
// }

// void uv_trader::ReqQryInvestorPositionDetail(CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, void(*callback)(int, void*), int uuid) {
// 	CThostFtdcQryInvestorPositionDetailField *_pQryInvestorPositionDetail = new CThostFtdcQryInvestorPositionDetailField();
// 	memcpy(_pQryInvestorPositionDetail, pQryInvestorPositionDetail, sizeof(CThostFtdcQryInvestorPositionDetailField));
// 	this->invoke(_pQryInvestorPositionDetail, T_INVESTORPOSITIONDETAIL_RE, callback, uuid);
// }

// void uv_trader::ReqQrySettlementInfoConfirm(CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, void(*callback)(int, void*), int uuid)
// {
// 	CThostFtdcQrySettlementInfoConfirmField *_pQrySettlementInfoConfirm = new CThostFtdcQrySettlementInfoConfirmField();
// 	memcpy(_pQrySettlementInfoConfirm, pQrySettlementInfoConfirm, sizeof(CThostFtdcQrySettlementInfoConfirmField));
// 	this->invoke(_pQrySettlementInfoConfirm, T_QCONFIRM_RE, callback, uuid);
// }

void uv_trader::OnFrontConnected()
{
	std::string log = "uv_trader OnFrontConnected";
	logger_cout(log.c_str());
	CbRtnField *field = new CbRtnField();
	field->eFlag = T_ON_CONNECT;
	field->work.data = field;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}

void uv_trader::OnFrontDisconnected(int nReason)
{
	std::string log = "uv_trader OnFrontDisconnected------>";
	logger_cout(log.append("nReason:").append(to_string(nReason)).c_str());
	CbRtnField *field = new CbRtnField();
	field->eFlag = T_ON_DISCONNECTED;
	field->nReason = nReason;
	field->work.data = field;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}

void uv_trader::OnRspAuthenticate(CQdpFtdcRtnAuthenticateField *pRspAuthenticateField, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	std::string log = "uv_trader OnRspAuthenticate";
	logger_cout(log.append("").c_str());
	CbRtnField *field = new CbRtnField();
	field->eFlag = T_ON_RSPAUTHENTICATE;
	field->work.data = field;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
};

void uv_trader::OnRspUserLogin(CQdpFtdcRspUserLoginField *pRspUserLogin, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspUserLoginField *_pRspUserLogin = NULL;
	if (pRspUserLogin)
	{
		_pRspUserLogin = new CQdpFtdcRspUserLoginField();
		memcpy(_pRspUserLogin, pRspUserLogin, sizeof(CQdpFtdcRspUserLoginField));
	}
	std::string log = "uv_trader OnRspUserLogin------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPUSERLOGIN, _pRspUserLogin, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRspUserLogout(CQdpFtdcRspUserLogoutField *pUserLogout, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspUserLogoutField *_pUserLogout = NULL;
	if (pUserLogout)
	{
		_pUserLogout = new CQdpFtdcRspUserLogoutField();
		memcpy(_pUserLogout, pUserLogout, sizeof(CQdpFtdcRspUserLogoutField));
	}
	std::string log = "uv_trader OnRspUserLogout------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPUSERLOGOUT, _pUserLogout, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRspOrderInsert(CQdpFtdcInputOrderField *pInputOrder, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcInputOrderField *_pInputOrder = NULL;
	if (pInputOrder)
	{
		_pInputOrder = new CQdpFtdcInputOrderField();
		memcpy(_pInputOrder, pInputOrder, sizeof(CQdpFtdcInputOrderField));
	}
	std::string log = "uv_trader OnRspOrderInsert------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPINSERT, _pInputOrder, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnErrRtnOrderInsert(CQdpFtdcInputOrderField *pInputOrder, CQdpFtdcRspInfoField *pRspInfo)
{
	CQdpFtdcInputOrderField *_pInputOrder = NULL;
	if (pInputOrder)
	{
		_pInputOrder = new CQdpFtdcInputOrderField();
		memcpy(_pInputOrder, pInputOrder, sizeof(CQdpFtdcInputOrderField));
	}
	std::string log = "uv_trader OnErrRtnOrderInsert";
	logger_cout(log.c_str());
	on_invoke(T_ON_ERRINSERT, _pInputOrder, pRspInfo, 0, 0);
}

// void uv_trader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcSettlementInfoConfirmField *_pSettlementInfoConfirm;
// 	if(pSettlementInfoConfirm)
// 	{
// 		_pSettlementInfoConfirm = new CThostFtdcSettlementInfoConfirmField();
// 		memcpy(_pSettlementInfoConfirm, pSettlementInfoConfirm, sizeof(CThostFtdcSettlementInfoConfirmField));
// 	}
// 	std::string log = "uv_trader OnRspSettlementInfoConfirm";
// 	logger_cout(log.c_str());
// 	on_invoke(T_ON_RSETTLEMENTINFOCONFIRM, _pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
// }

void uv_trader::OnRspOrderAction(CQdpFtdcOrderActionField *pInputOrderAction, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcOrderActionField *_pInputOrderAction = NULL;
	if (pInputOrderAction)
	{
		_pInputOrderAction = new CQdpFtdcOrderActionField();
		memcpy(_pInputOrderAction, pInputOrderAction, sizeof(CQdpFtdcOrderActionField));
	}
	std::string log = "uv_trader OnRspOrderAction------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPACTION, _pInputOrderAction, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnErrRtnOrderAction(CQdpFtdcOrderActionField *pOrderAction, CQdpFtdcRspInfoField *pRspInfo)
{
	CQdpFtdcOrderActionField *_pOrderAction = NULL;
	if (pOrderAction)
	{
		_pOrderAction = new CQdpFtdcOrderActionField();
		memcpy(_pOrderAction, pOrderAction, sizeof(CQdpFtdcOrderActionField));
	}
	std::string log = "uv_trader OnErrRtnOrderAction";
	logger_cout(log.c_str());
	on_invoke(T_ON_ERRACTION, _pOrderAction, pRspInfo, 0, 0);
}

void uv_trader::OnRspQryOrder(CQdpFtdcOrderField *pOrder, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcOrderField *_pOrder = NULL;
	if (pOrder)
	{
		_pOrder = new CQdpFtdcOrderField();
		memcpy(_pOrder, pOrder, sizeof(CQdpFtdcOrderField));
	}
	std::string log = "uv_trader OnRspQryOrder------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQORDER, _pOrder, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRtnOrder(CQdpFtdcOrderField *pOrder)
{
	CQdpFtdcOrderField *_pOrder = NULL;
	if (pOrder)
	{
		_pOrder = new CQdpFtdcOrderField();
		memcpy(_pOrder, pOrder, sizeof(CQdpFtdcOrderField));
	}
	//std::string log = "uv_trader OnRtnOrder";
	//logger_cout(log.c_str());
	on_invoke(T_ON_RTNORDER, _pOrder, new CQdpFtdcRspInfoField(), 0, 0);
}

void uv_trader::OnRspQryTrade(CQdpFtdcTradeField *pTrade, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcTradeField *_pTrade = NULL;
	if (pTrade)
	{
		_pTrade = new CQdpFtdcTradeField();
		memcpy(_pTrade, pTrade, sizeof(CQdpFtdcTradeField));
	}
	std::string log = "uv_trader OnRspQryTrade------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQTRADE, _pTrade, pRspInfo, nRequestID, bIsLast);
}

void uv_trader::OnRtnTrade(CQdpFtdcTradeField *pTrade)
{
	CQdpFtdcTradeField *_pTrade = NULL;
	if (pTrade)
	{
		_pTrade = new CQdpFtdcTradeField();
		memcpy(_pTrade, pTrade, sizeof(CQdpFtdcTradeField));
	}
	//std::string log = "uv_trader OnRtnTrade";
	//logger_cout(log.c_str());
	on_invoke(T_ON_RTNTRADE, _pTrade, new CQdpFtdcRspInfoField(), 0, 0);
}

//void uv_trader::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
void uv_trader::OnRspQryInvestorAccount(CQdpFtdcRspInvestorAccountField *pRspInvestorAccount, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspInvestorAccountField *_pTradingAccount = NULL;
	if (pRspInvestorAccount)
	{
		_pTradingAccount = new CQdpFtdcRspInvestorAccountField();
		memcpy(_pTradingAccount, pRspInvestorAccount, sizeof(CQdpFtdcRspInvestorAccountField));
	}
	// std::string log = "uv_trader OnRspQryTradingAccount------>";
	// logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQTRADINGACCOUNT, _pTradingAccount, pRspInfo, nRequestID, bIsLast);
}

// void uv_trader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcInstrumentField *_pInstrument = NULL;
// 	if (pInstrument) {
// 		_pInstrument = new CThostFtdcInstrumentField();
// 		memcpy(_pInstrument, pInstrument, sizeof(CThostFtdcInstrumentField));
// 	}
// 	std::string log = "uv_trader OnRspQryInstrument------>";
// 	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
// 	on_invoke(T_ON_RQINSTRUMENT, _pInstrument, pRspInfo, nRequestID, bIsLast);
// }

// void uv_trader::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcDepthMarketDataField *_pDepthMarketData = NULL;
// 	if (pDepthMarketData)
// 	{
// 		_pDepthMarketData = new CThostFtdcDepthMarketDataField();
// 		memcpy(pDepthMarketData, _pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));
// 	}
// 	std::string log = "uv_trader OnRspQryDepthMarketData------>";
// 	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
// 	on_invoke(T_ON_RQDEPTHMARKETDATA, _pDepthMarketData, pRspInfo, nRequestID, bIsLast);
// }

// void uv_trader::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcSettlementInfoField* _pSettlementInfo = NULL;
// 	if (pSettlementInfo) {
// 		_pSettlementInfo = new CThostFtdcSettlementInfoField();
// 		memcpy(_pSettlementInfo, pSettlementInfo, sizeof(CThostFtdcSettlementInfoField));
// 	}
// 	std::string log = "uv_trader OnRspQrySettlementInfo------>";
// 	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
// 	on_invoke(T_ON_RQSETTLEMENTINFO, _pSettlementInfo, pRspInfo, nRequestID, bIsLast);
// }

void uv_trader::OnRspQryInvestorFee(CQdpFtdcInvestorFeeField *pInvestorFee, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcInvestorFeeField* _pInvestorFee = NULL;
	if (pInvestorFee) {
		_pInvestorFee = new CQdpFtdcInvestorFeeField();
		memcpy(_pInvestorFee, pInvestorFee, sizeof(CQdpFtdcInvestorFeeField));
	}
	std::string log = "uv_trader CQdpFtdcInvestorFeeField------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	// on_invoke(T_ON_RQINSTRUMENTCOMMISSIONRATE, _pInvestorFee, pRspInfo, nRequestID, bIsLast);
}
// void uv_trader::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcInstrumentCommissionRateField* _pInstrumentCommissionRate = NULL;
// 	if (pInstrumentCommissionRate) {
// 		_pInstrumentCommissionRate = new CThostFtdcInstrumentCommissionRateField();
// 		memcpy(_pInstrumentCommissionRate, pInstrumentCommissionRate, sizeof(CThostFtdcInstrumentCommissionRateField));
// 	}
// 	std::string log = "uv_trader OnRspQryInstrumentCommissionRate------>";
// 	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
// 	on_invoke(T_ON_RQINSTRUMENTCOMMISSIONRATE, _pInstrumentCommissionRate, pRspInfo, nRequestID, bIsLast);
// }

void uv_trader::OnRspQryInvestorPosition(CQdpFtdcRspInvestorPositionField *pInvestorPositionDetail, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspInvestorPositionField* _pInvestorPositionDetail = NULL;
	if (pInvestorPositionDetail){
		_pInvestorPositionDetail = new CQdpFtdcRspInvestorPositionField();
		memcpy(_pInvestorPositionDetail, pInvestorPositionDetail, sizeof(CQdpFtdcRspInvestorPositionField));
	}
	std::string log = "uv_trader OnRspQryInvestorPosition------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RQINVESTORPOSITIONDETAIL, _pInvestorPositionDetail, pRspInfo, nRequestID, bIsLast);
}

// void uv_trader::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
// {
// 	CThostFtdcSettlementInfoConfirmField* _pSettlementInfoConfirm = NULL;
// 	if (pSettlementInfoConfirm){
// 		_pSettlementInfoConfirm = new CThostFtdcSettlementInfoConfirmField();
// 		memcpy(_pSettlementInfoConfirm, pSettlementInfoConfirm, sizeof(CThostFtdcSettlementInfoConfirmField));
// 	}
// 	std::string log = "uv_trader OnRspQrySettlementInfoConfirm------>";
// 	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
// 	on_invoke(T_ON_RQSETTLEMENTINFOCONFIRM, _pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
// }

void uv_trader::OnRspError(CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspInfoField *_pRspInfo = NULL;
	if (pRspInfo)
	{
		_pRspInfo = new CQdpFtdcRspInfoField();
		memcpy(_pRspInfo, pRspInfo, sizeof(CQdpFtdcRspInfoField));
	}
	std::string log = "uv_trader OnRspError------>";
	logger_cout(log.append("requestid:").append(to_string(nRequestID)).append(",islast:").append(to_string(bIsLast)).c_str());
	on_invoke(T_ON_RSPERROR, _pRspInfo, pRspInfo, nRequestID, bIsLast);
}

// void uv_trader::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder) {
// 	CThostFtdcErrorConditionalOrderField *_pErrorConditionalOrder = NULL;
// 	if (pErrorConditionalOrder)
// 	{
// 		_pErrorConditionalOrder = new CThostFtdcErrorConditionalOrderField();
// 		memcpy(_pErrorConditionalOrder, pErrorConditionalOrder, sizeof(CThostFtdcErrorConditionalOrderField));
// 	}
// 	std::string log = "uv_trader OnRtnErrorConditionalOrder------>";
// 	logger_cout(log.append("").c_str());
// 	on_invoke(T_ON_RTNERRORCONDITIONALORDER, _pErrorConditionalOrder, new CThostFtdcRspInfoField(), 0, 0);
// };

void uv_trader::_async(uv_work_t *work)
{
	LookupQdpApiBaton *baton = static_cast<LookupQdpApiBaton *>(work->data);
	uv_trader *uv_trader_obj = static_cast<uv_trader *>(baton->uv_trader_obj);
	std::string log = "uv_trader _async---1--->";
	// logger_cout(log.append(to_string(baton->fun)).c_str());
	switch (baton->fun)
	{
	case T_CONNECT_RE:
	{
		UVConnectField *_pConnectF = static_cast<UVConnectField *>(baton->args);
		uv_trader_obj->m_pApi = CQdpFtdcTraderApi::CreateFtdcTraderApi();
		uv_trader_obj->m_pApi->RegisterSpi(uv_trader_obj);
		uv_trader_obj->m_pApi->RegisterFront(_pConnectF->front_addr);
		uv_trader_obj->m_pApi->Init(); // CThostFtdcMdApi
		//logger_cout(log.append("invoke connect,the result is 0 | szPath is ").append(_pConnectF->szPath).append(" | ApiVersion is ").append(CThostFtdcTraderApi::GetApiVersion()).c_str());
		break;
	}
	case T_AUTHENTICATE_RE:
	{
		CQdpFtdcAuthenticateField *req = static_cast<CQdpFtdcAuthenticateField *>(baton->args);
		// strcpy_s(req.BrokerID, CVTPUser::GetInstance()->GetBrokerID());
		// strcpy_s(req.UserID, CVTPUser::GetInstance()->GetAccountID());
		// strcpy_s(req.UserProductInfo, "VTP");
		// strcpy_s(req.AuthCode, CVTPUser::GetInstance()->GetAuthCode());
		// strcpy_s(req.AppID, CVTPUser::GetInstance()->GetAppID());
		baton->nResult = uv_trader_obj->m_pApi->ReqAuthenticate(req, 2);
		//logger_cout(log.append("invoke ReqAuthenticate,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_LOGIN_RE:	{
		CQdpFtdcReqUserLoginField *_pReqUserLoginField = static_cast<CQdpFtdcReqUserLoginField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqUserLogin(_pReqUserLoginField, baton->iRequestID);
		//logger_cout(log.append("invoke ReqUserLogin,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_LOGOUT_RE:
	{
		CQdpFtdcReqUserLogoutField *_pUserLogout = static_cast<CQdpFtdcReqUserLogoutField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqUserLogout(_pUserLogout, baton->iRequestID);
		//logger_cout(log.append("invoke ReqUserLogout,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	// case T_CONFIRM_RE:
	// {
	// 	CThostFtdcSettlementInfoConfirmField *_pSettlementInfoConfirm = static_cast<CThostFtdcSettlementInfoConfirmField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqSettlementInfoConfirm(_pSettlementInfoConfirm, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqSettlementInfoConfirm,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	case T_INSTRUMENT_RE:
	{
		CQdpFtdcQryInstrumentField *_pQryInstrument = static_cast<CQdpFtdcQryInstrumentField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrument(_pQryInstrument, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInstrument,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_TRADINGACCOUNT_RE:
	{
		CQdpFtdcQryInvestorAccountField *_pQryTradingAccount = static_cast<CQdpFtdcQryInvestorAccountField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorAccount(_pQryTradingAccount, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryTradingAccount,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INVESTORPOSITION_RE:
	{
		CQdpFtdcQryInvestorPositionField *_pQryInvestorPosition = static_cast<CQdpFtdcQryInvestorPositionField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPosition(_pQryInvestorPosition, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInvestorPosition,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	// case T_INVESTORPOSITIONDETAIL_RE:
	// {
	// 	CThostFtdcQryInvestorPositionDetailField *_pQryInvestorPositionDetail = static_cast<CThostFtdcQryInvestorPositionDetailField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPositionDetail(_pQryInvestorPositionDetail, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQryInvestorPositionDetail,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	case T_INSERT_RE:
	{
		CQdpFtdcInputOrderField *_pInputOrder = static_cast<CQdpFtdcInputOrderField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqOrderInsert(_pInputOrder, baton->iRequestID);
		logger_cout(log.append("invoke ReqOrderInsert,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_ACTION_RE:
	{
		CQdpFtdcOrderActionField *_pInputOrderAction = static_cast<CQdpFtdcOrderActionField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqOrderAction(_pInputOrderAction, baton->iRequestID);
		logger_cout(log.append("invoke ReqOrderAction,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	// case T_MARGINRATE_RE:
	// {
	// 	CThostFtdcQryInstrumentMarginRateField *_pQryInstrumentMarginRate = static_cast<CThostFtdcQryInstrumentMarginRateField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrumentMarginRate(_pQryInstrumentMarginRate, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQryInstrumentMarginRate,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	// case T_DEPTHMARKETDATA_RE:
	// {
	// 	CThostFtdcQryDepthMarketDataField *_pQryDepthMarketData = static_cast<CThostFtdcQryDepthMarketDataField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQryDepthMarketData(_pQryDepthMarketData, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQryDepthMarketData,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	// case T_SETTLEMENTINFO_RE:
	// {
	// 	CThostFtdcQrySettlementInfoField *_pQrySettlementInfo = static_cast<CThostFtdcQrySettlementInfoField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQrySettlementInfo(_pQrySettlementInfo, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQrySettlementInfo,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	// case T_INSTRUMENTCOMMISSIONRATE_RE:
	// {
	// 	CThostFtdcQryInstrumentCommissionRateField *_pQryInstrumentCommissionRate = static_cast<CThostFtdcQryInstrumentCommissionRateField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrumentCommissionRate(_pQryInstrumentCommissionRate, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQryInstrumentCommissionRate,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	// case T_QCONFIRM_RE:
	// {
	// 	CThostFtdcQrySettlementInfoConfirmField *_pQrySettlementInfoConfirm = static_cast<CThostFtdcQrySettlementInfoConfirmField *>(baton->args);
	// 	baton->nResult = uv_trader_obj->m_pApi->ReqQrySettlementInfoConfirm(_pQrySettlementInfoConfirm, baton->iRequestID);
	// 	//logger_cout(log.append("invoke ReqQrySettlementInfoConfirm,the result:").append(to_string(baton->nResult)).c_str());
	// 	break;
	// }
	default:
	{
		//logger_cout(log.append("No case event:").append(to_string(baton->fun)).c_str());
		break;
	}
	}
}
///uv_queue_work
void uv_trader::_completed(uv_work_t *work, int)
{
	LookupQdpApiBaton *baton = static_cast<LookupQdpApiBaton *>(work->data);
	baton->callback(baton->nResult, baton);
	delete baton->args;
	delete baton;
}

void uv_trader::_on_async(uv_work_t *work)
{
	//do nothing
}

void uv_trader::_on_completed(uv_work_t *work, int)
{
	// std::string head = "uv_trader _on_completed  ==== ";
	// logger_cout(head.c_str());

	CbRtnField *cbTrnField = static_cast<CbRtnField *>(work->data);
	std::map<int, CbWrap *>::iterator it = cb_map.find(cbTrnField->eFlag);
	if (it != cb_map.end())
	{
		cb_map[cbTrnField->eFlag]->callback(cbTrnField);
	}
	if (cbTrnField->rtnField)
		delete cbTrnField->rtnField;
	if (cbTrnField->rspInfo)
		delete cbTrnField->rspInfo;
	delete cbTrnField;
}

void uv_trader::invoke(void *field, int ret, void (*callback)(int, void *), int uuid)
{
	LookupQdpApiBaton *baton = new LookupQdpApiBaton();
	baton->work.data = baton;
	baton->uv_trader_obj = this;
	baton->callback = callback;
	baton->args = field;
	baton->fun = ret;
	baton->uuid = uuid;

	iRequestID = iRequestID + 1;
	baton->iRequestID = iRequestID;
	//std::string head = "uv_trader invoke------>uuid:";
	//logger_cout(head.append(to_string(uuid)).append(",requestid:").append(to_string(baton->iRequestID)).c_str());
	uv_queue_work(uv_default_loop(), &baton->work, _async, _completed);
	//logger_cout("uv_trader invoke is finish");
}

void uv_trader::on_invoke(int event_type, void *_stru, CQdpFtdcRspInfoField *pRspInfo_org, int nRequestID, bool bIsLast)
{
	CQdpFtdcRspInfoField *_pRspInfo = NULL;
	if (pRspInfo_org)
	{
		_pRspInfo = new CQdpFtdcRspInfoField();
		memcpy(_pRspInfo, pRspInfo_org, sizeof(CQdpFtdcRspInfoField));
	}
	CbRtnField *field = new CbRtnField();
	field->work.data = field;
	field->eFlag = event_type;
	field->rtnField = _stru;
	field->rspInfo = (void *)_pRspInfo;
	field->nRequestID = nRequestID;
	field->bIsLast = bIsLast;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}
