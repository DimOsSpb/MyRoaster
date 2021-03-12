#pragma once
#include <Arduino.h>
#define ONE_SEC 1000;
#define ONE_MIN 60000;

class Timer{
    public:
        Timer(uint32_t periodMs);   //periodMs - Counter in Ms
        bool isReady();
    private:
        uint32_t _periodMs = ONE_SEC;
        uint32_t _timer = 0;
           
};