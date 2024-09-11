/////////////////////////////////////////////////////////////////////////
///@system QuantDo Platform
///@company �Ϻ���Ͷ����Ƽ����޹�˾
///@file QdpFtdcUserApiStruct.h
///@brief �����˿ͻ��˽ӿ�ʹ�õ�ҵ�����ݽṹ
///@history 
/////////////////////////////////////////////////////////////////////////

#if !defined(Qdp_FTDCSTRUCT_H)
#define Qdp_FTDCSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QdpFtdcUserApiDataType.h"

///ϵͳ�û���¼����
struct CQdpFtdcReqUserLoginField
{
	///������
	TQdpFtdcDateType	TradingDay;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����
	TQdpFtdcPasswordType	Password;
	///�û��˲�Ʒ��Ϣ
	TQdpFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TQdpFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TQdpFtdcProtocolInfoType	ProtocolInfo;
	///IP��ַ
	TQdpFtdcIPAddressType	IPAddress;
	///Mac��ַ
	TQdpFtdcMacAddressType	MacAddress;
	///�������Ĵ���
	TQdpFtdcDataCenterIDType	DataCenterID;
	///��̬����
	TQdpFtdcPasswordType	OneTimePassword;
	///�ն�IP��ַ
	TQdpFtdcIPAddressType	ClientIPAddress;
};

///ϵͳ�û���¼Ӧ��
struct CQdpFtdcRspUserLoginField
{
	///������
	TQdpFtdcDateType	TradingDay;
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///��¼�ɹ�ʱ��
	TQdpFtdcTimeType	LoginTime;
	///�û���󱾵ر�����
	TQdpFtdcUserOrderLocalIDType	MaxOrderLocalID;
	///����ϵͳ����
	TQdpFtdcTradingSystemNameType	TradingSystemName;
	///�������Ĵ���
	TQdpFtdcDataCenterIDType	DataCenterID;
	///��Ա˽������ǰ����
	TQdpFtdcSequenceNoType	PrivateFlowSize;
	///����Ա˽������ǰ����
	TQdpFtdcSequenceNoType	UserFlowSize;
	///�Ự���
	TQdpFtdcSessionIDType	SessionID;
	///ǰ�ñ��
	TQdpFtdcFrontIDType	FrontID;
};

///�û��ǳ�����
struct CQdpFtdcReqUserLogoutField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///Session�˳�ԭ��
	TQdpFtdcNumberType	LogoutReason;
};

///�û��ǳ�����
struct CQdpFtdcRspUserLogoutField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///Session�˳�ԭ��
	TQdpFtdcNumberType	LogoutReason;
};

///ǿ���û��˳�
struct CQdpFtdcForceUserExitField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
};

///�û������޸�
struct CQdpFtdcUserPasswordUpdateField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///������
	TQdpFtdcPasswordType	OldPassword;
	///������
	TQdpFtdcPasswordType	NewPassword;
};

///���뱨��
struct CQdpFtdcInputOrderField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�۸�����
	TQdpFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TQdpFtdcDirectionType	Direction;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TQdpFtdcPriceType	LimitPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///��Ч������
	TQdpFtdcTimeConditionType	TimeCondition;
	///GTD����
	TQdpFtdcDateType	GTDDate;
	///�ɽ�������
	TQdpFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TQdpFtdcVolumeType	MinVolume;
	///ֹ���
	TQdpFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TQdpFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TQdpFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TQdpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///Ӫҵ������
	TQdpFtdcBranchIDType	BranchID;
	///��¼���
	TQdpFtdcSequenceNoType	RecNum;
	///ҵ�����
	TQdpFtdcBusinessTypeType	BusinessType;
};

///��������
struct CQdpFtdcOrderActionField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///���γ��������ı��ر��
	TQdpFtdcUserOrderLocalIDType	UserOrderActionLocalID;
	///���������ı��ر������
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///����������־
	TQdpFtdcActionFlagType	ActionFlag;
	///�۸�
	TQdpFtdcPriceType	LimitPrice;
	///�����仯
	TQdpFtdcVolumeType	VolumeChange;
	///��¼���
	TQdpFtdcSequenceNoType	RecNum;
	///ǰ�ñ��
	TQdpFtdcFrontIDType	FrontID;
	///�Ự���
	TQdpFtdcSessionIDType	SessionID;
};

///�ڴ����
struct CQdpFtdcMemDbField
{
	///�ڴ����
	TQdpFtdcMemTableNameType	MemTableName;
};

///Ͷ�����ʽ��ʻ����������
struct CQdpFtdcReqAccountDepositField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountSeqNo;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///�������������ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountInsideSeqNo;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///���
	TQdpFtdcMoneyType	Amount;
	///�������
	TQdpFtdcAccountDirectionType	AmountDirection;
	///�û�����
	TQdpFtdcUserIDType	UserID;
};

///Ͷ�����ʽ��ʻ������Ӧ��
struct CQdpFtdcRspAccountDepositField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///�ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountSeqNo;
	///�������������ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountInsideSeqNo;
	///���
	TQdpFtdcMoneyType	Amount;
	///�������
	TQdpFtdcAccountDirectionType	AmountDirection;
	///�����ʽ�
	TQdpFtdcMoneyType	Available;
	///����׼����
	TQdpFtdcMoneyType	Balance;
	///�û�����
	TQdpFtdcUserIDType	UserID;
};

///���뱨��
struct CQdpFtdcUdpInputOrderField
{
	///���͹�˾���
	TQdpFtdcBrokerIDUdpType	BrokerID;
	///����������
	TQdpFtdcExchangeIDUdpType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDUdpType	InvestorID;
	///�û�����
	TQdpFtdcUserIDUdpType	UserID;
	///��Լ����
	TQdpFtdcInstrumentIDUdpType	InstrumentID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�۸�����
	TQdpFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TQdpFtdcDirectionType	Direction;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TQdpFtdcPriceType	LimitPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///��Ч������
	TQdpFtdcTimeConditionType	TimeCondition;
	///�ɽ�������
	TQdpFtdcVolumeConditionType	VolumeCondition;
	///Ӫҵ������
	TQdpFtdcBranchIDType	BranchID;
	///ֹ���
	TQdpFtdcPriceType	StopPrice;
	///�û��Զ�����
	TQdpFtdcUdpCustomType	UserCustom;
};

