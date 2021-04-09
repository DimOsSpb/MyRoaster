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
    init();
};

void Chart::init(){
    _chartCurrentX = 0;  
}
void Chart::initChanel(uint8_t ch_idx,int32_t minValueX,int32_t maxValueX,int32_t minValueY,int32_t maxValueY,uint16_t color,uint8_t depth){
    if(ch_idx >= 0 && ch_idx < CHART_CHANELS){
        _channels[ch_idx].counter = 0;
        _channels[ch_idx].lastX = 0;
        _channels[ch_idx].minValueY = minValueY;
        _channels[ch_idx].maxValueY = maxValueY;
        _channels[ch_idx].minValueX = minValueX;
        _channels[ch_idx].maxValueX = maxValueX;
        _channels[ch_idx].color = color;
        _channels[ch_idx].depth = depth;
        _channels[ch_idx].lastValues[0] = 0;
        _channels[ch_idx].lastValues[1] = 0;
        _channels[ch_idx].lastValues[2] = 0;
        if(maxValueY > minValueY)
            _channels[ch_idx].pointsInDivisionY = float(_rightBottom.y-_leftTop.y)/(maxValueY-minValueY);
        if(maxValueX > minValueX)
            _channels[ch_idx].pointsInDivisionX = float(_rightBottom.x-_leftTop.x)/(maxValueX-minValueX);
        
    }
};

void Chart::keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue){
    for (uint8_t i = 0; i < CHARRT_STAT_VALUES-1; i++)
        _channels[ch_idx].lastValues[i] = _channels[ch_idx].lastValues[i+1];
    _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1] = _filteredValue;
    _channels[ch_idx].lastValue = _value;
};
const int32_t Chart::getX(uint8_t ch_idx,int32_t _value){
    int32_t _val;
    if(_value<=_channels[ch_idx].minValueX) return _leftTop.x;
    if(_value>_channels[ch_idx].maxValueX) return _rightBottom.x;
    else 
    {
        _val =  _leftTop.x + (_value - _channels[ch_idx].minValueX)*_channels[ch_idx].pointsInDivisionX;
        if(_val > _rightBottom.x) return _rightBottom.x; 
    }
    return _val;
};

const int32_t Chart::getY(uint8_t ch_idx,int32_t _value){
    int32_t val_y;
    if(_value<=_channels[ch_idx].minValueY) return _rightBottom.y;
    if(_value>_channels[ch_idx].maxValueY) return _leftTop.y;
    else 
    {
        val_y = _rightBottom.y - (_value - _channels[ch_idx].minValueY)*_channels[ch_idx].pointsInDivisionY;
        if(val_y > _rightBottom.y) return _rightBottom.y; 
        if(val_y < _leftTop.y) {
// Serial.print("val_Y"); Serial.println(val_y);            
// Serial.print("_leftTop.y"); Serial.println(_leftTop.y);            
            //
        return _leftTop.y; 
        }
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
        if(_channels[i].lastX>=_rightBottom.x)
            return true;
    return false;
    

};

void Chart::clearRight(){
    _nextion->cropPic(_chartCurrentX, _leftTop.y, _rightBottom.x, _rightBottom.y);
};

void Chart::addChanelValue(uint8_t ch_idx,int32_t valueX,int32_t _valueY){
    uint16_t val_x1,val_x2,val_y1,val_y2;
    int32_t last_val,filtered_val;
    // Serial.print("BT=220 - ");Serial.println(getY(0, 220));
    // Serial.print("BT=200 - ");Serial.println(getY(0, 200));
    // Serial.print("BT=196 - ");Serial.println(getY(0, 196));
    // Serial.println("---");

    if(ch_idx >= 0  && ch_idx < CHART_CHANELS && _channels[ch_idx].depth>0){
        if(_channels[ch_idx].counter) {
            last_val = _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1];
            if(_channels[ch_idx].lastValue != _valueY ) 
                filtered_val = last_val + (_valueY - last_val) * CHART_FILTER_RATIO; // CHART_VALUE_FILTER_RATIO - коэффициент фильтрации 0.. 1. Обычно около 0.01-0.1 (то бишь float)
            else filtered_val = _valueY;
        }
        else{
            last_val = _valueY;
            filtered_val = _valueY;
        }


        val_x2 = getX(ch_idx, valueX);
        val_y2 = getY(ch_idx, filtered_val);
        if(_channels[ch_idx].counter){
            val_x1 = _channels[ch_idx].lastX;
            val_y1 = getY(ch_idx, last_val);
        }
        else { 
            val_x1 = val_x2;
            val_y1 = val_y2;
        }

        for (uint8_t i = 0; i < _channels[ch_idx].depth; i++)
        {
            uint8_t y1 = val_y1 > _leftTop.y ? val_y1-i : val_y1,
                    y2 = val_y2 > _leftTop.y ? val_y2-i : val_y2;
            _nextion->line( val_x1, y1, val_x2, y2, _channels[ch_idx].color );
                
        }
        
        _channels[ch_idx].lastX = val_x2;
        _channels[ch_idx].counter++;

       if(_channels[ch_idx].lastX > _chartCurrentX) _chartCurrentX = _channels[ch_idx].lastX;

        keepChartLastValue(ch_idx, _valueY, filtered_val);
    }
};

