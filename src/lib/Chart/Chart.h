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
    int32_t minValueY,maxValueY;
    int32_t lastValues[CHARRT_STAT_VALUES];     //Filtered Last 3 values
    int32_t lastValue;         //Not Filtered Last
    uint8_t depth;
    uint16_t color;    
    uint16_t counter;
    uint16_t lastX;
    float pointsInDivisionY; 
};

struct VisualAligment
{
    unsigned    Horisontal        : 2;            // 0 - left, 1 - center, 2 - right
    unsigned    Vertical          : 2;            // 0 - up, 1 - center, 2 - down
};

struct ChartLabelStyle {
    VisualAligment  LabelAligment;
    unsigned        BackgroundFill      : 2;            // is background Fill (0 – crop image, 1 – solid color, 2 – image, 3 – none)
    unsigned        FlagpoleLenght      : 3;            // 0-7 pixels
    VisualAligment  FlagAligment;
};
struct ChartLineStyle {
    bool                Label           : 1;            // display label
    ChartLabelStyle     LabelStyle;

};

#define CHART_VLSTYLE_DEFAULT    {false,1u,1u,1u,0u,1u,1u}
#define CHART_VLSTYLE_PFC        {true,0u,2u,3u,0u,1u,1u}
#define CHART_VLSTYLE_FC         {true,0u,0u,3u,0u,1u,1u}
#define CHART_VLSTYLE_FINISH     {true,2u,0u,3u,0u,1u,1u}
#define CHART_VLSTYLE_PFINISH     {true,2u,2u,3u,0u,1u,1u}

#define CHART_LSTYLE_DEFAULT    {0u,0u,1u,0u,1u,1u}
#define CHART_LSTYLE_FC         {0u,0u,3u,6u,0u,0u}

class Chart{
    
    public:
        Chart(Nextion *nextion,uint16_t ltx,uint16_t lty, uint16_t rbx,uint16_t rby);
        void init(int32_t minValueX,int32_t maxValueX);
        void initChanel(uint8_t ch_idx,int32_t minValueY,int32_t maxValueY,uint32_t color,uint8_t depth = 1);
        void clearRight();
        void addChanelValue(uint8_t ch_idx,int32_t valueX,int32_t valueY);
        void label(uint8_t ch_idx,int32_t valueX, int32_t valueY, char *label, ChartLabelStyle labelStyle = CHART_LSTYLE_DEFAULT, uint16_t fcolor = 65535, int32_t bcolor = 0);   
        void lineG(uint8_t ch_idx,int32_t valueY);                                                                          // Horizontal line by value
        void lineV(int32_t valueX, ChartLineStyle style = CHART_VLSTYLE_DEFAULT, uint16_t fcolor = 65535, int32_t bcolor = 0, char *text = NULL);   // Vertical line by value

        void chanelForecast(uint8_t ch_idx, uint16_t color);
        bool fieldIsOver();
    private:
        Nextion *_nextion;
        Channel _channels[CHART_CHANELS];
        Point _leftTop;
        Point _rightBottom;
        float _deltaXY;
        uint16_t _chartCurrentX;
        int32_t _minValueX,_maxValueX;
        float _pointsInDivisionX; 



        const void _printChartText(char *text, ChartLabelStyle aligment, int32_t x, int32_t y, uint8_t font, uint16_t fcolor, int32_t bcolor = -1);
        void keepChartLastValue(uint8_t ch_idx, int32_t _value, int32_t _filteredValue);
        const int32_t getX(int32_t _value);
        const int32_t getY(uint8_t ch_idx, int32_t _value);
        const int32_t average(int32_t *_values); //_values[AVERAGE_COUNT]!!
       

     
};