///�ͻ���Ϣ����ֵ
struct CQdpFtdcClientMaxSigVolField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱���
	TQdpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����ź���
	TQdpFtdcVolumeType	MaxSigVol;
};

///��Ӧ��Ϣ
struct CQdpFtdcRspInfoField
{
	///�������
	TQdpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TQdpFtdcErrorMsgType	ErrorMsg;
};

///������ѯ
struct CQdpFtdcQryOrderField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///�ɽ���ѯ
struct CQdpFtdcQryTradeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ɽ����
	TQdpFtdcTradeIDType	TradeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///��Լ��ѯ
struct CQdpFtdcQryInstrumentField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ʒ�ִ���
	TQdpFtdcProductIDType	ProductID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///��Լ��ѯӦ��
struct CQdpFtdcRspInstrumentField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ʒ�ִ���
	TQdpFtdcProductIDType	ProductID;
	///Ʒ������
	TQdpFtdcProductNameType	ProductName;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��Լ����
	TQdpFtdcInstrumentNameType	InstrumentName;
	///�������
	TQdpFtdcYearType	DeliveryYear;
	///������
	TQdpFtdcMonthType	DeliveryMonth;
	///�޼۵�����µ���
	TQdpFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TQdpFtdcVolumeType	MinLimitOrderVolume;
	///�м۵�����µ���
	TQdpFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TQdpFtdcVolumeType	MinMarketOrderVolume;
	///��������
	TQdpFtdcVolumeMultipleType	VolumeMultiple;
	///���۵�λ
	TQdpFtdcPriceTickType	PriceTick;
	///����
	TQdpFtdcCurrencyType	Currency;
	///��ͷ�޲�
	TQdpFtdcVolumeType	LongPosLimit;
	///��ͷ�޲�
	TQdpFtdcVolumeType	ShortPosLimit;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///��Լ����״̬
	TQdpFtdcInstrumentStatusType	InstrumentStatus;
	///������
	TQdpFtdcDateType	CreateDate;
	///������
	TQdpFtdcDateType	OpenDate;
	///������
	TQdpFtdcDateType	ExpireDate;
	///��ʼ������
	TQdpFtdcDateType	StartDelivDate;
	///��󽻸���
	TQdpFtdcDateType	EndDelivDate;
	///���ƻ�׼��
	TQdpFtdcPriceType	BasisPrice;
	///��ǰ�Ƿ���
	TQdpFtdcBoolType	IsTrading;
	///������Ʒ����
	TQdpFtdcInstrumentIDType	UnderlyingInstrID;
	///������Ʒ����
	TQdpFtdcUnderlyingMultipleType	UnderlyingMultiple;
	///�ֲ�����
	TQdpFtdcPositionTypeType	PositionType;
	///ִ�м�
	TQdpFtdcPriceType	StrikePrice;
	///��Ȩ����
	TQdpFtdcOptionsTypeType	OptionsType;
	///��Ʒ����
	TQdpFtdcProductClassType	ProductClass;
	///��Ȩ��Ȩ��ʽ
	TQdpFtdcOptionsModeType	OptionsMode;
};

///��Լ״̬
struct CQdpFtdcInstrumentStatusField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ʒ�ִ���
	TQdpFtdcProductIDType	ProductID;
	///Ʒ������
	TQdpFtdcProductNameType	ProductName;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��Լ����
	TQdpFtdcInstrumentNameType	InstrumentName;
	///�������
	TQdpFtdcYearType	DeliveryYear;
	///������
	TQdpFtdcMonthType	DeliveryMonth;
	///�޼۵�����µ���
	TQdpFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TQdpFtdcVolumeType	MinLimitOrderVolume;
	///�м۵�����µ���
	TQdpFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TQdpFtdcVolumeType	MinMarketOrderVolume;
	///��������
	TQdpFtdcVolumeMultipleType	VolumeMultiple;
	///���۵�λ
	TQdpFtdcPriceTickType	PriceTick;
	///����
	TQdpFtdcCurrencyType	Currency;
	///��ͷ�޲�
	TQdpFtdcVolumeType	LongPosLimit;
	///��ͷ�޲�
	TQdpFtdcVolumeType	ShortPosLimit;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///��Լ����״̬
	TQdpFtdcInstrumentStatusType	InstrumentStatus;
	///������
	TQdpFtdcDateType	CreateDate;
	///������
	TQdpFtdcDateType	OpenDate;
	///������
	TQdpFtdcDateType	ExpireDate;
	///��ʼ������
	TQdpFtdcDateType	StartDelivDate;
	///��󽻸���
	TQdpFtdcDateType	EndDelivDate;
	///���ƻ�׼��
	TQdpFtdcPriceType	BasisPrice;
	///��ǰ�Ƿ���
	TQdpFtdcBoolType	IsTrading;
	///������Ʒ����
	TQdpFtdcInstrumentIDType	UnderlyingInstrID;
	///������Ʒ����
	TQdpFtdcUnderlyingMultipleType	UnderlyingMultiple;
	///�ֲ�����
	TQdpFtdcPositionTypeType	PositionType;
	///ִ�м�
	TQdpFtdcPriceType	StrikePrice;
	///��Ȩ����
	TQdpFtdcOptionsTypeType	OptionsType;
	///��Ʒ����
	TQdpFtdcProductClassType	ProductClass;
	///��Ȩ��Ȩ��ʽ
	TQdpFtdcOptionsModeType	OptionsMode;
};

///Ͷ�����ʽ��ѯ
struct CQdpFtdcQryInvestorAccountField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
};

