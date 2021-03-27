#pragma once
#include <Nextion.h>


#define CHART_CHANELS 5
#define CHART_FILTER_RATIO 0.5   //0.01-0.1
#define CHARRT_STAT_VALUES 4

struct Point{
    uint16_t x,y;
};

struct ChartSpec
{
    Point leftTop;
    Point rightBottom;

};

struct Channel{
    int32_t minValue,maxValue;
    int32_t lastValues[CHARRT_STAT_VALUES];     //Filtered Last 3
    int32_t lastValue;         //Not Filtered Last
    uint8_t depth;
    uint16_t color;    
    uint16_t curX;
    float pointsInDivision; 
};

class Chart{
    
    public:
        Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby);
        void initChanel(uint8_t ch_idx,int32_t minValue,int32_t maxValue,uint16_t color,uint8_t depth = 1);
        void clearRight();
        void addChanelValue(uint8_t ch_idx,int32_t value);
        void lineG(uint8_t ch_idx,int32_t valueY);
        void lineV(uint8_t ch_idx,int32_t valueX);
        void lineV(uint8_t ch_idx);
        void fill(uint8_t ch_idx,int32_t value,bool before = false);
        void chanelForecast(uint8_t ch_idx);
        bool fieldIsOver(); 
    private:
        Nextion *_nextion;
        Channel _channels[CHART_CHANELS];
        Point _leftTop;
        Point _rightBottom;
        float _deltaXY;
        uint16_t _chartCurX;

        void keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue);
        const int32_t getY(uint8_t ch_idx, int32_t _value);
        const int32_t average(int32_t *_values); //_values[AVERAGE_COUNT]!!
       

     
};