#define DELTA_P 2 //difference currentX between average lastValues[0,1,2] and average lastValues[3,4,5]
void Chart::chanelForecast(uint8_t ch_idx, uint16_t color){
    int32_t firstValue,secondValue,overVal=0;
    float forecastVal;
    float ratio;            //Value Ghange ratio
    float leftPosRatio;     //Left Pos / DELTA_P Ratio
        

    uint16_t x1,x2,y1,y2;

    if(_channels[ch_idx].depth > 0 && _channels[ch_idx].counter > CHARRT_STAT_VALUES-1 && _channels[ch_idx].lastX<_rightBottom.x-1) {     //if statistics accumulated


        firstValue = average(&_channels[ch_idx].lastValues[0]); //average lastValues[0,1]
        secondValue = average(&_channels[ch_idx].lastValues[2]); //average lastValues[2,3]

        x1 = _channels[ch_idx].lastX+1;    //all times x1,xy here
        y1 = getY(ch_idx,secondValue);

        //clearRight();

        if(firstValue==secondValue)     // No changes 
            forecastVal = secondValue;
        else                            // Forecast
        {    
            ////////////////////////////////// FORECAST /////////////////////////////////////////////

            ratio = float(secondValue) - firstValue;
            leftPosRatio = (_rightBottom.x - _channels[ch_idx].lastX) / DELTA_P;
            forecastVal = secondValue + (ratio*leftPosRatio);
            if(forecastVal>_channels[ch_idx].maxValueY) 
            {

                overVal = abs(fabs(forecastVal) - abs(_channels[ch_idx].maxValueY))*_channels[ch_idx].pointsInDivisionY/_deltaXY;
                forecastVal = _channels[ch_idx].maxValueY;

            }
            if(forecastVal<_channels[ch_idx].minValueY) 
            {
                overVal = abs(abs(_channels[ch_idx].minValueY) - fabs(forecastVal))*_channels[ch_idx].pointsInDivisionY/_deltaXY;
                forecastVal=_channels[ch_idx].minValueY;
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
        

        _nextion->line(x1, y1, x2, y2, color);
    }
};

void Chart::lineG(uint8_t ch_idx,int32_t valueY){
    uint16_t y;
    if(_channels[ch_idx].depth>0){ 
        y = getY(ch_idx, valueY);   
        _nextion->line(_leftTop.x, y, _rightBottom.x, y, _channels[ch_idx].color );
    }
};

void Chart::lineV(uint8_t ch_idx,int32_t valueX, char *label, char *flag, ChartLabelStyle labelStyle, uint8_t flag_ch_idx){

    int32_t x,tx;

    if(_channels[ch_idx].depth>0 && valueX <= _channels[ch_idx].maxValueX && valueX >= _channels[ch_idx].minValueX){

        tx = valueX * _channels[ch_idx].pointsInDivisionX;
        x = tx - (_channels[ch_idx].depth * 0.5);
        if(x < _leftTop.x) x = _leftTop.x;
        for (uint8_t i = 0; i < _channels[ch_idx].depth; i++)
        {
            _nextion->line(x, _leftTop.y, x, _rightBottom.y, _channels[ch_idx].color );
            if(x < _rightBottom.x) x++ ;
            else x = _rightBottom.x;
        }

        if(labelStyle.Label)
            _printChartText(label, labelStyle.LabelGAlignment, labelStyle.LabelVAlignment, tx, _rightBottom.y, 2, 65535, 3);

        if(labelStyle.Flag)
        {

            _printChartText(flag, labelStyle.FlagGAlignment, labelStyle.FlagVAlignment, tx, getY(flag_ch_idx,_channels[flag_ch_idx].lastValue) , 2, 65535, _channels[ch_idx].color);
        }
    
    }
};

#define _LABEL_HEIGHT 14 
#define _PIXELS_IN_CHAR 7
const void Chart::_printChartText(char *text, uint8_t GAlign, uint8_t VAlign, int32_t x, int32_t y, uint8_t font, uint16_t tcolor, uint16_t bcolor){
    uint8_t tpl, bgflag = 3;
    tpl = strlen(text) * _PIXELS_IN_CHAR;   // 7 pixels in char (approximately)
    if(tpl>0)
    {
        switch(GAlign)
        {
        case 0:
            x = x - tpl;
            break;
        case 2:
            x = x - tpl * 0.5;
            break;
        case 1:
            break;
        }
        if(x < _leftTop.x) x = _leftTop.x;
        if(x > _rightBottom.x - tpl) x = _rightBottom.x - tpl;
        
        
        switch(VAlign)
        {
        case 0:
            y = y - _LABEL_HEIGHT;
            break;
        case 1:
            y = y-1;
            break;            
        case 2:
            y = y - _LABEL_HEIGHT * 0.5;
            break;
        case 3:
            y = _rightBottom.y - ((_rightBottom.y - _leftTop.y) * 0.5);
            bgflag = 1; //solid color
            break;
        }

        _nextion->text(&text[0], x, y, tpl, 14, 2, tcolor , bcolor, bgflag);     
    }
};

void Chart::lineX(uint8_t ch_idx,int32_t positionX){
    if(_channels[ch_idx].depth>0 && positionX <= _rightBottom.x && positionX >= _leftTop.x)
        _nextion->line(positionX, _leftTop.y, positionX, _rightBottom.y, _channels[ch_idx].color );
    
};

void Chart::lineX(uint8_t ch_idx){
    if(_channels[ch_idx].depth>0)
        _nextion->line(_channels[ch_idx].lastX, _leftTop.y, _channels[ch_idx].lastX, _rightBottom.y, _channels[ch_idx].color );
    
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