///Ͷ�����ʽ�Ӧ��
struct CQdpFtdcRspInvestorAccountField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///�ϴν���׼����
	TQdpFtdcMoneyType	PreBalance;
	///���տ����ʽ�
	TQdpFtdcMoneyType	PreAvailable;
	///�����
	TQdpFtdcMoneyType	Deposit;
	///������
	TQdpFtdcMoneyType	Withdraw;
	///ռ�ñ�֤��
	TQdpFtdcMoneyType	Margin;
	///��ȨȨ������֧
	TQdpFtdcMoneyType	Premium;
	///������
	TQdpFtdcMoneyType	Fee;
	///����ı�֤��
	TQdpFtdcMoneyType	FrozenMargin;
	///����Ȩ����
	TQdpFtdcMoneyType	FrozenPremium;
	///����������
	TQdpFtdcMoneyType	FrozenFee;
	///ƽ��ӯ��
	TQdpFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TQdpFtdcMoneyType	PositionProfit;
	///�����ʽ�
	TQdpFtdcMoneyType	Available;
	///����׼����
	TQdpFtdcMoneyType	Balance;
	///��ͷռ�ñ�֤��
	TQdpFtdcMoneyType	LongMargin;
	///��ͷռ�ñ�֤��
	TQdpFtdcMoneyType	ShortMargin;
	///��ͷ����ı�֤��
	TQdpFtdcMoneyType	LongFrozenMargin;
	///��ͷ����ı�֤��
	TQdpFtdcMoneyType	ShortFrozenMargin;
	///��̬Ȩ��
	TQdpFtdcMoneyType	DynamicRights;
	///���ն�
	TQdpFtdcMoneyType	Risk;
	///��������
	TQdpFtdcMoneyType	OtherFee;
	///��Ѻ���
	TQdpFtdcMoneyType	Mortgage;
	///����
	TQdpFtdcCurrencyIDType	Currency;
};

///����Ͷ���߲�ѯ
struct CQdpFtdcQryUserInvestorField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
};

///����Ͷ����Ӧ��
struct CQdpFtdcRspUserInvestorField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
};

///���ױ����ѯ
struct CQdpFtdcQryTradingCodeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
};

///���ױ����ѯ
struct CQdpFtdcRspTradingCodeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Ա���
	TQdpFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TQdpFtdcClientIDType	ClientID;
	///�ͻ�����Ȩ��
	TQdpFtdcTradingRightType	ClientRight;
	///�ͻ�����
	TQdpFtdcClientTypeType	ClientType;
	///�ͻ���ֵ����
	TQdpFtdcHedgeFlagType	ClientHedgeFlag;
	///�Ƿ��Ծ
	TQdpFtdcIsActiveType	IsActive;
};

///��������ѯ����
struct CQdpFtdcQryExchangeField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
};

///��������ѯӦ��
struct CQdpFtdcRspExchangeField
{
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///����������
	TQdpFtdcExchangeNameType	ExchangeName;
};

///Ͷ���ֲֲ߳�ѯ����
struct CQdpFtdcQryInvestorPositionField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///Ͷ���ֲֲ߳�ѯӦ��
struct CQdpFtdcRspInvestorPositionField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///��Ա���
	TQdpFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TQdpFtdcClientIDType	ClientID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��������
	TQdpFtdcDirectionType	Direction;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///ռ�ñ�֤��
	TQdpFtdcMoneyType	UsedMargin;
	///�ֲܳ���
	TQdpFtdcVolumeType	Position;
	///���ճֲֳɱ�
	TQdpFtdcPriceType	PositionCost;
	///��ֲ���(����ֵ)
	TQdpFtdcVolumeType	YdPosition;
	///���ճֲֳɱ�
	TQdpFtdcMoneyType	YdPositionCost;
	///����ı�֤��
	TQdpFtdcMoneyType	FrozenMargin;
	///���ֶ���ֲ�
	TQdpFtdcVolumeType	FrozenPosition;
	///ƽ�ֶ���ֲ�
	TQdpFtdcVolumeType	FrozenClosing;
	///�����Ȩ����
	TQdpFtdcMoneyType	FrozenPremium;
	///���һ�ʳɽ����
	TQdpFtdcTradeIDType	LastTradeID;
	///���һ�ʱ��ر������
	TQdpFtdcOrderLocalIDType	LastOrderLocalID;
	///����
	TQdpFtdcCurrencyIDType	Currency;
	///�ֲ�ӯ��
	TQdpFtdcMoneyType	PositionProfit;
	///��ֲ���
	TQdpFtdcVolumeType	TodayPosition;
	///���ƽ�ֶ���
	TQdpFtdcVolumeType	FrozenTodayClosing;
};

///�û���ѯ
struct CQdpFtdcQryUserField
{
	///�����û�����
	TQdpFtdcUserIDType	StartUserID;
	///�����û�����
	TQdpFtdcUserIDType	EndUserID;
};

///�û�
struct CQdpFtdcUserField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///�û���¼����
	TQdpFtdcPasswordType	Password;
	///�Ƿ��Ծ
	TQdpFtdcIsActiveType	IsActive;
	///�û�����
	TQdpFtdcUserNameType	UserName;
	///�û�����
	TQdpFtdcUserTypeType	UserType;
	///Ӫҵ��
	TQdpFtdcDepartmentType	Department;
	///��Ȩ���ܼ�
	TQdpFtdcGrantFuncSetType	GrantFuncSet;
	///�Ƿ���IP��MAC
	TQdpFtdcBoolType	CheckIpMacAddr;
};

///Ͷ�����������ʲ�ѯ
struct CQdpFtdcQryInvestorFeeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
};

///Ͷ������������
struct CQdpFtdcInvestorFeeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱���
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///���������Ѱ�����
	TQdpFtdcRatioType	OpenFeeRate;
	///���������Ѱ�����
	TQdpFtdcRatioType	OpenFeeAmt;
	///ƽ�������Ѱ�����
	TQdpFtdcRatioType	OffsetFeeRate;
	///ƽ�������Ѱ�����
	TQdpFtdcRatioType	OffsetFeeAmt;
	///ƽ��������Ѱ�����
	TQdpFtdcRatioType	OTFeeRate;
	///ƽ��������Ѱ�����
	TQdpFtdcRatioType	OTFeeAmt;
};

///Ͷ���߱�֤���ʲ�ѯ
struct CQdpFtdcQryInvestorMarginField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
};

///Ͷ���߱�֤����
struct CQdpFtdcInvestorMarginField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱���
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///��ͷռ�ñ�֤�𰴱���
	TQdpFtdcRatioType	LongMarginRate;
	///��ͷ��֤������
	TQdpFtdcRatioType	LongMarginAmt;
	///��ͷռ�ñ�֤�𰴱���
	TQdpFtdcRatioType	ShortMarginRate;
	///��ͷ��֤������
	TQdpFtdcRatioType	ShortMarginAmt;
};

