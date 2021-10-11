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
        ['Volume', 'int64'], //数量
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
        ['Volume', 'int64'], //数量
        [`BidPrice1`, 'double'],
        [`BidPrice2`, 'double'],
        [`BidPrice3`, 'double'],
        [`BidPrice4`, 'double'],
        [`BidPrice5`, 'double'],
        [`BidVolume1`, 'int64'],
        [`BidVolume2`, 'int64'],
        [`BidVolume3`, 'int64'],
        [`BidVolume4`, 'int64'],
        [`BidVolume5`, 'int64'],
        [`AskPrice1`, 'double'],
        [`AskPrice2`, 'double'],
        [`AskPrice3`, 'double'],
        [`AskPrice4`, 'double'],
        [`AskPrice5`, 'double'],
        [`AskVolume1`, 'int64'],
        [`AskVolume2`, 'int64'],
        [`AskVolume3`, 'int64'],
        [`AskVolume4`, 'int64'],
        [`AskVolume5`, 'int64'],
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
        ['UpdateTime', 'string', 12], //最后修改时间
        ['UpdateMillisec', 'int32']
]

}

export  { receiveData }