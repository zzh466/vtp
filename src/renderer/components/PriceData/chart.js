

const X = 50.5;
const Y = 20.5;

const VALUECOLOR = {
    buy: '#ffc8e6',
    ask: '#b4dcc8',
    deficit: '#0000ff',
    profit: '#ff0000',
    hold: '#00ff00'

}

const BUYBACKGROUND = '#322810'
const ASKBACKGROUND = '#103210'
const FONTCOLOR = '#7f7f7f'
class Chart {
    constructor(dom, width, height, step = 0.2, config ={}){
        const {
            barToBorder,
            barVolume,
            barWeight = 10,
            calcBarType,
            volumeScaleCount,
            volumeScaleHeight = 30,
            volumeScaleTick,
            volumeScaleType
        } = config
        this.ctx = dom.getContext('2d');
        this.stepwidth =barWeight;
        this.stepHeight = volumeScaleHeight;
        this.volumeScaleType = volumeScaleType;
        this.volumeScaleCount =volumeScaleCount;
        this.volumeScaleTick = volumeScaleTick;
        this.width = width;
        this.height = height;
        this.step = step;
        this.data = [];
        this.start = 0;
        this.range = this.initRange();
        const decimal = (this.step.toString().split(1) || []).length;
        this.decimal = decimal;
        this.placeOrder=[];  
        this.traded ={};
        this.init();
    }