///ʵʱ�����ѯ����
struct CQdpFtdcQryMarketDataField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///ʵʱ�����ѯӦ��
struct CQdpFtdcMarketDataField
{
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����
	TQdpFtdcPriceType	OpenPrice;
	///��߼�
	TQdpFtdcPriceType	HighestPrice;
	///��ͼ�
	TQdpFtdcPriceType	LowestPrice;
	///���¼�
	TQdpFtdcPriceType	LastPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///�ɽ����
	TQdpFtdcMoneyType	Turnover;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///�ֲ���
	TQdpFtdcVolumeType	OpenInterest;
	///������
	TQdpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TQdpFtdcVolumeType	PreOpenInterest;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
};

///�����ѯӦ��
struct CQdpFtdcRspMarketDataField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///���������
	TQdpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TQdpFtdcSettlementIDType	SettlementID;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///������
	TQdpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TQdpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TQdpFtdcRatioType	PreDelta;
	///����
	TQdpFtdcPriceType	OpenPrice;
	///��߼�
	TQdpFtdcPriceType	HighestPrice;
	///��ͼ�
	TQdpFtdcPriceType	LowestPrice;
	///������
	TQdpFtdcPriceType	ClosePrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TQdpFtdcRatioType	CurrDelta;
	///���¼�
	TQdpFtdcPriceType	LastPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///�ɽ����
	TQdpFtdcMoneyType	Turnover;
	///�ֲ���
	TQdpFtdcLargeVolumeType	OpenInterest;
	///�����һ
	TQdpFtdcPriceType	BidPrice1;
	///������һ
	TQdpFtdcVolumeType	BidVolume1;
	///������һ
	TQdpFtdcPriceType	AskPrice1;
	///������һ
	TQdpFtdcVolumeType	AskVolume1;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
};

///�������ӷ��ʲ�ѯ����
struct CQdpFtdcQrySGEDeferRateField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///�������ӷ��ʲ�ѯӦ��
struct CQdpFtdcSGEDeferRateField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��������
	TQdpFtdcTradingDayType	TradeDate;
	///֧������
	TQdpFtdcPayDirectionType	Direction;
	///���ӷ���
	TQdpFtdcRatioType	DeferRate;
};

///�ּ۱��ѯ����
struct CQdpFtdcQryMBLMarketDataField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��ʼ��Լ����
	TQdpFtdcInstrumentIDType	InstIDStart;
	///������Լ����
	TQdpFtdcInstrumentIDType	InstIDEnd;
	///��������
	TQdpFtdcDirectionType	Direction;
};

///�ּ۱��ѯӦ��
struct CQdpFtdcMBLMarketDataField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��������
	TQdpFtdcDirectionType	Direction;
	///�۸�
	TQdpFtdcPriceType	Price;
	///����
	TQdpFtdcVolumeType	Volume;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
};

///��ѯ�ֲ���ϸ����
struct CQdpFtdcQryInvestorPositionDetailField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///���Ⱥ�Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///��ѯ�ֲ���ϸӦ��
struct CQdpFtdcRspInvestorPositionDetailField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///��������
	TQdpFtdcDirectionType	Direction;
	///��������
	TQdpFtdcDateType	OpenDate;
	///�ɽ����
	TQdpFtdcTradeIDType	TradeID;
	///����
	TQdpFtdcVolumeType	Volume;
	///���ּ�
	TQdpFtdcMoneyType	OpenPrice;
	///������
	TQdpFtdcDateType	TradingDay;
	///��������
	TQdpFtdcTradeTypeType	TradeType;
	///��Ϻ�Լ����
	TQdpFtdcInstrumentIDType	CombInstrumentID;
	///���ն���ƽ��ӯ��
	TQdpFtdcMoneyType	CloseProfitByDate;
	///��ʶԳ�ƽ��ӯ��
	TQdpFtdcMoneyType	CloseProfitByTrade;
	///���ն��гֲ�ӯ��
	TQdpFtdcMoneyType	PositionProfitByDate;
	///��ʶԳ�ֲ�ӯ��
	TQdpFtdcMoneyType	PositionProfitByTrade;
	///Ͷ���߱�֤��
	TQdpFtdcMoneyType	Margin;
	///��������֤��
	TQdpFtdcMoneyType	ExchMargin;
	///��֤����
	TQdpFtdcMoneyType	MarginRateByMoney;
	///��֤����(������)
	TQdpFtdcMoneyType	MarginRateByVolume;
	///������
	TQdpFtdcPriceType	LastSettlementPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///ƽ����
	TQdpFtdcVolumeType	CloseVolume;
	///ƽ�ֽ��
	TQdpFtdcMoneyType	CloseAmount;
};

///Ͷ������Ȩ�������ʲ�ѯ
struct CQdpFtdcQryInvestorOptionFeeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///Ͷ������Ȩ��������
struct CQdpFtdcRspInvestorOptionFeeField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱���
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///���������Ѱ�����
	TQdpFtdcRatioType	OpenFeeRate;
	///���������Ѱ�����
	TQdpFtdcRatioType	OpenFeeAmt;
	///ƽ�������Ѱ�����
	TQdpFtdcRatioType	OffsetFeeRate;
	///ƽ�������Ѱ�����
	TQdpFtdcRatioType	OffsetFeeAmt;
	///ƽ��������Ѱ�����
	TQdpFtdcRatioType	OTFeeRate;
	///ƽ��������Ѱ�����
	TQdpFtdcRatioType	OTFeeAmt;
	///ִ�������Ѱ�����
	TQdpFtdcRatioType	StrikeFeeRate;
	///ִ�������Ѱ�����
	TQdpFtdcRatioType	StrikeFeeAmt;
};

///QDPǰ��ͨ����Ϣ
struct CQdpFtdcFrontInfoField
{
	///������API��ǰ�ñ��
	TQdpFtdcBranchIDType	BranchID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///ϯλ��
	TQdpFtdcSeatIDType	SeatID;
	///���ӵ�ַ
	TQdpFtdcFrontAddrType	FrontAddr;
};

///��ѯQDPǰ��ͨ����Ϣ
struct CQdpFtdcQryFrontInfoField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
};

