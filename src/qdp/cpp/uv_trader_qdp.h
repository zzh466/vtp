#ifndef UV_TRADER_H_
#define UV_TRADER_H_

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "QdpFtdcTraderApi.h"
#include "QdpFtdcUserApiDataType.h"
#include <uv.h>
#include <node.h>
#include "wraper_struct_qdp.h"

extern bool islog;
void logger_cout(const char *content);
std::string to_string(int val);
std::string charto_string(char val);

class uv_trader : public CQdpFtdcTraderSpi
{
public:
  uv_trader(void);
  virtual ~uv_trader(void);

  const char *GetTradingDay();
  /// 注册
  int On(const char *eName, int cb_type, void (*callback)(CbRtnField *cbResult));
  /// 连接
  void Connect(UVConnectField *pConnectField, void (*callback)(int, void *), int uuid);
  ///客户端认证请求
  void Authenticate(CQdpFtdcAuthenticateField *pReqAuthenticateField, void (*callback)(int, void *), int uuid);
  /// 登录
  void ReqUserLogin(CQdpFtdcReqUserLoginField *pReqUserLoginField, void (*callback)(int, void *), int uuid);
  /// 断开
  void Disconnect();

  // 登出
  void ReqUserLogout(CQdpFtdcReqUserLogoutField *pUserLogout, void (*callback)(int, void *), int uuid);
  // 投资者结算结果确认
  // void  ReqSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, void(*callback)(int, void*),int uuid);
  // 请求查询合约
  void ReqQryInstrument(CQdpFtdcQryInstrumentField *pQryInstrument, void(*callback)(int, void*),int uuid);
  // 请求查询资金账户
  // void ReqQryTradingAccount(CThostFtdcQryTradingAccountField *pQryTradingAccount, void (*callback)(int, void *), int uuid);
  void ReqQryInvestorAccount(CQdpFtdcQryInvestorAccountField *pQryInvestorAccount, void (*callback)(int, void *), int uuid);
  // 请求查询投资者持仓
  void ReqQryInvestorPosition(CQdpFtdcQryInvestorPositionField *pQryInvestorPosition, void(*callback)(int, void*),int uuid);
  // 持仓明细
  // void ReqQryInvestorPositionDetail(CThostFtdcQryInvestorPositionDetailField *pQryInvestorPositionDetail, void(*callback)(int, void*),int uuid);
  // 报单录入请求
  void ReqOrderInsert(CQdpFtdcInputOrderField *pInputOrder, void (*callback)(int, void *), int uuid);
  // 报单操作请求
  void ReqOrderAction(CQdpFtdcOrderActionField *pInputOrderAction, void (*callback)(int, void *), int uuid);
  // 请求查询合约保证率
  // void ReqQryInstrumentMarginRate(CThostFtdcQryInstrumentMarginRateField *pQryInstrumentMarginRate, void(*callback)(int, void*),int uuid);
  // 请求查询行情
  // void ReqQryDepthMarketData(CThostFtdcQryDepthMarketDataField *pQryDepthMarketData, void (*callback)(int, void *), int uuid);
  // 请求查询投资者结算结果
  // void ReqQrySettlementInfo(CThostFtdcQrySettlementInfoField *pQrySettlementInfo, void(*callback)(int, void*), int uuid);
  // void ReqQryInstrumentCommissionRate(CThostFtdcQryInstrumentCommissionRateField *pQryInstrumentCommissionRate, void(*callback)(int, void*),int uuid);
  // void ReqQrySettlementInfoConfirm(CThostFtdcQrySettlementInfoConfirmField *pQrySettlementInfoConfirm, void(*callback)(int, void*),  int uuid);

private:
  static void _async(uv_work_t *work);
  static void _completed(uv_work_t *work, int);
  static void _on_async(uv_work_t *work);
  static void _on_completed(uv_work_t *work, int);
  void invoke(void *field, int ret, void (*callback)(int, void *), int uuid);
  void on_invoke(int event_type, void *_stru, CQdpFtdcRspInfoField *pRspInfo_org, int nRequestID, bool bIsLast);

  ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
  virtual void OnFrontConnected();
  ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
  ///@param nReason 错误原因
  ///        0x1001 网络读失败
  ///        0x1002 网络写失败
  ///        0x2001 接收心跳超时
  ///        0x2002 发送心跳失败
  ///        0x2003 收到错误报文
  virtual void OnFrontDisconnected(int nReason);
  ///客户端认证响应
  virtual void OnRspAuthenticate(CQdpFtdcRtnAuthenticateField *pRspAuthenticateField, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  ///登录请求响应
  virtual void OnRspUserLogin(CQdpFtdcRspUserLoginField *pRspUserLogin, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  ///登出请求响应
  virtual void OnRspUserLogout(CQdpFtdcRspUserLogoutField *pUserLogout, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 投资者结算结果确认响应
  // virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单录入请求响应
  virtual void OnRspOrderInsert(CQdpFtdcInputOrderField *pInputOrder, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单录入错误回报
  virtual void OnErrRtnOrderInsert(CQdpFtdcInputOrderField *pInputOrder, CQdpFtdcRspInfoField *pRspInfo);
  // 报单操作请求响应
  virtual void OnRspOrderAction(CQdpFtdcOrderActionField *pInputOrderAction, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单操作错误回报
  virtual void OnErrRtnOrderAction(CQdpFtdcOrderActionField *pOrderAction, CQdpFtdcRspInfoField *pRspInfo);
  // 请求查询报单响应
  virtual void OnRspQryOrder(CQdpFtdcOrderField *pOrder, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 报单通知
  virtual void OnRtnOrder(CQdpFtdcOrderField *pOrder);
  // 请求查询成交响应
  virtual void OnRspQryTrade(CQdpFtdcTradeField *pTrade, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 成交通知
  virtual void OnRtnTrade(CQdpFtdcTradeField *pTrade);
  // 请求查询投资者持仓响应
  virtual void OnRspQryInvestorPosition(CQdpFtdcRspInvestorPositionField *pInvestorPosition, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询投资者持仓明细响应
  // virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询账户资金响应
  virtual void OnRspQryInvestorAccount(CQdpFtdcRspInvestorAccountField *pRspInvestorAccount, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询合约响应
  // virtual void OnRspQryInstrument(CQdpFtdcRspInstrumentField *pInstrument, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询行情响应
  // virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 请求查询投资者结算结果响应
  // virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  ///投资者手续费率查询应答
  virtual void OnRspQryInvestorFee(CQdpFtdcInvestorFeeField *pInvestorFee, CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  virtual void OnRspError(CQdpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  // 提示条件单校验错误
  // virtual void OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);

  CQdpFtdcTraderApi *m_pApi;
  int iRequestID;
  uv_async_t async_t;
  static std::map<int, CbWrap *> cb_map;
};

#endif
