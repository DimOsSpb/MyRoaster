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

struct ChartVLabelStyle {
    bool            Flag             : 1;            // display text flag in current x,y position of chanel (if FlagVAlignment!=3)
    unsigned     FlagGAlignment      : 2;            // 0 - left, 1 - right, 2 - center
    unsigned     FlagVAlignment      : 2;            // 0 - up, 1 - down, 2 - center, 3 - center of Y
    bool            Label            : 1;            // display label in x position of chanel
    unsigned     LabelGAlignment     : 2;            // 0 - left, 1 - right, 2 - center
    unsigned     LabelVAlignment     : 2;            // 0 - up, 1 - down, 2 - center, 3 - center of Y
};

#define CHART_VLSTYLE_DEFAULT    {false,1u,3u,true,2u,1u}
#define CHART_VLSTYLE_PFC        {true,0u,3u,true,0u,1u}
#define CHART_VLSTYLE_FC         {false,0u,0u,true,0u,0u}
#define CHART_VLSTYLE_FINISH     {true,1u,3u,true,1u,0u}
#define CHART_VLSTYLE_PFINISH     {true,1u,3u,true,1u,1u}

struct ChartLabelStyle {
    unsigned     GAlignment     : 2;            // 0 - left, 1 - right, 2 - center
    unsigned     VAlignment     : 2;            // 0 - up, 1 - down, 2 - center
    unsigned     FlagpoleLenght      : 3;            // 0-7 pixels
};

#define CHART_LSTYLE_DEFAULT    {0u,0u,0u}
#define CHART_LSTYLE_FC         {0u,0u,4u}

class Chart{
    
    public:
        Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby);
        void init();
        void initChanel(uint8_t ch_idx,int32_t minValueX,int32_t maxValueX,int32_t minValueY,int32_t maxValueY,uint32_t color,uint8_t depth = 1);
        void clearRight();
        void addChanelValue(uint8_t ch_idx,int32_t valueX,int32_t valueY);
        void label(uint8_t ch_idx,int32_t valueX, int32_t valueY, char *label, ChartLabelStyle labelStyle = CHART_LSTYLE_DEFAULT);   
        void lineG(uint8_t ch_idx,int32_t valueY);                                                                          // Horizontal line by value
        void lineV(uint8_t ch_idx,int32_t valueX, char *label, char *flag, ChartVLabelStyle labelStyle = CHART_VLSTYLE_DEFAULT, uint8_t flag_ch_idx = 0);   // Vertical line by value
        void lineX(uint8_t ch_idx,int32_t pointX);                                                                          // Vertical line by value in pixels
        void lineX(uint8_t ch_idx);                                                                                         // Vertical line in last position on X
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


        const void _printChartText(char *text, uint8_t GAlign, uint8_t VAlign, int32_t x, int32_t y, uint8_t font, uint16_t tcolor, int32_t bcolor = -1);
        void keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue);
        const int32_t getX(uint8_t ch_idx, int32_t _value);
        const int32_t getY(uint8_t ch_idx, int32_t _value);
        const int32_t average(int32_t *_values); //_values[AVERAGE_COUNT]!!
       

     
};

