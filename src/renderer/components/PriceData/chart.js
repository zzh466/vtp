

const X = 50.5;
const Y = 20.5;

const VALUECOLOR = {
    buy: '#ffc8e6',
    ask: '#b4dcc8'
}

const BUYBACKGROUND = '#322810'
const ASKBACKGROUND = '#103210'
const FONTCOLOR = '#7f7f7f'
class Chart {
    constructor(dom, width, height, step = 0.2){
        this.ctx = dom.getContext('2d');
        
        this.width = width;
        this.height = height;
        this.step = step;
        this.data = [];
        this.start = 0;
        this.count = Math.floor((width - 200) / 20 ) * 2;
        const decimal = (this.step.toString().split(1) || []).length;
        this.decimal = decimal;
        this.range =[10, 20, 30, 50, 100, 200];
        this.init();
    }

    static getHeight(range, value){
        let stepHeight = 30;
        let start = 0;
        let before = 0;
        for(let i= 0; i < range.length; i++ ){
            if(value <= range[i]){
                start = start + ((value-before) / (range[i]-before)) * stepHeight; 
                break
            }else {
                start = start + 30;
                before = range[i];
            }
        }
        return start;
    }
     init(){
        const ctx= this.ctx;
        ctx.beginPath();
        ctx.strokeStyle = '#00ff00'
        ctx.moveTo(X, Y + 30);
        ctx.lineTo(X,270.5);
        ctx.stroke();
       
        let range = this.range;
        ctx.beginPath();
        ctx.moveTo(X + 50, Y+10);
        ctx.strokeStyle = FONTCOLOR
        ctx.lineTo(this.width - 19.5,Y+10);
        ctx.stroke();
        this.renderRange(range);
       
    }
     renderRange(range){
        const ctx= this.ctx;
        ctx.textAlign = 'right'
        ctx.font= '12px sans-serif';
        ctx.fillStyle= FONTCOLOR;
        let start = 35
        const _Y = Y + 30;
        ctx.save();
        ctx.setLineDash([1, 1])
        range.forEach(e => {
            ctx.fillText(e.toString(), X - 10, _Y + start + 5);
            ctx.beginPath();
            ctx.strokeStyle = FONTCOLOR
            ctx.moveTo(X, _Y + start);
            ctx.lineTo(X+ 50 , _Y + start);
            ctx.stroke();
            start = start + 30;
        });
        ctx.restore()
    }

     initData(price){
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
        const buyIndex = this.buyIndex - start;
        const askIndex = this.askIndex - start;
        ctx.fillStyle = BUYBACKGROUND;
        ctx.fillRect(_x, y, buyIndex *10 + 10,this.height);
        ctx.fillStyle = ASKBACKGROUND;
        ctx.fillRect(_x + askIndex *10, y, this.width- _x - askIndex *10 , this.height);
        for(let i = start; (i-start) <= this.count; i ++ ){
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * 10;
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
        this.ctx.clearRect(0 , 0 ,this.width , Y );
        const start = this.start;
        const ctx = this.ctx;
        ctx.fillStyle= FONTCOLOR;
        ctx.textAlign = 'left'
        ctx.clearRect(X , 0 ,this.width ,30);
        for(let i = start; (i-start) <= this.count; i ++ ){
            const { price } = this.data[i];
            const  x = X + 50 + (i - start) * 10;
            const y = Y + 10;
            if(price % (this.step * 10) ===0){
                ctx.fillText(price, x , 20);
                ctx.beginPath();
                ctx.moveTo(x, y );
                ctx.lineTo(x, y + 10);
                ctx.stroke();
                ctx.save();
                ctx.beginPath();
                ctx.moveTo(x, y + 20);                
                ctx.setLineDash([1, 2])
                ctx.lineTo(x, this.height);
                ctx.stroke();
                ctx.restore()
            }else{
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.lineTo(x, y + 5);
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
       
        for(let i = this.start; (i-this.start)  <= this.count; i ++ ){
            const { volum, type} = this.data[i];
            if(i> buyIndex && i<askIndex){
                continue
            }
            if(volum){
                if((type === 'buy' && i > buyIndex) || (type ==='ask' && i < askIndex)){
                    continue;
                }
                ctx.fillStyle= VALUECOLOR[type]; 
                const  x = _x + (i-this.start) * 10;
                const height = Chart.getHeight(this.range, volum); 
                ctx.fillRect(x,y,9,height);
                if(i === buyIndex || i === this.askIndex){
                    ctx.font= '12px sans-serif';
                    ctx.fillStyle= FONTCOLOR;
                    ctx.fillText(volum, x , y + 10);
                }
            }
            
           

        }
    }
    clearData(startPrice, endPrice){
        const start = this.getindex(startPrice);
        const end = this.getindex(endPrice);
        for(let i = start; i <= end; i++){
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
    getindex(price){
        let index = Math.round((price - this.data[0].price) / this.step);
        const start = this.start;
        if(index > this.data.length - 6){
            this.pushData(index - this.data.length + 6);
            this.renderPrice();
        }
        if(index < 5){
            const count = this.unshiftData(5 - index)
            this.ctx.clearRect(0 , 0 ,this.width , Y );
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
        const x = X + 50 + (this.getindex(price) - this.start) * 10;
        ctx.clearRect(X,20,this.width,10)
        ctx.beginPath();
        ctx.moveTo(x, 21);
        ctx.lineTo(x+10, 21);
        ctx.lineTo(x+10, 29);
        ctx.lineTo(x, 29);
        ctx.lineTo(x, 21);
        ctx.stroke()
        ctx.restore();
        this.currentPrice = price;
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
        this.renderBakcground();
        this.renderVolume();
        this.renderCurrentPirce(arg.LastPrice);
       
    }
}

export default Chart;