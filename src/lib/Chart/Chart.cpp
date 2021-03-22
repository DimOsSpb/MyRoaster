#include <Chart.h>

Chart::Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby){
    _nextion = nextion;
    _leftTop.x = ltx;
    _leftTop.y = lty;
    _rightBottom.x = rbx;
    _rightBottom.y = rby;
};

void Chart::initChanel(uint8_t ch_idx,uint16_t MinValue,uint16_t MaxValue,uint16_t color,uint8_t depth){
    if(0 <= ch_idx && ch_idx < CHART_CHANELS){
        _channels[ch_idx].minValue = MinValue;
        _channels[ch_idx].maxValue = MaxValue;
        _channels[ch_idx].color = color;
        _channels[ch_idx].depth = depth;
        _channels[ch_idx].lastValues[0] = {0,0};
        _channels[ch_idx].lastValues[1] = {0,0};
        _channels[ch_idx].lastValues[2] = {0,0};
        if(_rightBottom.y>_leftTop.y)_channels[ch_idx].pointsInDivision = MaxValue/(_rightBottom.y-_leftTop.y);
    }
};

void Chart::keepChartLastValue(uint8_t ch_idx, Point _value){
    _channels[ch_idx].lastValues[0] = _channels[ch_idx].lastValues[1];
    _channels[ch_idx].lastValues[1] = _channels[ch_idx].lastValues[2];
    _channels[ch_idx].lastValues[2] = _value;
};


void Chart::addChanelValue(uint8_t ch_idx,uint8_t _value){
    uint16_t val;
    if(0 <= ch_idx && ch_idx < CHART_CHANELS){
        //   Serial.println(_channels[ch_idx].maxValue);
        //   Serial.println(_value);
        //   Serial.println(_channels[ch_idx].pointsInDivision);
          if(_value>_channels[ch_idx].maxValue) val = _leftTop.y;
          else val = _rightBottom.y - _value*_channels[ch_idx].pointsInDivision;
        //val = _channels[ch_idx].pointsInDivision*(_value>_channels[ch_idx].maxValue?_channels[ch_idx].maxValue:_value);
        //  Serial.println(val);


        _nextion->line( _channels[ch_idx].lastValues[2].x,
                        _channels[ch_idx].lastValues[2].y,
                        _channels[ch_idx].curPos,
                        val,
                        _channels[ch_idx].color);
        _channels[ch_idx].curPos++;
        keepChartLastValue(ch_idx, {_channels[ch_idx].curPos,val});
    }
};





