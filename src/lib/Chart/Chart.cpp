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

void Chart::initChanel(uint8_t ch_idx,uint16_t MinValue,uint16_t MaxValue,uint16_t color,uint8_t depth){
    if(ch_idx >= 0 && ch_idx < CHART_CHANELS){
        _channels[ch_idx].minValue = MinValue;
        _channels[ch_idx].maxValue = MaxValue;
        _channels[ch_idx].color = color;
        _channels[ch_idx].depth = depth;
        _channels[ch_idx].lastValues[0] = 0;
        _channels[ch_idx].lastValues[1] = 0;
        _channels[ch_idx].lastValues[2] = 0;
        if(MaxValue>0)_channels[ch_idx].pointsInDivision = float(_rightBottom.y-_leftTop.y)/MaxValue;
    }
};

void Chart::keepChartLastValue(uint8_t ch_idx, uint16_t _value, uint16_t _filteredValue){
    for (uint8_t i = 0; i < CHARRT_STAT_VALUES-1; i++)
        _channels[ch_idx].lastValues[i] = _channels[ch_idx].lastValues[i+1];
    _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1] = _filteredValue;
    _channels[ch_idx].lastValue = _value;
};

uint16_t Chart::getY(uint8_t ch_idx,uint16_t _value){
    uint16_t val_y;

    if(_value>_channels[ch_idx].maxValue) val_y = _leftTop.y;
    else val_y = _rightBottom.y - _value*_channels[ch_idx].pointsInDivision;

    return val_y;
};

bool Chart::fieldIsOver(){
    for (uint8_t i = 0; i < CHART_CHANELS; i++)
        if(_channels[i].curPos>_rightBottom.x)
            return true;
    return false;
    

};

void Chart::addChanelValue(uint8_t ch_idx,uint16_t _value){
    uint16_t val_x1,val_y1,val_y2;
    int32_t last_val,filtered_val;
                                    // filtered_val - фильтрованное значение
                                    // val - новое значение (с датчика)
                                    // CHART_VALUE_FILTER_RATIO - коэффициент фильтрации 0.. 1. Обычно около 0.01-0.1 (то бишь float)  

    if(ch_idx >= 0  && ch_idx < CHART_CHANELS){
        if(_channels[ch_idx].curPos) {
            last_val = _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1];
            if(_channels[ch_idx].lastValue != _value ) filtered_val = last_val + (_value - last_val) * CHART_FILTER_RATIO;
            else filtered_val = _value;
        }
        else{
            last_val = _value;
            filtered_val = _value;
        }

        //   Serial.println(last_val);
        //   Serial.println(filtered_val);

        val_y2 = getY(ch_idx, filtered_val);
        if(_channels[ch_idx].curPos){
            val_x1 = _channels[ch_idx].curPos - 1;
            val_y1 = getY(ch_idx, last_val);
        }
        else { 
            val_x1 = _channels[ch_idx].curPos;
            val_y1 = val_y2;
        }
        
    // Serial.print("pointsInDivision:"); Serial.println(_channels[ch_idx].pointsInDivision);
    // Serial.print("x1:"); Serial.print(val_x1);
    // Serial.print(" y1:"); Serial.print(val_y1);
    // Serial.print(" x2:"); Serial.print(_channels[ch_idx].curPos);
    // Serial.print(" y2:"); Serial.println(val_y2);
    
        _nextion->line( val_x1,
                        val_y1,
                        _channels[ch_idx].curPos,
                        val_y2,
                        _channels[ch_idx].color);
        _channels[ch_idx].curPos++;
        keepChartLastValue(ch_idx, _value, filtered_val);
    }
};

void Chart::chanelForecast(uint8_t ch_idx){
    float   changeRatio,        //Value Ghange ratio 
            leftTimeRatio;      //Сhange time / Left time Ratio

    uint16_t x1,x2,y1,y2,forecastVal,overVal;

    if(_channels[ch_idx].curPos>CHARRT_STAT_VALUES-1) {      //we have stat

        x1 = _channels[ch_idx].curPos-1;                    //all times x1,xy here
        y1 = getY(ch_idx,_channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1]);

        // if(_channels[ch_idx].lastValues[1]!=0) 
        //     k = float(_channels[ch_idx].lastValues[2])/_channels[ch_idx].lastValues[1];  

        if(_channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1]==_channels[ch_idx].lastValues[0])
            forecastVal = _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1];
        else
        {    
            changeRatio = float(_channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1])/_channels[ch_idx].lastValues[0];
            leftTimeRatio = 10; //(_rightBottom.x - _channels[ch_idx].curPos)/20;                           //2 pos (time)
            forecastVal = _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1]*changeRatio;
     Serial.print("forecastVal="); Serial.print(_channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1]); Serial.print(" * "); Serial.print( changeRatio); Serial.print(" * "); Serial.println(leftTimeRatio);
        }

        y2 = getY(ch_idx, forecastVal);        
        x2 = _rightBottom.x;

        _nextion->cropPic(x1, _leftTop.y, x2, _rightBottom.y);

        if(forecastVal>_channels[ch_idx].maxValue){
        
            overVal = (forecastVal - _channels[ch_idx].maxValue)*_channels[ch_idx].pointsInDivision/_deltaXY;
        Serial.print("overVal="); Serial.println(overVal);
        Serial.print("x2-x1="); Serial.println(x2-x1);
            if(overVal<(x2-x1)) x2 -= overVal; 
        }
        


    // Serial.print("last-y:"); Serial.print(_channels[ch_idx].lastValues[2]);
    // Serial.print(" forecastVal:"); Serial.println(forecastVal);
     
    _nextion->line(x1, y1, x2, y2, 33840);
    }
};