    static getHeight(range, value, stepHeight){
        let start = 0;
        let before = 0;
        for(let i= 0; i < range.length; i++ ){
            if(value <= range[i]){
                start = start + ((value-before) / (range[i]-before)) * stepHeight; 
                break
            }else {
                start = start + stepHeight;
                before = range[i];
            }
        }
        return start;
    }
     init(){
        const ctx= this.ctx;
      
        this.count = Math.floor((this.width - 150) / (this.stepwidth * 2) ) * 2;
        
        
        let range = this.range;
        ctx.beginPath();
        ctx.moveTo(X + 50, Y+10);
        ctx.strokeStyle = FONTCOLOR
        ctx.lineTo(this.width - 19.5,Y+10);
        ctx.stroke();
        this.renderRange(range);
       
    }
    initRange(){
        let {stepHeight, volumeScaleType, volumeScaleCount, volumeScaleTick, height} = this;
        let baseRange = null;
        
        switch(volumeScaleType){
            case 0:
                volumeScaleCount = Math.floor((height - 100)/stepHeight)
                break;
            case 2:
                volumeScaleCount = Math.floor((height - 100)/stepHeight)
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
        ctx.font= '12px sans-serif';
        ctx.fillStyle= FONTCOLOR;
       
       
        let stepHeight = this.stepHeight;
        let start = 30
        const _Y = Y + stepHeight;
        ctx.save();
        ctx.setLineDash([1, 1])
        range.forEach(e => {
            ctx.fillText(e.toString(), X - 10, _Y + start + 5);
            ctx.beginPath();
            ctx.strokeStyle = FONTCOLOR
            ctx.moveTo(X, _Y + start);
            ctx.lineTo(X+ 50 , _Y + start);
            ctx.stroke();
            start = start + stepHeight;
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
        this.data = [];
        this.initData(this.currentPrice);
        this.renderPrice()
    }
     initData(price){
         if(!price) return;
        const count = this.count / 2;
        const decimal = this.decimal;
        this.data.push({
            price: price.toFixed(decimal)
        })
        for(let i = 1; i <= count; i++ ){
            this.data.push({
                price: (price + this.step * i).toFixed(decimal)
            })
            this.data.unshift({
                price: (price - this.step * i).toFixed(decimal)
            })
        }
        
    }
    renderBakcground(){
        const ctx = this.ctx;
        const y = Y + 30;
        const _x = X + 50.5;
        const start = this.start;
        ctx.clearRect(_x-2 , y - 5 ,this.width , this.height);
        const stepwidth = this.stepwidth;
        const buyIndex = this.buyIndex - start;
        const askIndex = this.askIndex - start;
        ctx.fillStyle = BUYBACKGROUND;
        ctx.fillRect(_x, y, buyIndex *stepwidth + stepwidth,this.height);
        ctx.fillStyle = ASKBACKGROUND;
        ctx.fillRect(_x + askIndex *stepwidth, y, this.width- _x - askIndex *stepwidth - 30 , this.height);
        for(let i = start; (i-start) <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * stepwidth;
            const y = Y + 10;
            if(price % (this.step * 10) ===0){
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
        const stepwidth = this.stepwidth;
        for(let i = start; (i-start) <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * stepwidth;
            const y = Y + 10;
            if(price % (this.step * 10) ===0){
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
        const stepwidth = this.stepwidth;
        for(let i = this.start; (i-this.start)  <= this.count; i ++ ){
            if(!this.data[i]){
                console.log(i, JSON.parse(JSON.stringify(this.data)))
                continue;
            }
            const { volum, type} = this.data[i];
            if(i> buyIndex && i<askIndex){
                continue
            }
            if(volum){
                if((type === 'buy' && i > buyIndex) || (type ==='ask' && i < askIndex)){
                    continue;
                }
                ctx.fillStyle= VALUECOLOR[type]; 
                const  x = _x + (i-this.start) * stepwidth;
                const height = Chart.getHeight(this.range, volum, this.stepHeight); 
                ctx.fillRect(x,y,stepwidth -1,height);
                if(i === buyIndex || i === this.askIndex){
                    ctx.font= '12px sans-serif';
                    ctx.fillStyle= FONTCOLOR;
                    ctx.fillText(volum, x , y + 10);
                }
            }
            
           

        }
    }
    clearData(startPrice, endPrice){
        const start = this.getindex(startPrice, true);
        const end = this.getindex(endPrice, true);
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

        return count
    }
    getindex(price, pure){
        // console.log(price, pure);
        let index = Math.round((price - this.data[0].price) / this.step);
        if(pure){
            return index;
        }
        const start = this.start;
        if(index > this.data.length - 6){
            this.pushData(index - this.data.length + 6);
            this.renderPrice();
        }
        if(index < 5){
            const count = this.unshiftData(5 - index)
           
            this.renderPrice();
            index = index + count;
        }
        if(index < start + 5){
            this.start = start - 5
        }
        if(index > start + this.count - 5){
            this.start = start + 5;
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
    renderCurrentPirce(price){
        const ctx =this.ctx;
        ctx.save();
        ctx.setLineDash([]);
        ctx.strokeStyle = FONTCOLOR;
        if(this.currentPrice !== price){
            ctx.strokeStyle = '#ffff00';
        };
        const stepwidth = this.stepwidth;
        const x = X + 50 + (this.getindex(price, true) - this.start) * stepwidth;
        ctx.clearRect(X,20,this.width,10)
        ctx.beginPath();
        ctx.moveTo(x, 21);
        ctx.lineTo(x+stepwidth, 21);
        ctx.lineTo(x+stepwidth, 29);
        ctx.lineTo(x, 29);
        ctx.lineTo(x, 21);
        ctx.stroke()
        ctx.restore();
        this.currentPrice = price;
    }
    renderPlaceOrder(){
        if(this.data.length===0) return;
        const pricearray = this.placeOrder.reduce((a, b) => {
            const {LimitPrice, volume, Direction} =b;
            const item = a.find(({price})=> price===LimitPrice)
            if(item){
                item.volume = item.volume + volume
            }else{
                a.push({
                    price: LimitPrice,
                    volume: volume,
                    direction: Direction
                })
            }
            return a;
        },[])
        const ctx =this.ctx;
        ctx.save();
        
        const y = Y + 30 ;
        const _x = X + 50.5;
        const {stepwidth, stepHeight, range} = this;
        let _volume = [0, 0];
        pricearray.forEach(({price, volume, direction}) => {
            const index = this.getindex(price, true);
            const  x = _x + (index-this.start) * stepwidth;
            const height = Chart.getHeight(range, volume, stepHeight); 
            ctx.fillStyle = 'red';
            _volume[direction] = _volume[direction] + volume;
            ctx.fillRect(x,y,stepwidth -1,height);

        })
        this.holdVolume = _volume;
        // console.log(this.placeOrder, pricearray)
        ctx.restore()
    }
    renderTradeOrder(){
        const _x = X + 50;
        const _y = Y + 17;
        const {price, direction} = this.traded;
        const {ctx , width, start, stepwidth} = this;
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
            ctx.fillRect(_x + begin * stepwidth, _y , (end - begin + 1) * stepwidth, 7);
            ctx.fillStyle = '#fff';
            ctx.fillText(price.length, _x+cindex * stepwidth + 5, _y + 8)
            ctx.restore()
        }
    }
    renderHighandLow(){
        // console.log(this.LowestPrice, this.HighestPrice)
        const lowindex = this.getindex(this.LowestPrice, true);
        const highindex = this.getindex(this.HighestPrice, true);
        const {start, ctx, count, stepwidth, height} = this;
        let lowX = X + (lowindex - start) * stepwidth;
        let HighX = (highindex - start) * stepwidth
        if(lowindex < 0){
            lowX = X - 50;
        }
        if(highindex > start + count){
            HighX = count * stepwidth + 50
        }
        ctx.clearRect(X , Y + 30 ,1 , height - 10);
        ctx.clearRect( count * stepwidth +100 + X , Y + 30 ,1 , height - 10);
        const offset= X + 50;
        ctx.save()
        ctx.setLineDash([]);
        ctx.beginPath();
        ctx.strokeStyle = '#00ff00'
        ctx.moveTo(lowX+offset, Y + 30);
        ctx.lineTo(lowX+offset,height - 10);
        ctx.stroke();
        ctx.beginPath();
        
        ctx.strokeStyle = '#ffff00'
        ctx.moveTo(HighX+offset, Y + 30);
        ctx.lineTo(HighX+offset,height - 10);
        ctx.stroke();
        ctx.restore();
    }
    render(arg){
     
        if(this.data.length === 0) {
            console.log(arg.LastPrice)
            this.initData(arg.LastPrice);
            this.renderPrice();
        }
        this.renderTime(arg.UpdateTime)
        this.clearData(arg.BidPrice5, arg.BidPrice1 || arg.LastPrice);
        this.clearData(arg.AskPrice1 || arg.LastPrice, arg.AskPrice5 );
        for(let i = 1; i<= 5; i++){
            const buyPirce = arg[`BidPrice${i}`];
            const buyIndex = this.getindex(buyPirce)
            const buyData = this.data[buyIndex];
            buyData.volum = arg[`BidVolume${i}`];
            buyData.type = 'buy';
            const askPirce = arg[`AskPrice${i}`];
            const askIndex = this.getindex(askPirce)
            const askData = this.data[askIndex];
            askData.volum = arg[`AskVolume${i}`];
            askData.type = 'ask';
            if(i === 1) {
                this.buyIndex = buyIndex;
                this.askIndex = askIndex;
            }
        }
        this.LowestPrice = arg.LowestPrice;
        this.HighestPrice = arg.HighestPrice;
        this.renderBakcground();
        this.renderVolume();
        this.renderHighandLow()
        this.renderCurrentPirce(arg.LastPrice);
        this.renderPlaceOrder();
        this.renderTradeOrder();



       
    }
}

export default Chart;