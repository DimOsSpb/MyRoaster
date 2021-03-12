#pragma once
#include <max6675.h>

// Coffe Bean Thermo Zond pins config
#define THERMO_Z_CB_DO 4
#define THERMO_Z_CB_CS 5
#define THERMO_Z_CB_CLK 6

#define ROASTER_STATE_STARTED 1
#define ROASTER_STATE_STOPPED 0
#define ROASTER_STATE_FALSE 255

struct RoasterStates
{
    uint32_t Time;
    uint8_t State;
    uint8_t LastTP;
    uint8_t TP;            //TP is the turning point, or the lowest point of the temperature trough and delineates the moment the cold beans hit the roaster.
    uint8_t RoR;           //Delta of curent cofeeBeansTemp & last sec cofee bean temperature. (Rate of rise, or ROR, is linked to bean temperature (or BT; discussed below), and is the rate or speed at which this little green seed is transforming into a darker hew.)
    uint16_t FC;            //First crack in Ms from start of roast
    uint8_t BT;             //BT stands for bean temperature and it will change drastically due to changes occurring within the bean both chemically and physically since the crack means the bean can release moisture and, in so doing, change the temperature within the drum. This, for many reasons, is incredibly important and noted in the graph.
};


class Roaster{
    
    public:
        Roaster();
        void init();
        uint8_t getCoffeBeanTemperature();  //Current temperature of coffee bean 0-255
        bool start();
        bool stop();
        RoasterStates *readStates();
    private:
        MAX6675 _thermo_z_cb;               //Coffe bean thermo zond
        uint32_t _timerStart;               //keep millis() value on start roast 
        RoasterStates _curSates;
         
};

