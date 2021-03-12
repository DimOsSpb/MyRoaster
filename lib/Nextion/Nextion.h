#pragma once
#include <SoftwareSerial.h>
#include <Arduino.h>


class Nextion{
    public:
        Nextion(uint8_t RX, uint8_t TX);
        void sendCommand(const char* cmd);
        void init();
        String readInput();
    private:
        SoftwareSerial _serial;
        uint8_t _ndt[3] = {255,255,255};
         
};