///Ͷ���ֲ߳��޶��ѯ
struct CQdpFtdcQryInvestorPositionLimitField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///Ͷ���ֲ߳��޶�
struct CQdpFtdcRspInvestorPositionLimitField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��ͷ�ֲ��޶�
	TQdpFtdcVolumeType	LongPosiLimit;
	///��ͷ�ֲ��޶�
	TQdpFtdcVolumeType	ShortPosiLimit;
	///Ʒ�ֶ�ͷ�ֲ�
	TQdpFtdcVolumeType	LongPosition;
	///Ʒ�ֶ�ͷ����ֲ�
	TQdpFtdcVolumeType	LongFrozen;
	///Ʒ�ֿ�ͷ�ֲ�
	TQdpFtdcVolumeType	ShortPosition;
	///Ʒ�ֿ�ͷ����ֲ�
	TQdpFtdcVolumeType	ShortFrozen;
};

///���׻��ʲ�ѯ
struct CQdpFtdcQryExchangeRateField
{
	///����
	TQdpFtdcCurrencyIDType	InstCurrency;
	///����
	TQdpFtdcCurrencyIDType	AccCurrency;
};

///���׻���֪ͨ
struct CQdpFtdcRspExchangeRateField
{
	///����
	TQdpFtdcCurrencyIDType	InstCurrency;
	///����
	TQdpFtdcCurrencyIDType	AccCurrency;
	///����
	TQdpFtdcRatioType	ExchangeRate;
};

///ѯ��¼��
struct CQdpFtdcInputForQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///���ر������
	TQdpFtdcOrderLocalIDType	OrderLocalID;
};

///����¼��
struct CQdpFtdcInputQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///ϵͳ�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///Ӧ�۱��
	TQdpFtdcOrderSysIDType	ForQuoteSysID;
	///���۸�
	TQdpFtdcPriceType	AskPrice;
	///������
	TQdpFtdcVolumeType	AskVolume;
	///����ƽ��־
	TQdpFtdcOffsetFlagType	AskOffsetFlag;
	///��Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	AskHedgeFlag;
	///��۸�
	TQdpFtdcPriceType	BidPrice;
	///������
	TQdpFtdcVolumeType	BidVolume;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	BidOffsetFlag;
	///��Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	BidHedgeFlag;
	///ҵ��Ԫ
	TQdpFtdcBusinessUnitType	BusinessUnit;
	///Ӫҵ������
	TQdpFtdcBranchIDType	BranchID;
	///�򷽱��ر������
	TQdpFtdcUserOrderLocalIDType	BidOrderRef;
	///�������ر������
	TQdpFtdcUserOrderLocalIDType	AskOrderRef;
};

///ѯ����֪ͨ
struct CQdpFtdcForQuoteRspField
{
	///������
	TQdpFtdcDateType	TradingDay;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///ѯ�۱��
	TQdpFtdcOrderSysIDType	OrderSysID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///����ʱ��
	TQdpFtdcTimeType	InsertTime;
};

///����֪ͨ
struct CQdpFtdcQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///ϵͳ�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///Ӧ�۱��
	TQdpFtdcOrderSysIDType	ForQuoteSysID;
	///���۸�
	TQdpFtdcPriceType	AskPrice;
	///������
	TQdpFtdcVolumeType	AskVolume;
	///����ƽ��־
	TQdpFtdcOffsetFlagType	AskOffsetFlag;
	///��Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	AskHedgeFlag;
	///��۸�
	TQdpFtdcPriceType	BidPrice;
	///������
	TQdpFtdcVolumeType	BidVolume;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	BidOffsetFlag;
	///��Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	BidHedgeFlag;
	///ҵ��Ԫ
	TQdpFtdcBusinessUnitType	BusinessUnit;
	///Ӫҵ������
	TQdpFtdcBranchIDType	BranchID;
	///�򷽱��ر������
	TQdpFtdcUserOrderLocalIDType	BidOrderRef;
	///�������ر������
	TQdpFtdcUserOrderLocalIDType	AskOrderRef;
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///��Ա���
	TQdpFtdcParticipantIDType	ParticipantID;
	///�ͻ���
	TQdpFtdcClientIDType	ClientID;
	///�µ�ϯλ��
	TQdpFtdcSeatIDType	SeatID;
	///����ʱ��
	TQdpFtdcTimeType	InsertTime;
	///���ر������
	TQdpFtdcOrderLocalIDType	OrderLocalID;
	///������Դ
	TQdpFtdcOrderSourceType	OrderSource;
	///����״̬
	TQdpFtdcOrderStatusType	OrderStatus;
	///����ʱ��
	TQdpFtdcTimeType	CancelTime;
	///�����û����
	TQdpFtdcUserIDType	CancelUserID;
	///��ɽ�����
	TQdpFtdcVolumeType	VolumeTraded;
	///ʣ������
	TQdpFtdcVolumeType	VolumeRemain;
	///ǰ�ñ��
	TQdpFtdcFrontIDType	FrontID;
	///�Ự���
	TQdpFtdcSessionIDType	SessionID;
	///����������
	TQdpFtdcMoneyType	FrozenFee;
	///����ı�֤��
	TQdpFtdcMoneyType	FrozenMargin;
	///��ȨȨ������֧
	TQdpFtdcMoneyType	FrozenPremium;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///�µ��û����
	TQdpFtdcUserIDType	OrderUserID;
	///��������
	TQdpFtdcTradeTypeType	TradeType;
	///�����־
	TQdpFtdcDealFlagType	DealFlag;
	///�ѳɽ�Ӷ��
	TQdpFtdcMoneyType	TradeCommision;
	///�ɽ����
	TQdpFtdcMoneyType	TradeAmnt;
	///��������
	TQdpFtdcVolumeType	CancleVolume;
	///������
	TQdpFtdcRequestIDType	RequestID;
	///ͨ����Ӧ��APIID
	TQdpFtdcNumberType	APIID;
	///�µ�IP��ַ
	TQdpFtdcIPAddressType	IPAddress;
	///�µ�MAC��ַ
	TQdpFtdcMacAddressType	MacAddress;
	///��ϵͳ�������
	TQdpFtdcOrderSysIDType	BidOrderSysID;
	///����ϵͳ�������
	TQdpFtdcOrderSysIDType	AskOrderSysID;
};

