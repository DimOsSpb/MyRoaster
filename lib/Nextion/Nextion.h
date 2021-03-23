#pragma once
#include <SoftwareSerial.h>
#include <Arduino.h>


class Nextion{
    public:
        Nextion(uint8_t RX, uint8_t TX);
        void sendCommand(const char* cmd);
        void init();
        String readInput();
        void line(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby,uint16_t color);
        void cropPic(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby);
    private:
        SoftwareSerial _serial;
        uint8_t _ndt[3] = {255,255,255};
        char _buf[100];
       
         
};