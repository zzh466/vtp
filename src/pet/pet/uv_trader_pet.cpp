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

std::map<int, UVPetWrap *> uv_trader_pet::cb_map;

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

void uv_trader_pet::Connect(UvPetRegisterFrontReqInfo* reqInfo, void(*callback)(int, void*),  int uuid)
{
	UvPetRegisterFrontReqInfo *_pConnectField = new UvPetRegisterFrontReqInfo();
	memcpy(_pConnectField, reqInfo, sizeof(UvPetRegisterFrontReqInfo));
	logger_cout("trader Connect this -> invoke");
	this->invoke(_pConnectField, T_CONNECT_RE, callback, uuid);
}

// 

// void uv_trader_pet::Subscribe(std::string& strErrInfo)
// {
	
// }

// void uv_trader_pet::OnDisconnected(UvPetDisconnected *pDisconnected, void(*callback)(int, void*),  int uuid)
// {
//     std::string log = "uv_trader_pet OnFrontConnected";
//     logger_cout(log.c_str());
// }

// void uv_trader_pet::OnEvent(UvPetEventField *pEventFidle, void(*callback)(int, void*),  int uuid)
// {
//     UvPetEventField *_petEventField = NULL;
//     if (pEventFidle)
//     {
//         _pUserLogout = new UvPetEventField();
//         memcpy(_petEventField, pEventFidle, sizeof(UvPetEventField));
//     }
//     std::string log = "uv_trader_pet OnEvent------>";
//     on_invoke(T_ON_RSPUSERLOGOUT, _pUserLogout, pRspInfo, nRequestID, bIsLast);

void uv_trader_pet::OnConnected()
{
	logger_cout("OnConnected");
}

void uv_trader_pet::OnDisconnected(int nErrorCode)
{
	logger_cout("OnDisconnected");
}

void uv_trader_pet::OnEvent(const int nUrlHash, const std::string& strBizBody)
{
	logger_cout("OnEvent");
}

// 	CThostFtdcErrorConditionalOrderField *_pErrorConditionalOrder = NULL;
// 	if (pErrorConditionalOrder)
// 	{
// 		_pErrorConditionalOrder = new CThostFtdcErrorConditionalOrderField();
// 		memcpy(_pErrorConditionalOrder, pErrorConditionalOrder, sizeof(CThostFtdcErrorConditionalOrderField));
// 	}
// 	std::string log = "uv_trader OnRtnErrorConditionalOrder------>";
// 	logger_cout(log.append("").c_str());
// 	on_invoke(T_ON_RTNERRORCONDITIONALORDER, _pErrorConditionalOrder, new CThostFtdcRspInfoField(), 0, 0);
// }

int uv_trader_pet::testAsyncGeneralAction(
				const std::string& strAction,
				std::function<void(const int, const std::string&, const std::string&)>
								fnCallback,
				std::function<void(nlohmann::json&)> fnUserSetting = NULL,
				bool bPrintInput = false)
{
				nlohmann::json jsonRequest;
				jsonRequest["action"] = strAction;
				jsonRequest["accessToken"] = m_strToken;
				jsonRequest["mac"] = m_strMac;
				jsonRequest["vbrokerid"] = m_strVPRT;

				if (fnUserSetting)
								fnUserSetting(jsonRequest);

				m_pApi->Request(strAction, jsonRequest.dump(), fnCallback, 10 * 1000);

				return 0;
}

void uv_trader_pet::_async(uv_work_t *work)
{
	LookupPetApiBaton *baton = static_cast<LookupPetApiBaton *>(work->data);
	uv_trader_pet *uv_trader_obj = static_cast<uv_trader_pet *>(baton->uv_trader_obj);
	std::string log = "uv_trader _async---1--->";
	// logger_cout(log.append(to_string(baton->fun)).c_str());
	switch (baton->fun)
	{
		case T_CONNECT_RE:
		{
			std::string strHolder, strInfo;
			iudge::client::IudgeClientApi::InitializeLogger(strHolder, "iudge_log",
                                                    nullptr);

			UvPetRegisterFrontReqInfo *_pConnectF = static_cast<UvPetRegisterFrontReqInfo *>(baton->args);
			uv_trader_obj->m_pApi = iudge::client::IudgeClientApi::CreateClient();
			std::string strErrInfo = "pet Connect is success";
			if (!uv_trader_obj->m_pApi)
			{
				strErrInfo = "pet Connect api is null";
			}
			int nRet = 0, nRet2 = 0;
			nRet = uv_trader_obj->m_pApi->RegistSpi(uv_trader_obj, strErrInfo);
			// logger_cout(log.append(to_string(nRet)).c_str());
			// const std::string host(_pConnectF->host);
			// nRet = uv_trader_obj->m_pApi->RegisterFront(host, _pConnectF->post, strErrInfo);
			nRet2 = uv_trader_obj->m_pApi->RegisterFront(uv_trader_obj->HOST, uv_trader_obj->PORT, strErrInfo);
			// logger_cout(log.append(to_string(nRet)).c_str());
			// if (0 == (nRet = uv_trader_obj->m_pApi->RegistSpi(uv_trader_obj, strErrInfo)) 
			// && 0 == (nRet = uv_trader_obj->m_pApi->RegisterFront(_pConnectF->host, _pConnectF->post, strErrInfo)))
			logger_cout(strErrInfo.c_str());
			break;
		}
		// case T_LOGIN_RE:
		// {
		// 	CThostFtdcReqUserLoginField *_pReqUserLoginField = static_cast<CThostFtdcReqUserLoginField *>(baton->args);
		// 	baton->nResult = uv_trader_obj->m_pApi->ReqUserLogin(_pReqUserLoginField, baton->iRequestID);
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
void uv_trader_pet::_completed(uv_work_t *work, int)
{
	LookupPetApiBaton *baton = static_cast<LookupPetApiBaton *>(work->data);
	baton->callback(baton->nResult, baton);
	delete baton->args;
	delete baton;
}

void uv_trader_pet::_on_async(uv_work_t *work)
{
	//do nothing
}

void uv_trader_pet::_on_completed(uv_work_t *work, int)
{
	// std::string head = "uv_trader _on_completed  ==== ";
	// logger_cout(head.c_str());

	UVPetRtnInfo *cbTrnField = static_cast<UVPetRtnInfo *>(work->data);
	std::map<int, UVPetWrap *>::iterator it = cb_map.find(cbTrnField->eFlag);
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

void uv_trader_pet::invoke(void *field, int ret, void (*callback)(int, void *), int uuid)
{
	LookupPetApiBaton *baton = new LookupPetApiBaton();
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

void uv_trader_pet::on_invoke(int event_type, void *_stru, UVPetResqInfo *pRspInfo_org, int nRequestID, bool bIsLast)
{
	UVPetResqInfo *_pRspInfo = NULL;
	if (pRspInfo_org)
	{
		_pRspInfo = new UVPetResqInfo();
		memcpy(_pRspInfo, pRspInfo_org, sizeof(UVPetResqInfo));
	}
	UVPetRtnInfo *field = new UVPetRtnInfo();
	field->work.data = field;
	field->eFlag = event_type;
	field->rtnField = _stru;
	field->rspInfo = (void *)_pRspInfo;
	field->nRequestID = nRequestID;
	field->bIsLast = bIsLast;
	uv_queue_work(uv_default_loop(), &field->work, _on_async, _on_completed);
}