///���۲���
struct CQdpFtdcQuoteActionField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///���γ��������ı��ر��
	TQdpFtdcUserOrderLocalIDType	UserOrderActionLocalID;
	///���������ı��ر������
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///����������־
	TQdpFtdcActionFlagType	ActionFlag;
	///�۸�
	TQdpFtdcPriceType	LimitPrice;
	///�����仯
	TQdpFtdcVolumeType	VolumeChange;
	///��¼���
	TQdpFtdcSequenceNoType	RecNum;
	///ǰ�ñ��
	TQdpFtdcFrontIDType	FrontID;
	///�Ự���
	TQdpFtdcSessionIDType	SessionID;
};

///ѯ�۲�ѯ
struct CQdpFtdcQryForQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
};

///ѯ�۲�ѯӦ��
struct CQdpFtdcForQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///���ر������
	TQdpFtdcOrderLocalIDType	OrderLocalID;
	///ѯ��״̬
	TQdpFtdcForQuoteStatusTypeType	ForQuoteStatus;
	///������
	TQdpFtdcDateType	TradingDay;
	///����ʱ��
	TQdpFtdcTimeType	InsertTime;
};

///���۲�ѯ
struct CQdpFtdcQryQuoteField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TQdpFtdcOrderSysIDType	OrderSysID;
};

///�ɽ�
struct CQdpFtdcTradeField
{
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Ա���
	TQdpFtdcParticipantIDType	ParticipantID;
	///�µ�ϯλ��
	TQdpFtdcSeatIDType	SeatID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ͻ���
	TQdpFtdcClientIDType	ClientID;
	///�û����
	TQdpFtdcUserIDType	UserID;
	///�ɽ����
	TQdpFtdcTradeIDType	TradeID;
	///�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///���ر������
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��������
	TQdpFtdcDirectionType	Direction;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///�ɽ��۸�
	TQdpFtdcPriceType	TradePrice;
	///�ɽ�����
	TQdpFtdcVolumeType	TradeVolume;
	///�ɽ�ʱ��
	TQdpFtdcTimeType	TradeTime;
	///�����Ա���
	TQdpFtdcParticipantIDType	ClearingPartID;
	///�ɽ����
	TQdpFtdcMoneyType	TradeAmnt;
	///��¼���
	TQdpFtdcSequenceNoType	RecNum;
	///��������
	TQdpFtdcTradeTypeType	TradeType;
};

///����
struct CQdpFtdcOrderField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TQdpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TQdpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///�۸�����
	TQdpFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TQdpFtdcDirectionType	Direction;
	///��ƽ��־
	TQdpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TQdpFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TQdpFtdcPriceType	LimitPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///��Ч������
	TQdpFtdcTimeConditionType	TimeCondition;
	///GTD����
	TQdpFtdcDateType	GTDDate;
	///�ɽ�������
	TQdpFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TQdpFtdcVolumeType	MinVolume;
	///ֹ���
	TQdpFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TQdpFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TQdpFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TQdpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TQdpFtdcCustomType	UserCustom;
	///Ӫҵ������
	TQdpFtdcBranchIDType	BranchID;
	///��¼���
	TQdpFtdcSequenceNoType	RecNum;
	///ҵ�����
	TQdpFtdcBusinessTypeType	BusinessType;
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///��Ա���
	TQdpFtdcParticipantIDType	ParticipantID;
	///�ͻ���
	TQdpFtdcClientIDType	ClientID;
	///�µ�ϯλ��
	TQdpFtdcSeatIDType	SeatID;
	///����ʱ��
	TQdpFtdcTimeType	InsertTime;
	///���ر������
	TQdpFtdcOrderLocalIDType	OrderLocalID;
	///������Դ
	TQdpFtdcOrderSourceType	OrderSource;
	///����״̬
	TQdpFtdcOrderStatusType	OrderStatus;
	///����ʱ��
	TQdpFtdcTimeType	CancelTime;
	///�����û����
	TQdpFtdcUserIDType	CancelUserID;
	///��ɽ�����
	TQdpFtdcVolumeType	VolumeTraded;
	///ʣ������
	TQdpFtdcVolumeType	VolumeRemain;
	///ǰ�ñ��
	TQdpFtdcFrontIDType	FrontID;
	///�Ự���
	TQdpFtdcSessionIDType	SessionID;
};

///����������
struct CQdpFtdcFlowMessageCancelField
{
	///����ϵ�к�
	TQdpFtdcSequenceSeriesType	SequenceSeries;
	///������
	TQdpFtdcDateType	TradingDay;
	///�������Ĵ���
	TQdpFtdcDataCenterIDType	DataCenterID;
	///������ʼ���к�
	TQdpFtdcSequenceNoType	StartSequenceNo;
	///���˽������к�
	TQdpFtdcSequenceNoType	EndSequenceNo;
};

///��Ϣ�ַ�
struct CQdpFtdcDisseminationField
{
	///����ϵ�к�
	TQdpFtdcSequenceSeriesType	SequenceSeries;
	///���к�
	TQdpFtdcSequenceNoType	SequenceNo;
};

///�������
struct CQdpFtdcInvestorAccountDepositResField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TQdpFtdcAccountIDType	AccountID;
	///�ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountSeqNo;
	///�������������ʽ���ˮ��
	TQdpFtdcAccountSeqNoType	AccountInsideSeqNo;
	///���
	TQdpFtdcMoneyType	Amount;
	///�������
	TQdpFtdcAccountDirectionType	AmountDirection;
	///�����ʽ�
	TQdpFtdcMoneyType	Available;
	///����׼����
	TQdpFtdcMoneyType	Balance;
	///�û�����
	TQdpFtdcUserIDType	UserID;
};

///QDP������Ϣ֪ͨ
struct CQdpFtdcMessageNotifyInfoField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�޸��û����
	TQdpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TQdpFtdcInvestorIDType	InvestorID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///�ۼӴ���
	TQdpFtdcTotalNumsType	Nums;
	///ҵ�����
	TQdpFtdcBusinessTypeType	BusinessType;
	///������Ϣ
	TQdpFtdcErrorMsgType	WarnMsg;
};

///�����������
struct CQdpFtdcMarketDataBaseField
{
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///���������
	TQdpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TQdpFtdcSettlementIDType	SettlementID;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///������
	TQdpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TQdpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TQdpFtdcRatioType	PreDelta;
};

