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
 * @brief �¼��ص��ӿ���,
 *	���鲻Ҫ�ڻص����ڴ��������ʱ�߼�,
 *  ��ÿ���ֱ�ӿ����߽�����û��߳��в���
 *  ����ʹ���߲���ע���¼����Բ��̳�
 */
class IudgeClientSpi {
public:
  /**
   * @brief ���ӳɹ�֪ͨ
   *
   * @param nErrorCode
   */
  virtual void OnConnected(){};

  /**
   * @brief ��������֪ͨ
   *
   * @param nErrorCode
   */
  virtual void OnDisconnected(int nErrorCode){};

  /**
   * @brief �¼�֪ͨ
   *
   * @param strBizBody
   */
  virtual void OnEvent(const int nUrlHash, const std::string &strBizBody){};
};

class IUDGE_CLIENT_API_EXPORT IudgeClientApi {
public:
  IudgeClientApi(){};

  /**
   * @brief ��ʼ���ײ���־��� ֻ��Ҫ�ɹ�����һ�μ���
   *		 �ʼ������libû����־����,���ǵ�ʹ����ʹ���ϵĶ�����,������Ҫ�ſ���־·��,��־����������Ŀ
   *		 Ϊ�˲����ԭ���߼�,�����ӽӿڿ����ļ���־����.�������಻Ӱ��ʹ��
   *
   * @return
   */
  static int InitializeLogger(std::string &strErrInfo,
                              const char *pszLogPath = ".",
                              const char *pszLogFile = "libCxxToJ",
                              const unsigned int uLogLevel = 0);

  /**
   * @brief �����ͻ��˾��
   *
   * @return IudgeClientApi*
   */
  static IudgeClientApi *CreateClient();

  /**
   * @brief ע��ص��ӿ�
   *
   * @param spi �������¼��ص��ӿ���
   * @param strErrInfo ������Ϣ����
   * @return int����ֵ (0.���óɹ� ����.����ʧ��)
   */
  virtual int RegistSpi(IudgeClientSpi *spi, std::string &strErrInfo) = 0;

  /**
   * @brief ���ӵ��м������
   *
   * @param strHost 	 ������ip
   * @param uPort 	 	 ������port
   * @param strErrInfo  ������Ϣ����
   * @param nTimeOut  	 ��ʱʱ��(����),Ĭ��4000
   * @return int 		 ����ֵ (0.���óɹ� ����.����ʧ��)
   */
  virtual int RegisterFront(const std::string &strHost,
                            const unsigned int &uPort, std::string &strErrInfo,
                            const int nTimeOut = 4 * 1000) = 0;

  /**
   * @brief ���������м������(ͬ���汾)
   *
   * @param strUrl 		����url
   * @param strBizBody 	�������
   * @param strReplyBody 	�ظ�����
   * @param strErrInfo 	������Ϣ
   * @param nTimeOut  	    ��ʱʱ��(����),Ĭ��10000
   * @return int 			����ֵ (0.���óɹ� ����.����ʧ��)
   */
  virtual int Request(const std::string &strUrl, const std::string &strBizBody,
                      std::string &strReplyBody, std::string &strErrInfo,
                      const int nTimeOut = 10 * 1000) = 0;

  /**
   * @brief �첽������Ӧ��������ԭ��
   *
   * @param const int nRet 				����ֵ (0.���óɹ�
   * ����.����ʧ��)
   * @param const std::string& strBizBody 	�ظ�����
   * @param const std::string& strErrInfo 	������Ϣ
   */
  typedef std::function<void(const int, const std::string &,
                             const std::string &)>
      reply_callback_t;

  /**
   * @brief ���������м������(�첽�汾)
   *
   * @param strUrl 		����url
   * @param strBizBody 	�������
   * @param fntCallback 	�첽�����û�����ص�
   * @param nTimeOut  	    ��ʱʱ��(����),Ĭ��10000
   */
  virtual void Request(const std::string &strUrl, const std::string &strBizBody,
                       reply_callback_t fntCallback,
                       const int nTimeOut = 10 * 1000) = 0;

protected:
  virtual ~IudgeClientApi(){};
};
} // namespace client
} // namespace iudge
