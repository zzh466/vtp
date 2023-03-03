#include <uv.h>
#include <cstring>
#include <sstream>
#include <string>

#include "uv_trader_pet.h"

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

std::map<int, CbWrap *> uv_trader_pet::cb_map;

uv_trader_pet::uv_trader_pet(void)
{
	iRequestID = 0;
	uv_async_init(uv_default_loop(), &async_t, NULL);
	logger_cout("uv_trader init");
}

uv_trader_pet::~uv_trader_pet(void)
{
	uv_close((uv_handle_t *)&async_t, NULL);
}

void uv_trader_pet::OnConnected(void(*callback)(int, void*),  int uuid)
{
    std::string log = "uv_trader_pet OnConnected";
    logger_cout(log.c_str());
    
    std::string& strErrInfo = "pet Connect is success";
    m_pApi = iudge::client::IudgeClientApi::CreateClient();
    if (!m_pApi)
    {
        strErrInfo = "pet Connect api is null";
    }
    int nRet = 0;
    if (0 == (nRet = m_pApi->RegistSpi(this, strErrInfo)) &&
        0 == (nRet = m_pApi->RegisterFront(HOST, PORT, strErrInfo)))

    logger_cout(strErrInfo.c_str());
}

void uv_trader_pet::Login(UvPetLoginInfo *pLoginInfo, void(*callback)(int, void*),  int uuid)
{
    std::string strReply;
    nlohmann::json jsonRequest;
    jsonRequest["userName"] = pLoginInfo->user;
    jsonRequest["password"] = pLoginInfo->pwd;
    jsonRequest["vacctnumber"] = pLoginInfo->vprt;
    jsonRequest["mac"] = pLoginInfo->mac;
    jsonRequest["action"] = "/external/login";
    int nRet = m_pApi->Request("/external/login", jsonRequest.dump(), strReply,
                               strErrInfo);
    if (0 != nRet)
        return nRet;

    auto jsonResp = nlohmann::json::parse(strReply);
    m_strToken = jsonResp["Authorization"].get<std::string>();
    strErrInfo = std::string("Token=").append(m_strToken);
    return 0;
}

void uv_trader_pet::Subscribe(std::string& strErrInfo)
{
	
}

void uv_trader_pet::OnDisconnected(UvPetDisconnected *pDisconnected, void(*callback)(int, void*),  int uuid)
{
    std::string log = "uv_trader_pet OnFrontConnected";
    logger_cout(log.c_str());
}

void uv_trader_pet::OnEvent(UvPetEventField *pEventFidle, void(*callback)(int, void*),  int uuid)
{
    UvPetEventField *_petEventField = NULL;
    if (pEventFidle)
    {
        _pUserLogout = new UvPetEventField();
        memcpy(_petEventField, pEventFidle, sizeof(UvPetEventField));
    }
    std::string log = "uv_trader_pet OnEvent------>";
    on_invoke(T_ON_RSPUSERLOGOUT, _pUserLogout, pRspInfo, nRequestID, bIsLast);

	CThostFtdcErrorConditionalOrderField *_pErrorConditionalOrder = NULL;
	if (pErrorConditionalOrder)
	{
		_pErrorConditionalOrder = new CThostFtdcErrorConditionalOrderField();
		memcpy(_pErrorConditionalOrder, pErrorConditionalOrder, sizeof(CThostFtdcErrorConditionalOrderField));
	}
	std::string log = "uv_trader OnRtnErrorConditionalOrder------>";
	logger_cout(log.append("").c_str());
	on_invoke(T_ON_RTNERRORCONDITIONALORDER, _pErrorConditionalOrder, new CThostFtdcRspInfoField(), 0, 0);
}