///���龲̬����
struct CQdpFtdcMarketDataStaticField
{
	///����
	TQdpFtdcPriceType	OpenPrice;
	///��߼�
	TQdpFtdcPriceType	HighestPrice;
	///��ͼ�
	TQdpFtdcPriceType	LowestPrice;
	///������
	TQdpFtdcPriceType	ClosePrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TQdpFtdcRatioType	CurrDelta;
};

///�������³ɽ�����
struct CQdpFtdcMarketDataLastMatchField
{
	///���¼�
	TQdpFtdcPriceType	LastPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///�ɽ����
	TQdpFtdcMoneyType	Turnover;
	///�ֲ���
	TQdpFtdcLargeVolumeType	OpenInterest;
};

///�������ż�����
struct CQdpFtdcMarketDataBestPriceField
{
	///�����һ
	TQdpFtdcPriceType	BidPrice1;
	///������һ
	TQdpFtdcVolumeType	BidVolume1;
	///������һ
	TQdpFtdcPriceType	AskPrice1;
	///������һ
	TQdpFtdcVolumeType	AskVolume1;
};

///�����������������
struct CQdpFtdcMarketDataBid23Field
{
	///����۶�
	TQdpFtdcPriceType	BidPrice2;
	///��������
	TQdpFtdcVolumeType	BidVolume2;
	///�������
	TQdpFtdcPriceType	BidPrice3;
	///��������
	TQdpFtdcVolumeType	BidVolume3;
};

///������������������
struct CQdpFtdcMarketDataAsk23Field
{
	///�����۶�
	TQdpFtdcPriceType	AskPrice2;
	///��������
	TQdpFtdcVolumeType	AskVolume2;
	///��������
	TQdpFtdcPriceType	AskPrice3;
	///��������
	TQdpFtdcVolumeType	AskVolume3;
};

///���������ġ�������
struct CQdpFtdcMarketDataBid45Field
{
	///�������
	TQdpFtdcPriceType	BidPrice4;
	///��������
	TQdpFtdcVolumeType	BidVolume4;
	///�������
	TQdpFtdcPriceType	BidPrice5;
	///��������
	TQdpFtdcVolumeType	BidVolume5;
};

///���������ġ�������
struct CQdpFtdcMarketDataAsk45Field
{
	///��������
	TQdpFtdcPriceType	AskPrice4;
	///��������
	TQdpFtdcVolumeType	AskVolume4;
	///��������
	TQdpFtdcPriceType	AskPrice5;
	///��������
	TQdpFtdcVolumeType	AskVolume5;
};

///�������ʱ������
struct CQdpFtdcMarketDataUpdateTimeField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
};

///�������
struct CQdpFtdcDepthMarketDataField
{
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///���������
	TQdpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TQdpFtdcSettlementIDType	SettlementID;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///������
	TQdpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TQdpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TQdpFtdcRatioType	PreDelta;
	///����
	TQdpFtdcPriceType	OpenPrice;
	///��߼�
	TQdpFtdcPriceType	HighestPrice;
	///��ͼ�
	TQdpFtdcPriceType	LowestPrice;
	///������
	TQdpFtdcPriceType	ClosePrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TQdpFtdcRatioType	CurrDelta;
	///���¼�
	TQdpFtdcPriceType	LastPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///�ɽ����
	TQdpFtdcMoneyType	Turnover;
	///�ֲ���
	TQdpFtdcLargeVolumeType	OpenInterest;
	///�����һ
	TQdpFtdcPriceType	BidPrice1;
	///������һ
	TQdpFtdcVolumeType	BidVolume1;
	///������һ
	TQdpFtdcPriceType	AskPrice1;
	///������һ
	TQdpFtdcVolumeType	AskVolume1;
	///����۶�
	TQdpFtdcPriceType	BidPrice2;
	///��������
	TQdpFtdcVolumeType	BidVolume2;
	///�������
	TQdpFtdcPriceType	BidPrice3;
	///��������
	TQdpFtdcVolumeType	BidVolume3;
	///�����۶�
	TQdpFtdcPriceType	AskPrice2;
	///��������
	TQdpFtdcVolumeType	AskVolume2;
	///��������
	TQdpFtdcPriceType	AskPrice3;
	///��������
	TQdpFtdcVolumeType	AskVolume3;
	///�������
	TQdpFtdcPriceType	BidPrice4;
	///��������
	TQdpFtdcVolumeType	BidVolume4;
	///�������
	TQdpFtdcPriceType	BidPrice5;
	///��������
	TQdpFtdcVolumeType	BidVolume5;
	///��������
	TQdpFtdcPriceType	AskPrice4;
	///��������
	TQdpFtdcVolumeType	AskVolume4;
	///��������
	TQdpFtdcPriceType	AskPrice5;
	///��������
	TQdpFtdcVolumeType	AskVolume5;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��������
	TQdpFtdcPriceType	AskPrice6;
	///��������
	TQdpFtdcVolumeType	AskVolume6;
	///��������
	TQdpFtdcPriceType	AskPrice7;
	///��������
	TQdpFtdcVolumeType	AskVolume7;
	///�����۰�
	TQdpFtdcPriceType	AskPrice8;
	///��������
	TQdpFtdcVolumeType	AskVolume8;
	///�����۾�
	TQdpFtdcPriceType	AskPrice9;
	///��������
	TQdpFtdcVolumeType	AskVolume9;
	///�������
	TQdpFtdcPriceType	BidPrice6;
	///��������
	TQdpFtdcVolumeType	BidVolume6;
	///�������
	TQdpFtdcPriceType	BidPrice7;
	///��������
	TQdpFtdcVolumeType	BidVolume7;
	///����۰�
	TQdpFtdcPriceType	BidPrice8;
	///��������
	TQdpFtdcVolumeType	BidVolume8;
	///����۾�
	TQdpFtdcPriceType	BidPrice9;
	///��������
	TQdpFtdcVolumeType	BidVolume9;
	///������ʮ
	TQdpFtdcPriceType	AskPrice10;
	///������ʮ
	TQdpFtdcVolumeType	AskVolume10;
	///�����ʮ
	TQdpFtdcPriceType	BidPrice10;
	///������ʮ
	TQdpFtdcVolumeType	BidVolume10;
};

///���ĺ�Լ�������Ϣ
struct CQdpFtdcSpecificInstrumentField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
};

