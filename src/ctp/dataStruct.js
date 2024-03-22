const receiveData = {
    SP:[
        ['TradingDay', 'string', 9],//交易日
        ['InstrumentID', 'string', 31],//合约代码
        ['ExchangeID', 'string', 9], //交易所代码
        ['ExchangeInstID', 'string', 31],//合约在交易所的代码
        ['LastPrice', 'double'], //最新价
        ['PreSettlementPrice', 'double'],//上次结算价
        ['PreClosePrice', 'double'], //昨收盘
        ['PreOpenInterest', 'double'], //昨持仓量
        ['OpenPrice', 'double'], //今开盘
        ['HighestPrice', 'double'], //最高价
        ['LowestPrice', 'double'], //最低价
        ['Volume', 'int32'],
        ['', 'int32'], //数量
        ['Turnover', 'double'], //成交金额
        ['OpenInterest', 'double'],//持仓量
        ['ClosePrice', 'double'], //今收盘
        ['SettlementPrice', 'double'], //本次结算价
        ['UpperLimitPrice', 'double'],//涨停板价
        ['LowerLimitPrice', 'double'],//跌停板价
        ['PreDelta', 'double'], //昨虚实度
        ['CurrDelta', 'double'],//今虚实度
        ['UpdateTime', 'string', 12], //最后修改时间
        ['UpdateMillisec', 'int32']
    ].concat(//最后修改毫秒
        [1,2,3,4,5].reduce((a, b) => a.concat([
            [`BidPrice${b}`, 'double'],
            [`BidVolume${b}`, 'int32'],
            [``, 'int32'],
            [`AskPrice${b}`, 'double'],
            [`AskVolume${b}`, 'int32'],
            [``, 'int32'],
        ]), [])).concat([
        ['AveragePrice', 'double'],//当日均价
        ['ActionDay', 'string', 16]
    ]), //业务日期
    GZ: [
        ['InstrumentID', 'string', 31],
        ['LastPrice', 'double'],
        ['Volume', 'int32'], //数量
        [`BidPrice1`, 'double'],
        [`BidPrice2`, 'double'],
        [`BidPrice3`, 'double'],
        [`BidPrice4`, 'double'],
        [`BidPrice5`, 'double'],
        [`BidVolume1`, 'int32'],
        [`BidVolume2`, 'int32'],
        [`BidVolume3`, 'int32'],
        [`BidVolume4`, 'int32'],
        [`BidVolume5`, 'int32'],
        [`AskPrice1`, 'double'],
        [`AskPrice2`, 'double'],
        [`AskPrice3`, 'double'],
        [`AskPrice4`, 'double'],
        [`AskPrice5`, 'double'],
        [`AskVolume1`, 'int32'],
        [`AskVolume2`, 'int32'],
        [`AskVolume3`, 'int32'],
        [`AskVolume4`, 'int32'],
        [`AskVolume5`, 'int32'],
        ['HighestPrice', 'double'],//最高价
        ['LowestPrice', 'double'],//最低价
        ['UpperLimitPrice', 'double'],//涨停板价
        ['LowerLimitPrice', 'double'],//跌停板价
        ['PreSettlementPrice', 'double'],//上次结算价
        ['PreClosePrice', 'double'], //昨收盘
        ['PreOpenInterest', 'double'], //昨持仓量
        ['PreDelta', 'double'], //昨虚实度
        ['CurrDelta', 'double'],//今虚实度
        ['OpenPrice', 'double'], //今开盘
        ['ClosePrice', 'double'], //今收盘
        ['SettlementPrice', 'double'], //本次结算价
        ['Turnover', 'double'], //成交金额
        ['OpenInterest', 'double'],//持仓量
        ['TradingDay', 'string', 9],//交易日
        ['UpdateTime', 'string', 9], //最后修改时间
        ['UpdateMillisec', 'int32']
]

}
const orderData = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['InstrumentID', 'string', 31],
  
    ['OrderRef', 'string', 13],
    ['UserID', 'string', 16],
    ['OrderPriceType', 'string', 2],
    ['Direction', 'string', 2],
    ['CombOffsetFlag', 'string', 5],
    ['CombHedgeFlag', 'string', 5],
    ['LimitPrice', 'double'],
    ['VolumeTotalOriginal', 'int32'],
    ['TimeCondition', 'string', 2],
    ['GTDDate', 'string', 9],
    ['VolumeCondition', 'string', 2],
    ['MinVolume', 'int32'],
    ['ContingentCondition', 'string', 2],
    ['StopPrice', 'double'],
    ['ForceCloseReason', 'string', 2],
    ['IsAutoSuspend', 'int32'],
    ['BusinessUnit', 'string', 21],
    ['RequestID', 'int32'],
    ['OrderLocalID', 'string', 13],
    ['ExchangeID', 'string', 9],
    ['ParticipantID', 'string', 11],
    ['ClientID', 'string', 11],
    ['exchangeInstID', 'string', 31],
    ['TraderID', 'string', 21],
    ['InstallID', 'int32'],
    ['OrderSubmitStatus', 'string', 2],
    ['NotifySequence', 'int32'],
    ['TradingDay', 'string', 9],
    ['SettlementID', 'int32'],
    ['OrderSysID', 'string', 21],
    ['OrderSource', 'string', 2],
    ['OrderStatus', 'string', 2],
    ['OrderType', 'string', 2],
    ['VolumeTraded', 'int32'],
    ['VolumeTotal', 'int32'],
    ['InsertDate', 'string', 9],
    ['InsertTime', 'string', 9],
    ['ActiveTime', 'string', 9],
    ['SuspendTime', 'string', 9],
    ['UpdateTime', 'string', 9],
    ['CancelTime', 'string', 9],
    ['ActiveTraderID', 'string', 21],
    ['ClearingPartID', 'string', 11],
    ['SequenceNo', 'int32'],
    ['FrontID', 'int32'],
    ['SessionID', 'int32'],
    ['UserProductInfo', 'string', 11],
    ['StatusMsg', 'string',162],
    ['UserForceClose', 'int32'],
    ['ActiveUserID', 'string', 16],
    ['BrokerOrderSeq', 'int32'], 
    ['RelativeOrderSysID', 'string', 21],
    ['ZCETotalTradedVolume', 'int32'],
    ['IsSwapOrder', 'int32'],
    ['BranchID', 'string', 9],
    ['InvestUnitID', 'string', 17],
    ['AccountID', 'string', 13],
    ['CurrencyID', 'string', 4],
    ['IPAddress', 'string', 16],
    ['MacAddress', 'string', 21]
 
]
const tradeData = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['InstrumentID', 'string', 31],
    ['OrderRef', 'string', 13],
    ['UserID', 'string', 16],
    ['ExchangeID', 'string', 9],
    ['TradeID', 'string', 21],
    ['Direction', 'string', 2],
    ['OrderSysID', 'string', 21],
    ['ParticipantID', 'string', 11],
    ['ClientID', 'string', 11],
    ['tradingRole', 'string', 2],
    ['ExchangeInstID', 'string', 31],
    ['OffsetFlag', 'string', 2],
    ['HedgeFlag', 'string', 2],
    ['Price', 'double'],
    ['Volume', 'int32'],
    ['TradeDate', 'string', 9],
    ['TradeTime', 'string', 9],
    ['TradeType', 'string', 2],
    ['PriceSource', 'string', 2],
    ['TraderID', 'string', 21],
    ['OrderLocalID', 'string', 13],
    ['ClearingPartID', 'string', 11],
    ['BusinessUnit', 'string', 21],
    ['SequenceNo', 'int32'],
    ['TradingDay', 'string', 9],
    ['SettlementID', 'int32'],
    ['BrokerOrderSeq', 'int32'],
    ['TradeSource', 'string', 2],
    ['TnvestUnitID', 'string', 17]
]
const inderOrderData = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['InstrumentID', 'string', 31],
    ['OrderRef', 'string', 13],
    ['UserID', 'string', 16],
    ['OrderPriceType', 'string', 2],
    ['Direction', 'string', 2],
    ['CombOffsetFlag', 'string', 5],
    ['CombHedgeFlag', 'string', 5],
    ['LimitPrice', 'double'],
    ['VolumeTotalOriginal', 'int32'],
    ['TimeCondition', 'string', 2],
    ['GTDDate', 'string', 9],
    ['VolumeCondition', 'string', 2],
    ['MinVolume', 'int32'],
    ['ContingentCondition', 'string', 2],
    ['StopPrice', 'double'],
    ['ForceCloseReason', 'string', 2],
    ['IsAutoSuspend', 'int32'],
    ['BusinessUnit', 'string', 21],
    ['RequestID', 'int32'],
    ['UserForceClose', 'int32'],
    ['IsSwapOrder', 'int32'],
    ['ExchangeID', 'string', 9],
    ['InvestUnitID', 'string', 17],
    ['AccountID', 'string', 13],
    ['CurrencyID', 'string', 4],
    ['ClientID', 'string', 11],
    ['IPAddress', 'string', 16],
    ['MacAddress', 'string', 21]
]
const tradingAccountData = [
    ['BrokerID', 'string', 11],
    ['AccountID', 'string', 11],
    ['PreMortgage', 'double'],
    ['PreCredit', 'double'],
    ['PreDeposit', 'double'],
    ['PreBalance', 'double'],
    ['PreMargin', 'double'],
    ['InterestBase', 'double'],
    ['Interest', 'double'],
    ['Deposit', 'double'],
    ['Withdraw', 'double'],
    ['FrozenMargin', 'double'],
    ['FrozenCash', 'double'],
    ['FrozenCommission', 'double'],
    ['CurrMargin', 'double'],
    ['CashIn', 'double'],
    ['Commission', 'double'],
    ['CloseProfit', 'double'],
    ['PositionProfit', 'double'],
    ['Balance', 'double'],
    ['Available', 'double'],
    ['WithdrawQuota', 'double'],
    ['reserve', 'double'],
    ['TradingDay', 'string', 9],
    ['SettlementID', 'int32'],
    ['Credit', 'double'],
    ['Mortgage', 'double'],
    ['ExchangeMargin', 'double'],
    ['DeliveryMargin', 'double'],
    ['ExchangeDeliveryMargin', 'double'],
    ['ReserveBalance', 'double'],
    ['CurrencyID', 'string', 4],
    ['PreFundMortgageIn', 'double'],
    ['PreFundMortgageOut', 'double'],
    ['FundMortgageIn', 'double'],
    ['FundMortgageOut', 'double'],
    ['FundMortgageAvailable', 'double'],
    ['MortgageableFund', 'double'],
    ['SpecProductMargin', 'double'],
    ['SpecProductFrozenMargin', 'double'],
    ['SpecProductCommission', 'double'],
    ['SpecProductFrozenCommission', 'double'],
    ['SpecProductPositionProfit', 'double'],
    ['SpecProductCloseProfit', 'double'],
    ['SpecProductPositionProfitByAlg', 'double'],
    ['SpecProductExchangeMargin', 'double'],
    ['BizType', 'string', 2],
    ['FrozenSwap', 'double'],
    ['RemainSwap', 'double'],
]
const cancelOrder = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['orderActionRef', 'int32'],
    ['OrderRef', 'string', 13],
    ['RequestID', 'int32'],
    ['FrontID', 'int32'],
    ['SessionID', 'int32'],
    ['ExchangeID', 'string', 9],
    ['OrderSysID', 'string', 21],
    ['ActionFlag','string', 2],
    ['LimitPrice', 'double'],
    ['volumeChange', 'int32'],
    ['UserID', 'string', 16],
    ['InstrumentID', 'string', 31],
    ['InvestUnitID', 'string', 17],
    ['IPAddress', 'string', 16],
    ['MacAddress', 'string', 21]
]
const acctounRsqData = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['CurrencyID', 'string', 4],
    ['BizType', 'string', 2],
    ['AccountID', 'string', 13],
]
const respInfoVOData = [
    ['info', 'object', [
        ['ErrorID', 'int32'],
        ['ErrorMsg', 'string', 162]
    ]],
    ['requestId', 'int32'],
    ['isLast', 'bool']
]
const commissionRateData =  [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['InstrumentID', 'string', 31],
    ['ExchangeID', 'string', 9],
    ['InvestUnitID', 'string', 17],
]
const commissionRateRsqData =  [
   
    ['InstrumentID', 'string', 31],
    ['InvestorRange', 'string', 2],
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['OpenRatioByMoney', 'double'],
    ['OpenRatioByVolume', 'double'],
    ['CloseRatioByMoney', 'double'],
    ['CloseRatioByVolume', 'double'],
    ['CloseTodayRatioByMoney', 'double'],
    ['CloseTodayRatioByVolume', 'double'],
    ['ExchangeID', 'string', 9],
    ['BizType', 'string', 2],
    ['InvestUnitID', 'string', 17]
]
const qrySettlementConfirmInfo =[
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
 
    ['AccountID', 'string', 13],
    ['CurrencyID', 'string', 4],
]

