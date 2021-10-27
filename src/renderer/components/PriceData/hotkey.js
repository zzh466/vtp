import { ipcRenderer } from "electron";
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
         if(haskey){
             switch(haskey[2]){
                 case '0':
                    if(!chart.data.length)return;
                     const direction = haskey[3];
                     const overprice = haskey[4];
                     let price;
                     if(direction === '0'){
                         price = parseFloat(chart.data[chart.buyIndex].price) + parseInt(overprice) * chart.step;

                     }else{
                        price = parseFloat(chart.data[chart.askIndex].price) - parseInt(overprice) * chart.step;

                     }
                
                     vue.putOrder(price, direction);
                    break;
                case '3':
                     const volume = haskey[5];
                     vue.changeConfig('volume', parseInt(volume));
                     break
                case  '6':
                     const type = haskey[7];
                     vue.changeConfig('type', type);
                     break;
                case '2':
                    ipcRenderer.send('cancel-order');
                     break;
                case '8':
                    ipcRenderer.send('cancel-order', vue.$route.qurey.id);
                    break
             }
         }
     }
}