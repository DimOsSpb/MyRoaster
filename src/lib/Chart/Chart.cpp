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

void Chart::init(int32_t minValueX,int32_t maxValueX){
    _chartCurrentX = 0; 
    _minValueX = minValueX;
    _maxValueX = maxValueX;     
    if(_maxValueX > _minValueX)
        _pointsInDivisionX = float(_rightBottom.x-_leftTop.x)/(maxValueX-minValueX);
}
void Chart::initChanel(uint8_t ch_idx,int32_t minValueY,int32_t maxValueY,uint32_t color,uint8_t depth){
    if(ch_idx >= 0 && ch_idx < CHART_CHANELS){
        _channels[ch_idx].counter = 0;
        _channels[ch_idx].lastX = 0;
        _channels[ch_idx].minValueY = minValueY;
        _channels[ch_idx].maxValueY = maxValueY;

        _channels[ch_idx].color = color;
        _channels[ch_idx].depth = depth;
        _channels[ch_idx].lastValues[0] = 0;
        _channels[ch_idx].lastValues[1] = 0;
        _channels[ch_idx].lastValues[2] = 0;

        if(maxValueY > minValueY)
            _channels[ch_idx].pointsInDivisionY = float(_rightBottom.y-_leftTop.y)/(maxValueY-minValueY);

    }
};

void Chart::keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue){
    for (uint8_t i = 0; i < CHARRT_STAT_VALUES-1; i++)
        _channels[ch_idx].lastValues[i] = _channels[ch_idx].lastValues[i+1];
    _channels[ch_idx].lastValues[CHARRT_STAT_VALUES-1] = _filteredValue;
    _channels[ch_idx].lastValue = _value;
};
const int32_t Chart::getX(int32_t _value){
    int32_t _val;
    if(_value<=_minValueX) return _leftTop.x;
    if(_value>_maxValueX) return _rightBottom.x;
    else 
    {
        _val =  _leftTop.x + (_value - _minValueX)*_pointsInDivisionX;
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


        val_x2 = getX(valueX);
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



void Chart::lineV(int32_t valueX, ChartLineStyle style, uint16_t fcolor, int32_t bcolor,char *label){

    int32_t x;

    x = getX(valueX);
    _nextion->line(x, _leftTop.y, x, _rightBottom.y, fcolor );

    if(style.Label)
      
        _printChartText(label, style.LabelStyle, x, _rightBottom.y, 2, fcolor, bcolor);


};

#define _LABEL_HEIGHT 14 
#define _PIXELS_IN_CHAR 10
const void Chart::_printChartText(char *text, ChartLabelStyle aligment, int32_t x, int32_t y, uint8_t font, uint16_t fcolor, int32_t bcolor){
    uint8_t sl,tpl;
    sl = strlen(text);
    tpl = (sl * _PIXELS_IN_CHAR) - sl*3;   // 7 pixels in char (approximately)
    if(tpl>0)
    {
        switch(aligment.LabelAligment.Horisontal)        {
        case 0:
            x = x - tpl;
            break;
        case 1:
            x = x - tpl * 0.5;
            break;
        case 2:
            break;
        }
        if(x < _leftTop.x) x = _leftTop.x;
        if(x > _rightBottom.x - tpl) x = _rightBottom.x - tpl;
        
        
        switch(aligment.LabelAligment.Vertical)
        {
        case 0:
            y = y - _LABEL_HEIGHT;
            break;
        case 1:
            y = y - _LABEL_HEIGHT * 0.5;
            break;
        case 2:
            y = y-1;
            break;            
        }

        

        _nextion->text(&text[0], x, y, tpl, 14, 2, fcolor , bcolor, aligment.BackgroundFill);     
    }
};

void Chart::label(uint8_t ch_idx,int32_t valueX, int32_t valueY, char *label, ChartLabelStyle labelStyle, uint16_t fcolor, int32_t bcolor){

    int32_t x,y, fp_ltx,fp_lty,fp_rbx,fp_rby;


    if(_channels[ch_idx].depth>0){

        x = getX(valueX);
        y = getY(ch_idx, valueY);

        if(labelStyle.FlagpoleLenght > 0)
        {
            switch(labelStyle.FlagAligment.Horisontal)
            {
            case 0: //left
                fp_ltx = x - labelStyle.FlagpoleLenght;
                fp_rbx = x;
                x = fp_ltx;
                break;
            case 1: //center
                fp_ltx = x;
                fp_rbx = x;
                break;
            case 2: //right
                fp_ltx = x;
                fp_rbx = x + labelStyle.FlagpoleLenght;
                x = fp_rbx;
                break;
            }
            switch(labelStyle.FlagAligment.Vertical)
            {
            case 0:
                if(labelStyle.FlagAligment.Horisontal != 2)
                {
                    fp_lty = y - labelStyle.FlagpoleLenght;
                    fp_rby = y;
                }
                else
                {
                    fp_lty = y;
                    fp_rby = y - labelStyle.FlagpoleLenght;
                }
                y = y - labelStyle.FlagpoleLenght;
                break;
            case 1:
                fp_lty = y;
                fp_rby = x;
                break;
            case 2:
                fp_lty = y;
                fp_lty = y + labelStyle.FlagpoleLenght;
                y = fp_lty;
            }


            if(fp_ltx < _leftTop.x) fp_ltx = _leftTop.x;
            if(fp_rbx > _rightBottom.x) fp_rbx = _rightBottom.x;
            if(fp_lty < _leftTop.y) fp_lty = _leftTop.y;
            if(fp_rby > _rightBottom.y) fp_rby = _rightBottom.y;
            
            _nextion->line(fp_ltx, fp_lty, fp_rbx, fp_rby, fcolor );

        }
        _printChartText(label, labelStyle, x, y , 2, fcolor, bcolor);

        
     }
};


