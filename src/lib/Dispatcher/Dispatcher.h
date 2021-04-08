#pragma once
#include <Nextion.h>
#include <Roaster.h>
#include <Chart.h>

#define NEXTIAN_RX 9
#define NEXTIAN_TX 10
#define GRAPH_FREQUENCY 2500    //in MS - 2,5secs
#define CHART_DX 320 
#define CHART_DY 120 

#define CHART_X_MIN 0
#define CHART_X_MAX_RATIO 1000*60   // 1 min in millis

#define CHART_BT_CRECK 220

#define CHART_BT_MAX 260
#define CHART_ROR_MAX 35

#define CHART_ROR_COLOR 65535u
#define CHART_BT_COLOR 13663u
#define CHART_FC_COLOR 64267u
#define CHART_MAX_BT_COLOR 63488u
#define CHART_BT220_COLOR 31281u
#define CHART_PDT_COLOR 63488u
#define CHART_PFC_COLOR 4127u


#define BTN_ST_ON_COMMAND 10
#define BTN_ST_OFF_COMMAND 11
#define BTN_FC_ON_COMMAND 20
#define BTN_FC_OFF_COMMAND 21
#define BTN_SC_ON_COMMAND 30
#define BTN_SC_OFF_COMMAND 31
#define RL_COMMAND 100
#define RT_COMMAND 101
#define ON_PAGE_COMMAND 200

#define BTN_ST_ON_DOWN_TEXT "\r\n Start" 
#define BTN_ST_OFF_DOWN_TEXT "\r\n Stop"
#define BTN_FC_ON_DOWN_TEXT "\r\n FCE" 
#define BTN_FC_OFF_DOWN_TEXT "\r\n FC"
#define BTN_SC_ON_DOWN_TEXT "\r\n SCE" 
#define BTN_SC_OFF_DOWN_TEXT "\r\n SC"

#define DEFAULT_ROR_FREQ 15     //in secs
#define DEFAULT_DTR 22          //Development Time Ratio (DTR) 15-25% (20-25% rec. https://www.scottrao.com/blog/2016/8/25/development-time-ratio )
#define DEFAULT_RL 7            //Default Roast Level
#define DEFAULT_PDT 12          //Planned Development Time

struct RoastLevels
{
    uint8_t GroupeIndex;
    String Name;

};

class Dispatcher{
    
    public:
        Dispatcher();
        void init();
        void refreshStates();
        void listEvents();
        void startRoast();
        void stopRoast();
        void startFirstCrack();
        void stopFirstCrack();
        void startSecondCrack();
        void stopSecondCrack();
        void changeRoastLevel(uint8_t level,bool _reflect);
        void changeRoastTime(uint8_t value,bool _reflect);
    private:
        Nextion _nextion;
        Roaster _roaster;
        Chart _chart;

        uint16_t _chartIndex,_chartFCIndex;
        uint16_t _chartLastBT,_chartLastRoR;
        uint32_t _lastChartTim;

        RoastProfile _profile;
        char _buf[100];
        void _reflectChanges_RL();
        void _reflectChanges_PDT();
        void _onNextionPage(uint8_t page);
        void _initChart();

     
};