const settlementInfo =[
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['TradingDay', 'string', 9],
    ['AccountID', 'string', 13],
    ['CurrencyID', 'string', 4],
]
const rspSettlementInfo = [
    ['TradingDay', 'string', 9],
    ['SettlementID', 'int32'],
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['sequenceNo', 'int32'],
    ['Content', 'string', 501*2],
    ['AccountID', 'string', 13],
    ['CurrencyID', 'string', 4],
]
const settlementInfoConfirm = [
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['ConfirmDate', 'string', 9],
    ['ConfirmTime', 'string', 9],
    ['SettlementID', 'int32'],
    ['AccountID', 'string', 26],
    ['CurrencyID', 'string', 4],
]
const qryInvestorPositionDetai = [
    ['InstrumentID', 'string', 31],
    ['BrokerID', 'string', 11],
    ['InvestorID', 'string', 13],
    ['HedgeFlag', 'string', 2],
    ['Direction', 'string', 2],
    ['OpenDate', 'string', 9],
    ['TradeID', 'string', 21],
    ['Volume', 'int32'],
    ['OpenPrice', 'double'],
    ['TradingDay', 'string', 9],
    ['SettlementID', 'int32'],
    ['TradeType', 'string', 2],
    ['CombInstrumentID', 'string', 31],
    ['ExchangeID', 'string', 9],
    ['CloseProfitByDate', 'double'],
    ['CloseProfitByTrade', 'double'],
    ['PositionProfitByDate', 'double'],
    ['PositionProfitByTrade', 'double'],
 
    ['Margin', 'double'],
    ['ExchMargin', 'double'],
    ['MarginRateByMoney', 'double'],
    ['MarginRateByVolume', 'double'],
    ['LastSettlementPrice', 'double'],
    ['SettlementPrice', 'double'],
    ['CloseVolume', 'int32'],
    ['CloseAmount', 'double'],
    ['TimeFirstVolume', 'int32'],
    ['InvestUnitID', 'string', 17]

]
export  { receiveData, orderData, tradeData, inderOrderData, tradingAccountData, cancelOrder, acctounRsqData, respInfoVOData, commissionRateData , commissionRateRsqData , settlementInfo, rspSettlementInfo, settlementInfoConfirm, qrySettlementConfirmInfo, qryInvestorPositionDetai}