int testAsyncGeneralAction(
				const std::string& strAction,
				std::function<void(const int, const std::string&, const std::string&)>
								fnCallback,
				std::function<void(nlohmann::json&)> fnUserSetting = NULL,
				bool bPrintInput = false)
{
				nlohmann::json jsonRequest;
				jsonRequest["action"] = strAction;
				jsonRequest["accessToken"] = m_strToken;
				jsonRequest["mac"] = "B06EBF818B0C";
				jsonRequest["vbrokerid"] = VPRT;

				if (fnUserSetting)
								fnUserSetting(jsonRequest);

				m_pApi->Request(strAction, jsonRequest.dump(), fnCallback, 10 * 1000);

				return 0;
}

void uv_trader::_async(uv_work_t *work)
{
	LookupCtpApiBaton *baton = static_cast<LookupCtpApiBaton *>(work->data);
	uv_trader *uv_trader_obj = static_cast<uv_trader *>(baton->uv_trader_obj);
	std::string log = "uv_trader _async---1--->";
	// logger_cout(log.append(to_string(baton->fun)).c_str());
	switch (baton->fun)
	{
	case T_CONNECT_RE:
	{
		UVConnectField *_pConnectF = static_cast<UVConnectField *>(baton->args);
		uv_trader_obj->m_pApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_pConnectF->szPath);
		uv_trader_obj->m_pApi->RegisterSpi(uv_trader_obj);
		uv_trader_obj->m_pApi->SubscribePublicTopic(static_cast<THOST_TE_RESUME_TYPE>(_pConnectF->public_topic_type));
		uv_trader_obj->m_pApi->SubscribePrivateTopic(static_cast<THOST_TE_RESUME_TYPE>(_pConnectF->private_topic_type));
		uv_trader_obj->m_pApi->RegisterFront(_pConnectF->front_addr);
		uv_trader_obj->m_pApi->Init(); // CThostFtdcMdApi
		//logger_cout(log.append("invoke connect,the result is 0 | szPath is ").append(_pConnectF->szPath).append(" | ApiVersion is ").append(CThostFtdcTraderApi::GetApiVersion()).c_str());
		break;
	}
	case T_AUTHENTICATE_RE:
	{
		CThostFtdcReqAuthenticateField *req = static_cast<CThostFtdcReqAuthenticateField *>(baton->args);
		// strcpy_s(req.BrokerID, CVTPUser::GetInstance()->GetBrokerID());
		// strcpy_s(req.UserID, CVTPUser::GetInstance()->GetAccountID());
		// strcpy_s(req.UserProductInfo, "VTP");
		// strcpy_s(req.AuthCode, CVTPUser::GetInstance()->GetAuthCode());
		// strcpy_s(req.AppID, CVTPUser::GetInstance()->GetAppID());
		baton->nResult = uv_trader_obj->m_pApi->ReqAuthenticate(req, 2);
		//logger_cout(log.append("invoke ReqAuthenticate,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_LOGIN_RE:
	{
		CThostFtdcReqUserLoginField *_pReqUserLoginField = static_cast<CThostFtdcReqUserLoginField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqUserLogin(_pReqUserLoginField, baton->iRequestID);
		//logger_cout(log.append("invoke ReqUserLogin,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_LOGOUT_RE:
	{
		CThostFtdcUserLogoutField *_pUserLogout = static_cast<CThostFtdcUserLogoutField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqUserLogout(_pUserLogout, baton->iRequestID);
		//logger_cout(log.append("invoke ReqUserLogout,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_CONFIRM_RE:
	{
		CThostFtdcSettlementInfoConfirmField *_pSettlementInfoConfirm = static_cast<CThostFtdcSettlementInfoConfirmField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqSettlementInfoConfirm(_pSettlementInfoConfirm, baton->iRequestID);
		//logger_cout(log.append("invoke ReqSettlementInfoConfirm,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INSTRUMENT_RE:
	{
		CThostFtdcQryInstrumentField *_pQryInstrument = static_cast<CThostFtdcQryInstrumentField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrument(_pQryInstrument, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInstrument,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_TRADINGACCOUNT_RE:
	{
		CThostFtdcQryTradingAccountField *_pQryTradingAccount = static_cast<CThostFtdcQryTradingAccountField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryTradingAccount(_pQryTradingAccount, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryTradingAccount,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INVESTORPOSITION_RE:
	{
		CThostFtdcQryInvestorPositionField *_pQryInvestorPosition = static_cast<CThostFtdcQryInvestorPositionField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPosition(_pQryInvestorPosition, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInvestorPosition,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INVESTORPOSITIONDETAIL_RE:
	{
		CThostFtdcQryInvestorPositionDetailField *_pQryInvestorPositionDetail = static_cast<CThostFtdcQryInvestorPositionDetailField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInvestorPositionDetail(_pQryInvestorPositionDetail, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInvestorPositionDetail,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INSERT_RE:
	{
		CThostFtdcInputOrderField *_pInputOrder = static_cast<CThostFtdcInputOrderField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqOrderInsert(_pInputOrder, baton->iRequestID);
		logger_cout(log.append("invoke ReqOrderInsert,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_ACTION_RE:
	{
		CThostFtdcInputOrderActionField *_pInputOrderAction = static_cast<CThostFtdcInputOrderActionField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqOrderAction(_pInputOrderAction, baton->iRequestID);
		logger_cout(log.append("invoke ReqOrderAction,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_MARGINRATE_RE:
	{
		CThostFtdcQryInstrumentMarginRateField *_pQryInstrumentMarginRate = static_cast<CThostFtdcQryInstrumentMarginRateField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrumentMarginRate(_pQryInstrumentMarginRate, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInstrumentMarginRate,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_DEPTHMARKETDATA_RE:
	{
		CThostFtdcQryDepthMarketDataField *_pQryDepthMarketData = static_cast<CThostFtdcQryDepthMarketDataField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryDepthMarketData(_pQryDepthMarketData, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryDepthMarketData,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_SETTLEMENTINFO_RE:
	{
		CThostFtdcQrySettlementInfoField *_pQrySettlementInfo = static_cast<CThostFtdcQrySettlementInfoField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQrySettlementInfo(_pQrySettlementInfo, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQrySettlementInfo,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_INSTRUMENTCOMMISSIONRATE_RE:
	{
		CThostFtdcQryInstrumentCommissionRateField *_pQryInstrumentCommissionRate = static_cast<CThostFtdcQryInstrumentCommissionRateField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryInstrumentCommissionRate(_pQryInstrumentCommissionRate, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQryInstrumentCommissionRate,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_QCONFIRM_RE:
	{
		CThostFtdcQrySettlementInfoConfirmField *_pQrySettlementInfoConfirm = static_cast<CThostFtdcQrySettlementInfoConfirmField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQrySettlementInfoConfirm(_pQrySettlementInfoConfirm, baton->iRequestID);
		//logger_cout(log.append("invoke ReqQrySettlementInfoConfirm,the result:").append(to_string(baton->nResult)).c_str());
		break;
	}
	case T_QORDER_RE:
	{
		CThostFtdcQryOrderField *_pQryOrder = static_cast<CThostFtdcQryOrderField *>(baton->args);
		baton->nResult = uv_trader_obj->m_pApi->ReqQryOrder(_pQryOrder, baton->iRequestID);
		break;
	}
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
	LookupCtpApiBaton *baton = static_cast<LookupCtpApiBaton *>(work->data);
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
	LookupCtpApiBaton *baton = new LookupCtpApiBaton();
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

void uv_trader::on_invoke(int event_type, void *_stru, CThostFtdcRspInfoField *pRspInfo_org, int nRequestID, bool bIsLast)
{
	CThostFtdcRspInfoField *_pRspInfo = NULL;
	if (pRspInfo_org)
	{
		_pRspInfo = new CThostFtdcRspInfoField();
		memcpy(_pRspInfo, pRspInfo_org, sizeof(CThostFtdcRspInfoField));
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
