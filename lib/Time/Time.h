#pragma once

#include <Arduino.h>

#define ONE_SEC 1000;
#define ONE_MIN 60000;

struct DHMS
{
    uint8_t Days; 
    uint8_t Hours; 
    uint8_t Mins; 
    uint8_t Secs; 
};


static inline DHMS getDHMS(uint32_t Msecs){
    uint32_t secs = Msecs/1000ul;
    DHMS time;
    time.Days = (secs / 3600ul) / 24;
    time.Hours = (secs / 3600ul) % 24;
    time.Mins = (secs % 3600ul) / 60ul;
    time.Secs = (secs % 3600ul) % 60ul;
    return time;
};

class Timer{
    public:
        Timer();
        Timer(uint32_t periodMs);   //periodMs - Counter in Ms
        void start(uint32_t periodMs);
        bool isReady();
    private:
        uint32_t _periodMs = ONE_SEC;
        uint32_t _timer = 0;
           
};