///�����ڴ������ѯ
struct CQdpFtdcShmDepthMarketDataField
{
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///IP��ַ
	TQdpFtdcIPAddressType	IPAddress;
	///���ĺ�
	TQdpFtdcTopicIDType	TopicID;
};

///����������
struct CQdpFtdcMarketDataExchangeIDField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
};

///TOPIC��ѯ
struct CQdpFtdcTopicSearchField
{
	///���ĺ�
	TQdpFtdcTopicIDType	TopicID;
};

///��Լ״̬
struct CQdpFtdcQmdInstrumentStateField
{
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///��Լ����״̬
	TQdpFtdcInstrumentStatusType	InstrumentStatus;
};

///����������������
struct CQdpFtdcMarketDataAsk67Field
{
	///��������
	TQdpFtdcPriceType	AskPrice6;
	///��������
	TQdpFtdcVolumeType	AskVolume6;
	///��������
	TQdpFtdcPriceType	AskPrice7;
	///��������
	TQdpFtdcVolumeType	AskVolume7;
};

///���������˾�����
struct CQdpFtdcMarketDataAsk89Field
{
	///�����۰�
	TQdpFtdcPriceType	AskPrice8;
	///��������
	TQdpFtdcVolumeType	AskVolume8;
	///�����۾�
	TQdpFtdcPriceType	AskPrice9;
	///��������
	TQdpFtdcVolumeType	AskVolume9;
};

///����������������
struct CQdpFtdcMarketDataBid67Field
{
	///�������
	TQdpFtdcPriceType	BidPrice6;
	///��������
	TQdpFtdcVolumeType	BidVolume6;
	///�������
	TQdpFtdcPriceType	BidPrice7;
	///��������
	TQdpFtdcVolumeType	BidVolume7;
};

///��������˾�����
struct CQdpFtdcMarketDataBid89Field
{
	///����۰�
	TQdpFtdcPriceType	BidPrice8;
	///��������
	TQdpFtdcVolumeType	BidVolume8;
	///����۾�
	TQdpFtdcPriceType	BidPrice9;
	///��������
	TQdpFtdcVolumeType	BidVolume9;
};

///������������ʮ����
struct CQdpFtdcMarketDataAskBid10Field
{
	///������ʮ
	TQdpFtdcPriceType	AskPrice10;
	///������ʮ
	TQdpFtdcVolumeType	AskVolume10;
	///�����ʮ
	TQdpFtdcPriceType	BidPrice10;
	///������ʮ
	TQdpFtdcVolumeType	BidVolume10;
};

///�û��ɼ���Ϣ
struct CQdpFtdcUserSystemInfoField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///��Ϣ����
	TQdpFtdcSystemInfoLenType	ClientSystemInfoLen;
	///��Ϣ����
	TQdpFtdcClientSystemInfoType	ClientSystemInfo;
	///�ͻ�ip
	TQdpFtdcIPAddressType	ClientPublicIP;
	///�ͻ��˿�
	TQdpFtdcIPPortType	ClientIPPort;
	///��¼ʱ��
	TQdpFtdcFullLoginTimeType	ClientLoginTime;
	///�ͻ���֤appid
	TQdpFtdcAppIDType	ClientAppID;
};

///������֤ϵͳ��Ϣ
struct CQdpFtdcAuthenticateField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///����汾��Ϣ
	TQdpFtdcProductInfoType	UserProductInfo;
	///�ͻ���֤appid
	TQdpFtdcAppIDType	AppID;
	///������֤id
	TQdpFtdcPasswordType	AuthCode;
};

///������֤ϵͳ��Ϣ
struct CQdpFtdcRtnAuthenticateField
{
	///���͹�˾���
	TQdpFtdcBrokerIDType	BrokerID;
	///�û�����
	TQdpFtdcUserIDType	UserID;
	///����汾��Ϣ
	TQdpFtdcProductInfoType	UserProductInfo;
	///�ͻ���֤appid
	TQdpFtdcAppIDType	AppID;
	///Socket���
	TQdpFtdcNumberType	SockID;
	///�ͻ���������
	TQdpFtdcAppTypeType	AppType;
};

///������֤��Կ
struct CQdpFtdcRSAPubKeyField
{
	///��֤��Կ
	TQdpFtdcSecKeyInfoType	SecKeyInfo;
};

///Full Market Data
struct CQdpFtdcClientDepthMarketDataField
{
	///Client ID
	TQdpFtdcClientIDType	ClientID;
	///����������
	TQdpFtdcExchangeIDType	ExchangeID;
	///������
	TQdpFtdcTradingDayType	TradingDay;
	///���������
	TQdpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TQdpFtdcSettlementIDType	SettlementID;
	///�����
	TQdpFtdcPriceType	PreSettlementPrice;
	///������
	TQdpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TQdpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TQdpFtdcRatioType	PreDelta;
	///����
	TQdpFtdcPriceType	OpenPrice;
	///��߼�
	TQdpFtdcPriceType	HighestPrice;
	///��ͼ�
	TQdpFtdcPriceType	LowestPrice;
	///������
	TQdpFtdcPriceType	ClosePrice;
	///��ͣ���
	TQdpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TQdpFtdcPriceType	LowerLimitPrice;
	///�����
	TQdpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TQdpFtdcRatioType	CurrDelta;
	///���¼�
	TQdpFtdcPriceType	LastPrice;
	///����
	TQdpFtdcVolumeType	Volume;
	///�ɽ����
	TQdpFtdcMoneyType	Turnover;
	///�ֲ���
	TQdpFtdcLargeVolumeType	OpenInterest;
	///�����һ
	TQdpFtdcPriceType	BidPrice1;
	///������һ
	TQdpFtdcVolumeType	BidVolume1;
	///������һ
	TQdpFtdcPriceType	AskPrice1;
	///������һ
	TQdpFtdcVolumeType	AskVolume1;
	///��Լ����
	TQdpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TQdpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TQdpFtdcMillisecType	UpdateMillisec;
};

///��ˮ״̬
struct CQdpFtdcFlowStatusField
{
	///����ϵ�к�
	TQdpFtdcSequenceSeriesType	SequenceSeries;
	///�Ƿ����
	TQdpFtdcBoolType	bReady;
};



#endif
