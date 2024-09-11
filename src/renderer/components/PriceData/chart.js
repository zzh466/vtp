
import { ipcRenderer } from 'electron';
const X = 50.5;
const Y = 20.5;


let  VALUECOLOR ;

// const VALUECOLOR = {
//     buy: '#ffc8e6',
//     ask: '#b4dcc8',
//     buy1: '#ffafc8',
//     ask1: '#a0dcb4',
//     deficit: '#0000ff',
//     profit: '#ff0000',
//     hold: '#00ff00',
//     limit: '#EF2E2E'
// }

const BUYBACKGROUND = '#322810'
// const ASKBACKGROUND = '#103210'
const ASKBACKGROUND = '#103210'
const FONTCOLOR = '#7f7f7f'
class Chart {
    constructor(dom, width, height, step = 0.2, config ={}){
        const {
            barToBorder,
            barVolume,
            barWidth = 10,
            calcBarType,
            volumeScaleCount,
            volumeScaleHeight = 30,
            volumeScaleTick,
            volumeScaleType
        } = config
        this.ctx = dom.getContext('2d');
        
        this.barToBorder = barToBorder;
        this.barWidth =barWidth;
        this.volumeScaleHeight = volumeScaleHeight;
        this.volumeScaleType = volumeScaleType;
        this.volumeScaleCount =volumeScaleCount;
        this.volumeScaleTick = volumeScaleTick;
        this.width = width;
        this.height = height;
        this.step = parseFloat(step);
        this.data = [];
        this.start = 0;
        
        this.range = this.initRange();

        this.setColor();
        const decimal = (this.step.toString().split('.')[1] || []).length;
        this.decimal = decimal;
        this.placeOrder=[];  
        this.traded ={};
        this.init();
    }

