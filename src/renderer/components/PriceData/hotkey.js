export default function generate(hotKey){
     hotKey = hotKey.split(';').map(e => {
         return e.split(',')
     });
     console.log(hotKey);
     return function(e, vue){
         console.log(e);
         const chart = vue.chart;
         if(!chart.data.length)return;
         const {keyCode} = e;
         const haskey = hotKey.find(key => key.length && key[1] === keyCode.toString());
         console.log(haskey)
         if(haskey){
             switch(haskey[2]){
                 case '0':
                     const direction = haskey[3];
                     const overprice = haskey[4];
                     let price;
                     if(direction === '0'){
                         price = chart.data[chart.buyIndex].price + parseInt(overprice) * chart.step;

                     }else{
                        price = chart.data[chart.askIndex].price - parseInt(overprice) * chart.step;

                     }
                
                     vue.putOrder(price, direction);
                    return 
                     
             }
         }
     }
}