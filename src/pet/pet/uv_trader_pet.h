#ifndef UV_TRADER_PET_H_
#define UV_TRADER_PET_H_

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <uv.h>
#include <node.h>

#include "uv_wraper_pet_struct.h"

#include "interface/iudge_client_api.h"
#include "nlohmann/json.hpp"

// #define EventUrl_ServPushMsg "ClientCommentRequest"
// #define RequestUrl_PushConfirm "/external/confirmReceiveSubscribe"
// #define RequestUrl_PlaceOrder "/external/order/placeOrder"
// #define RequestUrl_CancelOrder "/external/order/cancelInstructOrder"
// #define RequestUrl_GetFund "/external/broker/getBrokeracct"
// #define RequestUrl_GetPosi "/external/broker/getBrokeracctstock"
// #define RequestUrl_GetOrders "/external/order/getOrder"
// #define RequestUrl_Subscribe "/external/subscribeSubject"

extern bool islog;
void logger_cout(const char *content);
std::string to_string(int val);
std::string charto_string(char val);

class uv_trader_pet : public iudge::client::IudgeClientSpi
{
public:
  uv_trader_pet(void);
  virtual ~uv_trader_pet(void);

  void Connect(UvPetRegisterFrontReqInfo* reqInfo, void(*callback)(int, void*),  int uuid);
  // void OnLogin(UvPetLoginReqInfo *pLoginReqInfo, void(*callback)(int, void*),  int uuid);
  // void OnDisconnected(UvPetDisconnected *pDisconnected, void(*callback)(int, void*),  int uuid);
  
  virtual void OnConnected();
  virtual void OnDisconnected(int nErrorCode);
  virtual void OnEvent(const int nUrlHash, const std::string& strBizBody);
  // void OnEvent(UvPetEventField *pEventFidle, void(*callback)(int, void*),  int uuid);

private:
  static void _async(uv_work_t *work);
  static void _completed(uv_work_t *work, int);
  static void _on_async(uv_work_t *work);
  static void _on_completed(uv_work_t *work, int);
  void invoke(void *field, int ret, void (*callback)(int, void *), int uuid);
  void on_invoke(int event_type, void *_stru, UVPetResqInfo *pRspInfo_org, int nRequestID, bool bIsLast);
  int testAsyncGeneralAction(
				const std::string& strAction,
				std::function<void(const int, const std::string&, const std::string&)>
								fnCallback,
				std::function<void(nlohmann::json&)> fnUserSetting,
				bool bPrintInput);

  std::string m_strToken;
  std::string m_strMac;
  std::string m_strVPRT;

  iudge::client::IudgeClientApi* m_pApi;
  int iRequestID;
  uv_async_t async_t;
  static std::map<int, UVPetWrap *> cb_map;

  const std::string& HOST = "pet.iudge.cn";
  const unsigned int& PORT = 9125;
};

#endif
