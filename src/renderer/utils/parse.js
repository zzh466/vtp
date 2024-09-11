const  arr = ['date', 'invertUnit', 'exchange', 'tradingcode', 'product', 'instrument', 'direction', 's/h', 'price', 'lots', 'turnover', 'o/c', 'commission', 'realized', 'premium', 'tranNo', 'accountID']
export function parseComfirm(confirm){
    

    const createIndex = confirm.indexOf('Creation Date')
    const createDate = confirm.substr(createIndex+14,8);
    let data = [];
    const index = confirm.indexOf('成交记录')
    if(index > -1){
        confirm = confirm.slice(index + 4);
        const idx = confirm.indexOf('------\r\n|' + createDate);
        confirm = confirm.slice(idx + 8);
        const lastIndex = confirm.indexOf('|\r\n-------')
        confirm = confirm.slice(0,lastIndex + 1);
        
        console.log(confirm)
        const _data =confirm.split('\r\n')
        data = _data.map((e) =>{
            const item = {};
        
            e = e.slice(1, -1)
            const _arr = e.split('|')
            if(_arr.length < arr.length){
                const exchange = _arr[2]
                let match = exchange.match(/\d+/)
                let code = ''
                if(match){
                    code = match[0]
                    
                    _arr[2] = exchange.replace(code, '');
                    _arr.splice(3,0, code)
                }
            }
            for(let i = 0; i < arr.length; i++){
                item[arr[i]] = _arr[i]
            }
            return item
        })  
    }
    console.log(data)
    return {
        createDate,
        data
    }
}
export const columns = [{
   label: '成交时间',
   prop: 'date',
   width: '70'
},{
    label: '交易所',
    prop: 'exchange',
 },{
  label : '合约',
  prop: 'instrument',
    width: '70'
}, {
    label:"买/卖",
    prop: 'direction'
} ,{
    label:"投保",
    prop: 's/h'
}, {
    label:"成交价",
    prop: 'price',
      width: '100'
}, {
    label:"手数",
    prop: 'lots'
}, {
    label:"成交额",
    prop: 'turnover'
}, {
    label:"开/平",
    prop: 'o/c'
}, {
    label:"手续费",
    prop: 'commission',
     width: '80'
}, {
    label:"平仓盈亏",
    prop: 'realized',
     width: '80'
}, {
    label:"权利金收支",
    prop: 'premium',
     width: '80'
}, {
    label:"成交序号",
    prop: 'tranNo',
   
},{
    label: '账号',
    prop: 'accountID',
    width: '70'
 }]
