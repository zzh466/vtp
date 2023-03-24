import { ipcRenderer } from "electron";
function fixPirce(price, step){
    const times = Math.pow(10, step);
    return Math.round(price * times) / times;
}
let timer = null;
export default function generate(hotKey){
     hotKey = hotKey.split(';').map(e => {
         return e.split(',')
     });
     console.log(hotKey);
     return function(e, vue){
         console.log(e);
         const chart = vue.chart;
        //  
         const {keyCode} = e;
         const haskey = hotKey.find(key => key.length && key[1] === keyCode.toString());
         console.log(haskey)
         function order(config) {
            
            if(!chart.data.length)return;
            
            const direction = haskey[3];
            const overprice = haskey[4];
            let price;
            if(direction === '0'){
                price = parseFloat(chart.data[chart.buyIndex].price) + parseInt(overprice) * chart.step;

            }else{
               price = parseFloat(chart.data[chart.askIndex].price) - parseInt(overprice) * chart.step;

            }
       
            vue.putOrder(fixPirce(price, vue.chart.decimal), direction, undefined,config);
         }
         if(haskey){
             e.preventDefault();
             e.stopPropagation();
            ipcRenderer.send('info-log', `快捷键：${haskey[0]}`)
             switch(haskey[2]){
                 case '0':
                    order();
                   
                    break;
                case '1':
                     //保证先撤单
                   
                    ipcRenderer.invoke('async-cancel-order', {key: 'InstrumentID' , value: vue.$route.query.id}).then((cancel)=>{
                        
                        if(cancel){
                           vue.tasks.push(order);
                        }else {
                            order()
                        }
                        
                        
                    });
                    break;
                case '3':
                     const volume = haskey[5];
                     vue.changeConfig('volume', parseInt(volume));
                     break
                case '4':
                    vue.changeConfig('closeType', (+!parseInt(vue.config.closeType)).toString());
                    break
                case '5':
                    let _type = parseInt(vue.config.type);
                    _type++;
                    if(_type > 3){
                        _type = 0
                    }
                    vue.changeConfig('type', _type.toString());
                    break;
                case  '6':
                     const type = haskey[7];
                     vue.changeConfig('type', type);
                     break;
                case '2':
                    ipcRenderer.send('cancel-order', {key: 'ExchangeID' , value: vue.exchangeId});
                     break;
                case '7':
                    ipcRenderer.send('cancel-order');
                break;
                case '8':
                    ipcRenderer.send('cancel-order', {key: 'InstrumentID' , value: vue.$route.query.id});
                    break
                case '12':
                    order({OrderPriceType: '1'})
             }
         }
     }
}