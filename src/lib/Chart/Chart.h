#pragma once
#include <Nextion.h>


#define CHART_CHANELS 2
#define CHART_FILTER_RATIO 0.5   //0.01-0.1
#define CHARRT_STAT_VALUES 10

struct Point{
    uint16_t x,y;
};

struct ChartSpec
{
    Point leftTop;
    Point rightBottom;

};

struct Channel{
    uint16_t minValue,maxValue;
    uint16_t lastValues[CHARRT_STAT_VALUES];     //Filtered Last 3
    uint16_t lastValue;         //Not Filtered Last
    uint8_t depth;
    uint16_t color;    
    uint16_t curPos;
    float pointsInDivision; 
};

class Chart{
    
    public:
        Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby);
        void initChanel(uint8_t ch_idx,uint16_t minValue,uint16_t maxValue,uint16_t color,uint8_t depth=1);
        void addChanelValue(uint8_t ch_idx,uint16_t value);
        void chanelForecast(uint8_t ch_idx);
        bool fieldIsOver(); 
    private:
        Nextion *_nextion;
        Channel _channels[CHART_CHANELS];
        Point _leftTop;
        Point _rightBottom;
        float _deltaXY;

        void keepChartLastValue(uint8_t ch_idx, uint16_t _value, uint16_t _filteredValue);
        uint16_t getY(uint8_t ch_idx, uint16_t _value);
       

     
};

