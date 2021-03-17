#pragma once
#include <Nextion.h>
#include <Roaster.h>

class Dispatcher{
    
    public:
        Dispatcher();
        void init();
        void refreshStates();
        void listEvents();
    private:
        Nextion _nextion;
        Roaster _roaster;
        char _buf[100];
        uint16_t _chartIndex,_chartFCIndex;
        uint16_t _chartLastBT,_chartLastRoR;
        uint32_t _lastChartTim;
        RoastProfile _profile;

     
};

