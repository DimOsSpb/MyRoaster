#include <Chart.h>

Chart::Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby){
    _nextion = nextion;
    if(rbx>ltx && rby>lty){
        _leftTop.x = ltx;
        _leftTop.y = lty;
        _rightBottom.x = rbx;
        _rightBottom.y = rby;
        _deltaXY = float(rbx-ltx)/(rby-lty);
    }
};

void Chart::initChanel(uint8_t ch_idx,int32_t MinValue,int32_t MaxValue,uint16_t color,uint8_t depth){
    if(ch_idx >= 0 && ch_idx < CHART_CHANELS){
        _channels[ch_idx].minValue = MinValue;
        _channels[ch_idx].maxValue = MaxValue;
        _channels[ch_idx].color = color;
        _channels[ch_idx].depth = depth;
        _channels[ch_idx].lastValues[0] = 0;
        _channels[ch_idx].lastValues[1] = 0;
        _channels[ch_idx].lastValues[2] = 0;
        if(MaxValue>0)_channels[ch_idx].pointsInDivision = float(_rightBottom.y-_leftTop.y)/(MaxValue-MinValue);
    }
};

void Chart::keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue){
    for (uint8_t i = 0; i < CHARRT_STAT_VALUES-1; i++)
        _channels[ch_idx].lastValues[i] = _channels[ch_idx].lastValues[i+1];
    _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1] = _filteredValue;
    _channels[ch_idx].lastValue = _value;
};

const int32_t Chart::getY(uint8_t ch_idx,int32_t _value){
    int32_t val_y;
    if(_value<=_channels[ch_idx].minValue) return _rightBottom.y;
    if(_value>_channels[ch_idx].maxValue) return _leftTop.y;
    else 
    {
        
        val_y = (_value - _channels[ch_idx].minValue)*_channels[ch_idx].pointsInDivision;
        if(val_y > _rightBottom.y) return _leftTop.y; 
        else
            val_y = _rightBottom.y - val_y;
    }
    return val_y;
};

#define AVERAGE_COUNT 2 //_values[AVERAGE_COUNT]!!
const int32_t Chart::average(int32_t *_values){
    int32_t _val = *_values;
    for (size_t i = 0; i < AVERAGE_COUNT; i++)
        _val = _val + (*(_values+1) - _val) * 0.5;    
    return _val;
     
};

bool Chart::fieldIsOver(){
    for (uint8_t i = 0; i < CHART_CHANELS; i++)
        if(_channels[i].curX>_rightBottom.x)
            return true;
    return false;
    

};

void Chart::clearRight(){
    _nextion->cropPic(_chartCurX, _leftTop.y, _rightBottom.x, _rightBottom.y);
};

void Chart::addChanelValue(uint8_t ch_idx,int32_t _value){
    uint16_t val_x1,val_y1,val_y2;
    int32_t last_val,filtered_val;
    Serial.print("BT=220 - ");Serial.println(getY(0, 220));
    Serial.print("BT=200 - ");Serial.println(getY(0, 200));
    Serial.print("BT=196 - ");Serial.println(getY(0, 196));
    Serial.println("---");

    if(ch_idx >= 0  && ch_idx < CHART_CHANELS && _channels[ch_idx].depth>0){
        if(_channels[ch_idx].curX) {
            last_val = _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1];
            if(_channels[ch_idx].lastValue != _value ) 
                filtered_val = last_val + (_value - last_val) * CHART_FILTER_RATIO; // CHART_VALUE_FILTER_RATIO - коэффициент фильтрации 0.. 1. Обычно около 0.01-0.1 (то бишь float)
            else filtered_val = _value;
        }
        else{
            last_val = _value;
            filtered_val = _value;
        }


        val_y2 = getY(ch_idx, filtered_val);
        if(_channels[ch_idx].curX){
            val_x1 = _channels[ch_idx].curX - 1;
            val_y1 = getY(ch_idx, last_val);
        }
        else { 
            val_x1 = _channels[ch_idx].curX;
            val_y1 = val_y2;
        }

        for (uint8_t i = 0; i < _channels[ch_idx].depth; i++)
        {
            uint8_t y1 = val_y1 > _leftTop.y ? val_y1-i : val_y1,
                    y2 = val_y2 > _leftTop.y ? val_y2-i : val_y2;
            _nextion->line( val_x1, y1, _channels[ch_idx].curX, y2, _channels[ch_idx].color );
                
        }


       if(_channels[ch_idx].curX++ > _chartCurX) _chartCurX = _channels[ch_idx].curX;

        keepChartLastValue(ch_idx, _value, filtered_val);
    }
};