    static getHeight(range, value, volumeScaleHeight){
        let start = 0;
        let before = 0;
        if(value >  range[range.length - 1]){
            return volumeScaleHeight * range.length + 30
        }
        for(let i= 0; i < range.length; i++ ){
            if(value <= range[i]){
                start = start + ((value-before) / (range[i]-before)) * volumeScaleHeight; 
                break
            }else {
                start = start + volumeScaleHeight;
                before = range[i];
            }
        }
        if(start){
            start = Math.floor(start) + 0.5
        }
        
        return start ;
    }
     init(){
        const ctx= this.ctx;
        
        this.count = Math.floor((this.width - 150) / (this.barWidth * 2) ) * 2;
        
        
        let range = this.range;
        ctx.beginPath();
        ctx.moveTo(X + 50, Y+10);
        ctx.strokeStyle = '#404040'
        ctx.lineTo(this.width - 19.5,Y+10);
        ctx.stroke();
        
        this.renderRange(range);
       
    }
    setColor(type){
        if(type){
            VALUECOLOR = {
                buy: '#ef302d',
                ask: '#0f65a1',
                buy1: '#b31529',
                ask1: '#10559a',
                deficit: '#0000ff',
                profit: '#ff0000',
                hold: '#00ff00',
                limit: '#EF2E2E',
                low: '#00ff00',
                high: "#ffff00"
            }
        }else{
              
        
             VALUECOLOR = {
                buy: '#ffc8e6',
                ask: '#b4dcc8',
                buy1: '#ffafc8',
                ask1: '#a0dcb4',
                deficit: '#0000ff',
                profit: '#ff0000',
                hold: '#00ff00',
                limit: '#EF2E2E',
                low: '#00ff00',
                high: "#ffff00"
            }
        }
       
        
    }
    initRange(){

        
        let {volumeScaleHeight, volumeScaleType, volumeScaleCount, volumeScaleTick, height} = this;
        let baseRange = null;
        
        switch(volumeScaleType){
            case 0:
                volumeScaleCount = Math.floor((height - 100)/volumeScaleHeight)
                break;
            case 2:
                volumeScaleCount = Math.floor((height - 100)/volumeScaleHeight)
                baseRange = [10, 20, 30, 50, 100, 200, 400, 500, 1000, 2000, 3000, 4000, 5000].slice(0, volumeScaleCount);
                break;
            case 3:
                baseRange = [10, 20, 30, 50, 100, 200, 400, 500, 1000, 2000, 3000, 4000, 5000].slice(0, volumeScaleCount);
        }
        if(!baseRange){
            baseRange = [];
            for(let i = 1; i <= volumeScaleCount; i++){
                baseRange.push(i*volumeScaleTick)
            }
        }
        return baseRange
    }
     renderRange(range){
        
        const ctx= this.ctx;
        ctx.textAlign = 'right'
        ctx.font= '12px 宋体';
        ctx.fillStyle= FONTCOLOR;
       
       
        let {volumeScaleHeight, width} = this;
        let start = 30
        const _Y = Y + volumeScaleHeight;
        ctx.save();
      
     
        range.forEach(e => {
            ctx.fillText(e.toString(), X - 10, _Y + start + 5);
            ctx.beginPath();
            ctx.strokeStyle = '#404040'
            ctx.moveTo(X, _Y + start);
            ctx.lineTo(X+ 50 , _Y + start);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(width - 30, _Y + start);
            ctx.lineTo(width - 5 , _Y + start);
            ctx.stroke();
            start = start + volumeScaleHeight;
        });
        ctx.restore()
    }
    resize( width, height){
        
        const ctx= this.ctx;
        this.width = width;
        this.height = height;
        ctx.width = width;
        ctx.height = height;
        this.range = this.initRange()
        this.init();
        
        this.initData(this.currentPrice);
        this.renderPrice()
    }
     initData(price){
         if(!price || price > Number.MAX_SAFE_INTEGER) return;
         
        const count = this.count / 2;
        const decimal = this.decimal;
        const data = []
        
        data.push({
            price: price.toFixed(decimal)
        })
        for(let i = 1; i <= count; i++ ){

            data.push({
                price: (price + this.step * i).toFixed(decimal)
            })
            data.unshift({
                price: (price - this.step * i).toFixed(decimal)
            })
        }
        if(this.data.length){
            this.getindex(data[data.length - 1].price);
            this.getindex(data[0].price)
            this.start = this.getindex(price, true) - count;
        }else {
            this.data = data;
        }
        
    }
    renderBakcground(){
        const ctx = this.ctx;
        const y = Y + 30;
        const _x = X + 50.5;
        const start = this.start;
        ctx.clearRect(_x-2 , y - 5 ,this.width - 30 - _x, this.height);
        const barWidth = this.barWidth;
       
        let buyIndex = this.buyIndex - start;
       
        if(buyIndex < 0) {
            buyIndex = 0;
        } 
        if( this.buyIndex === this.lowerLimitindex){
            buyIndex = buyIndex -1
        }
        let askIndex = this.askIndex - start;
        if(askIndex < 0) {
            askIndex = 0;
        } 
        
        if( this.askIndex === this.UpperLimitindex){
            askIndex = askIndex +1
        }
        
        if(askIndex  > this.count || askIndex === 0) {
            askIndex = this.count ;
        } 
        if(buyIndex > this.count || askIndex < 0){
            ipcRenderer.send('error-log', JSON.stringify(this.args));
            ipcRenderer.send('error-log', JSON.stringify(this.data));
        }
        ctx.fillStyle = BUYBACKGROUND;
        ctx.fillRect(_x, y, buyIndex *barWidth + barWidth,this.height);
        ctx.fillStyle = ASKBACKGROUND;
        ctx.fillRect(_x + askIndex *barWidth, y, this.width- _x - askIndex *barWidth - 30 , this.height);
        for(let i = start; (i-start) <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * barWidth;
            const y = Y + 10;
            if((price * Math.pow(10, this.decimal)).toFixed()% (this.step * Math.pow(10, this.decimal + 1)) ===0){
                ctx.save();
                ctx.beginPath();
                ctx.moveTo(x, y + 20);                
                ctx.setLineDash([1, 2])
                ctx.lineTo(x, this.height);
                ctx.stroke();
                ctx.restore()
            }
            
        }

    }
    renderPrice(){
        
        const start = this.start;
        const ctx = this.ctx;
        ctx.fillStyle= FONTCOLOR;
        ctx.textAlign = 'left'
        ctx.clearRect(100 , 0 ,this.width ,Y);
        ctx.clearRect(100 , Y+10 ,this.width ,Y+6);
        const barWidth = this.barWidth;
        for(let i = start; (i-start) <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * barWidth;
            const y = Y + 10;
            if((price * Math.pow(10, this.decimal)).toFixed() % (this.step * Math.pow(10, this.decimal + 1)) ===0){
                ctx.save();
                ctx.fillText(price, x , 20);
                ctx.beginPath();
                ctx.moveTo(x, y );
                ctx.lineTo(x, y + 6);
                ctx.stroke();
                ctx.restore()
            }else{
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x, y + 3);
                ctx.stroke();
            }
             
        }
    }
    renderVolume(){
        const ctx = this.ctx;

        const y = Y + 30;
        const _x = X + 50.5;
        const buyIndex = this.buyIndex;
        const askIndex = this.askIndex;
        const barWidth = this.barWidth;
        
        for(let i = this.start; (i-this.start)  <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { volum, type, isone} = this.data[i];
            if(i> buyIndex && i<askIndex){
                continue
            }
            
            if(volum){
                if((type === 'buy' && i > buyIndex) || (type ==='ask' && i < askIndex)){
                    continue;
                }
                if(i=== buyIndex && type === 'buy'){
                    ctx.fillStyle= VALUECOLOR['buy1']; 
                }else if(i===askIndex && type === 'ask'){
                    ctx.fillStyle= VALUECOLOR['ask1']; 
                }else{
                    ctx.fillStyle= VALUECOLOR[type]; 
                }
               
                const  x = _x + (i-this.start) * barWidth;
                const height = Chart.getHeight(this.range, volum, this.volumeScaleHeight); 
                ctx.fillRect(x,y,barWidth -1,height);
                if(i === buyIndex || i === this.askIndex){
                    ctx.font= '12px 宋体';
                    ctx.fillStyle= FONTCOLOR;
                    ctx.fillText(volum, x , y + 10);
                }
            }
            
           

        }
    }
    clearData(startPrice, endPrice){
      
        let start = this.getindex(startPrice, true);
        if(start < 0) start = 0
        let end = this.getindex(endPrice, true);
        if(end > this.data.length -1) end = this.data.length -1;
        for(let i = start; i < end; i++){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            this.data[i].volum = 0;
        }
    }
    pushData(count){
        let {price} = this.data[this.data.length-1];
        price = parseFloat(price)
        const decimal = this.decimal
        if(count < 5){
            count = 5
        }
        for(let i =1; i <= count; i++ ){
            this.data.push({
                price: (price+ i* this.step).toFixed(decimal)
            })
        }
        this.start = this.start + count;
        
    }
    unshiftData(count){
        let {price} = this.data[0];
        price = parseFloat(price)
        const decimal = this.decimal
        if(count < 5){
            count = 5
        }
        for(let i =1; i <= count; i++ ){
            this.data.unshift({
                price:(price- i* this.step).toFixed(decimal)
            })
        }
        this.start = 0;
        return count
    }
    getindex(price, pure){
        // console.log(price, pure);
        if(Math.abs(price) > Number.MAX_SAFE_INTEGER){
           return undefined
        }
        if(!price) return this.start
        let index = Math.round((price - this.data[0].price) / this.step);
        if(pure){
            return index;
        }
        const barToBorder = parseInt(this.barToBorder);
        let offset = 0;
        let rerender = false;
        if(index > this.data.length - barToBorder){
             offset = index - this.data.length + barToBorder;
            this.pushData(offset);
          
            rerender = true;
        }
        if(index < barToBorder){
            offset = barToBorder - index;
            const count = this.unshiftData(offset)
           
            rerender = true;
            index = index + count;
        }
        const start = this.start;
        if(index < start + barToBorder){
            offset =  barToBorder+start - index;
            let min = offset;
           
            this.start = start - min
            rerender = true;
        }
        if(index > start + this.count - barToBorder){
            offset = index - start- this.count+barToBorder;
            let min = offset;
            
            this.start = start + min;
            rerender = true;
        }
        if(rerender){
            this.renderPrice();
        }
        if(Math.abs(offset) >= 50){
            ipcRenderer.send('error-log', price);
            ipcRenderer.send('error-log', JSON.stringify(this.args));
        }
        return index;
    }
    renderTime(time){
        const ctx = this.ctx;
        ctx.save();
        ctx.fillStyle = FONTCOLOR;
        ctx.clearRect(0,0, 50, 20);
        ctx.fillText(time, 0, 20);
        ctx.restore();
    }
    renderCurrentPirce(price, volume){
        const ctx =this.ctx;
        ctx.save();
        ctx.setLineDash([]);
        ctx.strokeStyle = FONTCOLOR;

        // console.log(volume, this.volume)
        if(this.volume < volume){
            ctx.strokeStyle = '#ffff00';
        };
        const barWidth = this.barWidth;
        const x = X + 50 + (this.getindex(price, true) - this.start) * barWidth;
        if(x < x+50){
            ipcRenderer.send('error-log', JSON.stringify(this.args));
            ipcRenderer.send('error-log', JSON.stringify(this.data));
        }
        ctx.clearRect(0,20,this.width,10)
        ctx.beginPath();
        ctx.moveTo(x, 21);
        ctx.lineTo(x+barWidth, 21);
        ctx.lineTo(x+barWidth, 29);
        ctx.lineTo(x, 29);
        ctx.lineTo(x, 21);
        ctx.stroke()
        ctx.restore();
        this.currentPrice = price;
        this.volume=volume
    }
    renderPlaceOrder(){
        if(this.data.length===0) return;
        
        const pricearray = this.placeOrder.reduce((a, b) => {
            const {LimitPrice, VolumeTotalOriginal, Direction, VolumeTraded, OrderStatus, OrderSysID} =b;
            
            if(OrderStatus !== '3' && OrderStatus!=='1' && OrderStatus!=='b'){
                return a;
            }
            const item = a.find(({price})=> price===LimitPrice)
            if(item){
                item.volume = item.volume + VolumeTotalOriginal -VolumeTraded
            }else{
                a.push({
                    price: LimitPrice,
                    volume: VolumeTotalOriginal -VolumeTraded,
                    direction: Direction,
                    OrderSysID
                })
            }
            return a;
        },[])
        const ctx =this.ctx;
        ctx.save();
        
        const y = Y + 30 ;
        const _x = X + 50.5;
        const {barWidth, volumeScaleHeight, range} = this;
        let _volume = [0, 0];
        pricearray.forEach(({price, volume, direction, OrderSysID = ''}) => {
            const index = this.getindex(price, true);
            console.log(this.start, this.start + this.count)
            if(index < this.start || index > this.start + this.count)return;
            const  x = _x + (index-this.start) * barWidth;
            const height = Chart.getHeight(range, volume, volumeScaleHeight); 
           
            ctx.fillStyle = OrderSysID.startsWith('TJBD_')?'blue':'red';
            _volume[direction] = _volume[direction] + volume;
            ctx.fillRect(x,y,barWidth -1,height);

        })
        this.holdVolume = _volume;
        // console.log(this.placeOrder, pricearray)
        ctx.restore()
    }
    renderTradeOrder(){
        const _x = X + 50;
        const _y = Y + 17;
        const {price, direction} = this.traded;
        const {ctx , width, start, barWidth} = this;
        if(!this.data.length)return;
        ctx.clearRect(0, _y - 1 , width, 10)
        if(direction && price.length){
            ctx.save();
            const average = price.reduce((a,b)=> a + parseFloat(b), 0) / price.length;
            const index = this.getindex(average, true)- start;
            let cindex;
            if(direction === '0'){
                cindex = this.buyIndex;
                
            }else{
                cindex = this.askIndex;
            }

            cindex = cindex - start;
            let begin, end, _direction;
            if(index <= cindex){
                begin = index;
                end = cindex;
                _direction = '0';
            }else{
                begin = cindex;
                end = index;
                _direction = '1';
            };
            let color
            if(_direction === direction){
                color = VALUECOLOR.profit;
            }else{
                color = VALUECOLOR.deficit;
            }
            if(begin === end){
                color = VALUECOLOR.hold;
            }
            ctx.fillStyle = color;
            ctx.fillRect(_x + begin * barWidth, _y , (end - begin + 1) * barWidth, 7);
            ctx.fillStyle = '#fff';
            ctx.fillText(price.length, _x+cindex * barWidth + 5, _y + 8)
            ctx.restore()
        }
    }
    renderHighandLow(){
        if(this.data.length === 0) return;
        const lowindex = this.getindex(this.LowestPrice, true);
        const highindex = this.getindex(this.HighestPrice, true);
        const {start, ctx, count, barWidth, height} = this;
        let lowX = (lowindex - start) * barWidth;
        let HighX = (highindex - start) * barWidth
        if(lowindex < start){
            lowX =  - 50;
        }
        if(highindex > start + count){
            HighX = this.width - X - 51
        }
        ctx.clearRect(X -1, Y + 29 ,2 , height - 10);
        ctx.clearRect( this.width - 2, Y + 29 ,2 , height - 10);
        const offset= X + 50;
        ctx.save()
        ctx.setLineDash([]);
        ctx.beginPath();
        ctx.strokeStyle = VALUECOLOR.low
        ctx.moveTo(lowX+offset, Y + 30);
        ctx.lineTo(lowX+offset,height - 10);
        ctx.stroke();
        ctx.beginPath();
        
        ctx.strokeStyle = VALUECOLOR.high
        ctx.moveTo(HighX+offset, Y + 30);
        ctx.lineTo(HighX+offset,height - 10);
        ctx.stroke();
        ctx.restore();
        this.renderLimited()
    }
    renderLimited(){
    
        const lowindex = this.lowerLimitindex
        const highindex =  this.UpperLimitindex;
   
        const offset= X + 49.5;
        const {start, ctx, count, barWidth, height} = this;
        ctx.save()
        ctx.strokeStyle = VALUECOLOR.limit;
        ctx.lineWidth = 2
        function render(index){
            if(start<=index  && index <=start+count){
                const _X = (index - start) * barWidth;
                ctx.beginPath();
                ctx.moveTo(_X+offset, Y + 30);
                ctx.lineTo(_X+offset,height - 10);
                ctx.stroke();
            }
        };
        render(lowindex);
        render(highindex);
        ctx.restore();
    }
    render(arg){
       
        if(!arg.LastPrice){
            arg.LastPrice = arg.AskPrice1 || arg.BidPrice1
        }
        if(arg.LastPrice > Number.MAX_SAFE_INTEGER) return
        if(this.data.length === 0) {
            console.log(arg.LastPrice)
            this.initData(arg.LastPrice);
            if(!this.data.length) return
            this.renderPrice();
        }
       
        
        this.args= arg
        this.renderTime(arg.UpdateTime)
        if(arg.BidPrice5  &&  arg.BidPrice5 <= Number.MAX_SAFE_INTEGER){
            this.clearData(arg.BidPrice5 , arg.BidPrice1 );
        }
        if(arg.AskPrice5  &&  arg.AskPrice5 <= Number.MAX_SAFE_INTEGER){
            this.clearData(arg.AskPrice1 , arg.AskPrice5 );
        }
        
        for(let i=1; i<= 5; i++){
            let buyPirce = arg[`BidPrice${i}`];
            let buyIndex ;
            const flag = i > 1;
            if(buyPirce){
                buyIndex = this.getindex(buyPirce, flag)
                const buyData = this.data[buyIndex];
                if(buyData){
                    buyData.volum = arg[`BidVolume${i}`];
                    buyData.type = 'buy';
                }                                        
            }
          
            
            const askPirce = arg[`AskPrice${i}`] ;
            let askIndex;
            if(askPirce){
                askIndex = this.getindex(askPirce, flag)
                const askData = this.data[askIndex];
                if(askData){
                    askData.volum = arg[`AskVolume${i}`];
                    askData.type = 'ask';
                }                              
            }
            
            if(i === 1) {
                if(buyIndex === undefined){
                    buyIndex = askIndex 
                }
                if(askIndex === undefined){
                    askIndex = buyIndex 
                }
                this.buyIndex = buyIndex;
                this.askIndex = askIndex;
                
            }
        }
        this.lowerLimitPrice = arg.LowerLimitPrice;
        this.UpperLimitPrice = arg.UpperLimitPrice;
        this.lowerLimitindex = this.getindex(arg.LowerLimitPrice, true);
        this.UpperLimitindex = this.getindex(arg.UpperLimitPrice, true);
        if(!arg.BidPrice1){
          
            this.buyIndex = this.lowerLimitindex
        }
        if(!arg.AskPrice1){
           
            this.askIndex = this.UpperLimitindex
        }
        this.LowestPrice = arg.LowestPrice;
        this.HighestPrice = arg.HighestPrice;
       
        this.renderBakcground();
        this.renderHighandLow()
        this.renderVolume();
       
      
        
        this.renderCurrentPirce(arg.LastPrice, arg.Volume);
        this.renderPlaceOrder();
        this.renderTradeOrder();



       
    }
}

export default Chart;