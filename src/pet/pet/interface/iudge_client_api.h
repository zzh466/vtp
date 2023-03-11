/**
 * @file iudge_client_api.h
 * @author djange (huangc@iudge.com)
 * @brief
 * @version 0.0.1
 * @date 2021-06-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#include <functional>
#include <string>

#ifdef _WIN32
#ifdef LIB_WRAPED_IUDGE_CLIENT_EXPORT
#define IUDGE_CLIENT_API_EXPORT __declspec(dllexport)
#else
#define IUDGE_CLIENT_API_EXPORT __declspec(dllimport)
#endif
#else
#define IUDGE_CLIENT_API_EXPORT
#endif

namespace iudge {
namespace client {
/**
 * @brief 事件回调接口类,
 *	建议不要在回调类内处理大量耗时逻辑,
 *  最好可以直接拷贝走结果到用户线程中操作
 *  对于使用者不关注的事件可以不继承
 */
class IudgeClientSpi {
public:
  /**
   * @brief 连接成功通知
   *
   * @param nErrorCode
   */
  virtual void OnConnected(){};

  /**
   * @brief 掉线主动通知
   *
   * @param nErrorCode
   */
  virtual void OnDisconnected(int nErrorCode){};

  /**
   * @brief 事件通知
   *
   * @param strBizBody
   */
  virtual void OnEvent(const int nUrlHash, const std::string &strBizBody){};
};

class IUDGE_CLIENT_API_EXPORT IudgeClientApi {
public:
  IudgeClientApi(){};

  /**
   * @brief 初始化底层日志组件 只需要成功调用一次即可
   *		 最开始给到的lib没有日志功能,考虑到使用者使用上的多样性,可能需要放开日志路径,日志名称设置条目
   *		 为了不变更原有逻辑,新增加接口开启文件日志功能.不调用亦不影响使用
   *
   * @return
   */
  static int InitializeLogger(std::string &strErrInfo,
                              const char *pszLogPath = ".",
                              const char *pszLogFile = "libCxxToJ",
                              const unsigned int uLogLevel = 0);

  /**
   * @brief 创建客户端句柄
   *
   * @return IudgeClientApi*
   */
  static IudgeClientApi *CreateClient();

  /**
   * @brief 注册回调接口
   *
   * @param spi 派生自事件回调接口类
   * @param strErrInfo 错误信息返回
   * @return int返回值 (0.调用成功 其他.调用失败)
   */
  virtual int RegistSpi(IudgeClientSpi *spi, std::string &strErrInfo) = 0;

  /**
   * @brief 连接到有迹服务端
   *
   * @param strHost 	 服务器ip
   * @param uPort 	 	 服务器port
   * @param strErrInfo  错误信息返回
   * @param nTimeOut  	 超时时间(毫秒),默认4000
   * @return int 		 返回值 (0.调用成功 其他.调用失败)
   */
  virtual int RegisterFront(const std::string &strHost,
                            const unsigned int &uPort, std::string &strErrInfo,
                            const int nTimeOut = 4 * 1000) = 0;

  /**
   * @brief 发送请求到有迹服务端(同步版本)
   *
   * @param strUrl 		请求url
   * @param strBizBody 	请求包体
   * @param strReplyBody 	回复包体
   * @param strErrInfo 	错误信息
   * @param nTimeOut  	    超时时间(毫秒),默认10000
   * @return int 			返回值 (0.调用成功 其他.调用失败)
   */
  virtual int Request(const std::string &strUrl, const std::string &strBizBody,
                      std::string &strReplyBody, std::string &strErrInfo,
                      const int nTimeOut = 10 * 1000) = 0;

  /**
   * @brief 异步请求响应函数对象原型
   *
   * @param const int nRet 				返回值 (0.调用成功
   * 其他.调用失败)
   * @param const std::string& strBizBody 	回复包体
   * @param const std::string& strErrInfo 	错误信息
   */
  typedef std::function<void(const int, const std::string &,
                             const std::string &)>
      reply_callback_t;

  /**
   * @brief 发送请求到有迹服务端(异步版本)
   *
   * @param strUrl 		请求url
   * @param strBizBody 	请求包体
   * @param fntCallback 	异步请求用户处理回调
   * @param nTimeOut  	    超时时间(毫秒),默认10000
   */
  virtual void Request(const std::string &strUrl, const std::string &strBizBody,
                       reply_callback_t fntCallback,
                       const int nTimeOut = 10 * 1000) = 0;

protected:
  virtual ~IudgeClientApi(){};
};
} // namespace client
} // namespace iudge