#define DELTA_P 2 //difference curX between average lastValues[0,1,2] and average lastValues[3,4,5]
void Chart::chanelForecast(uint8_t ch_idx){
    int32_t firstValue,secondValue,overVal=0;
    float forecastVal;
    float ratio;            //Value Ghange ratio
    float leftPosRatio;     //Left Pos / DELTA_P Ratio
        

    uint16_t x1,x2,y1,y2;

    if(_channels[ch_idx].depth > 0 && _channels[ch_idx].curX > CHARRT_STAT_VALUES-1 && _channels[ch_idx].curX<_rightBottom.x-1) {     //if statistics accumulated


        firstValue = average(&_channels[ch_idx].lastValues[0]); //average lastValues[0,1]
        secondValue = average(&_channels[ch_idx].lastValues[2]); //average lastValues[2,3]

        x1 = _channels[ch_idx].curX+1;    //all times x1,xy here
        y1 = getY(ch_idx,secondValue);

        //clearRight();

        if(firstValue==secondValue)     // No changes 
            forecastVal = secondValue;
        else                            // Forecast
        {    
            ////////////////////////////////// FORECAST /////////////////////////////////////////////

            ratio = float(secondValue) - firstValue;
            leftPosRatio = (_rightBottom.x - _channels[ch_idx].curX) / DELTA_P;
            forecastVal = secondValue + (ratio*leftPosRatio);
            if(forecastVal>_channels[ch_idx].maxValue) 
            {

                overVal = abs(fabs(forecastVal) - abs(_channels[ch_idx].maxValue))*_channels[ch_idx].pointsInDivision/_deltaXY;
                forecastVal = _channels[ch_idx].maxValue;

            }
            if(forecastVal<_channels[ch_idx].minValue) 
            {
                overVal = abs(abs(_channels[ch_idx].minValue) - fabs(forecastVal))*_channels[ch_idx].pointsInDivision/_deltaXY;
                forecastVal=_channels[ch_idx].minValue;
            }
            /////////////////////////////////////////////////////////////////////////////////////////
        }

        if(overVal > 0)
        {
            if(overVal<(_rightBottom.x-x1)) x2 = _rightBottom.x-overVal;
            else x2 = x1;
        }
        else x2 = _rightBottom.x;

        y2 = getY(ch_idx, uint16_t(forecastVal));        
        

        _nextion->line(x1, y1, x2, y2, 29614);
    }
};

void Chart::lineG(uint8_t ch_idx,int32_t valueY){
    uint16_t y;
    if(_channels[ch_idx].depth>0){ 
        y = getY(ch_idx, valueY);   
        _nextion->line(_leftTop.x, y, _rightBottom.x, y, _channels[ch_idx].color );
    }
};

void Chart::lineV(uint8_t ch_idx,int32_t valueX){
    if(_channels[ch_idx].depth>0 && valueX <= _channels[ch_idx].maxValue && valueX >= _channels[ch_idx].minValue)
        _nextion->line(valueX, _leftTop.y, valueX, _rightBottom.y, _channels[ch_idx].color );
    
};

void Chart::lineV(uint8_t ch_idx){
    if(_channels[ch_idx].depth>0)
        _nextion->line(_channels[ch_idx].curX, _leftTop.y, _channels[ch_idx].curX, _rightBottom.y, _channels[ch_idx].color );
    
};

void Chart::fill(uint8_t ch_idx,int32_t value,bool before){
    uint16_t y;
    if(_channels[ch_idx].depth>0){ 
        y = getY(ch_idx, value);
        if(before)   
            _nextion->fill(_leftTop.x, y, _rightBottom.x-_leftTop.x,_rightBottom.y-y, _channels[ch_idx].color);
        else
            _nextion->fill(_leftTop.x, _leftTop.y, _rightBottom.x-_leftTop.x,y-_leftTop.y, _channels[ch_idx].color);
    }    
};


