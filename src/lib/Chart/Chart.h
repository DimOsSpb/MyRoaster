#pragma once
#include <Nextion.h>


#define CHART_CHANELS 7
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
    int32_t minValueX,maxValueX,minValueY,maxValueY;
    int32_t lastValues[CHARRT_STAT_VALUES];     //Filtered Last 3 values
    int32_t lastValue;         //Not Filtered Last
    uint8_t depth;
    uint16_t color;    
    uint16_t counter;
    uint16_t lastX;
    float pointsInDivisionY; 
    float pointsInDivisionX; 
};

class Chart{
    
    public:
        Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby);
        void init();
        void initChanel(uint8_t ch_idx,int32_t minValueX,int32_t maxValueX,int32_t minValueY,int32_t maxValueY,uint16_t color,uint8_t depth = 1);
        void clearRight();
        void addChanelValue(uint8_t ch_idx,int32_t valueX,int32_t valueY);
        void lineG(uint8_t ch_idx,int32_t valueY);                      // Horizontal line by value
        void lineV(uint8_t ch_idx,int32_t valueX, String txt = "");     // Vertical line by value
        void lineX(uint8_t ch_idx,int32_t pointX);                      // Vertical line by value in pixels
        void lineX(uint8_t ch_idx);                                     // Vertical line in last position on X
        void fill(uint8_t ch_idx,int32_t value,bool before = false);
        void chanelForecast(uint8_t ch_idx, uint16_t color);
        bool fieldIsOver();
    private:
        Nextion *_nextion;
        Channel _channels[CHART_CHANELS];
        Point _leftTop;
        Point _rightBottom;
        float _deltaXY;
        uint16_t _chartCurrentX;

        void keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue);
        const int32_t getX(uint8_t ch_idx, int32_t _value);
        const int32_t getY(uint8_t ch_idx, int32_t _value);
        const int32_t average(int32_t *_values); //_values[AVERAGE_COUNT]!!
       

     
};

