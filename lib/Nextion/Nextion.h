#pragma once
#include <SoftwareSerial.h>
#include <Arduino.h>


class Nextion{
    public:
        Nextion(uint8_t RX, uint8_t TX);
        void sendCommand(const char* cmd);
        void init();
        uint8_t readInput(uint8_t max, byte *buffer);
        void line(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby,uint16_t color);
        void cropPic(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby);
        void fill(uint16_t ltx,uint16_t lty,uint16_t dx,uint16_t dy,uint16_t color);
        void onPage(uint8_t pageNum);
        uint8_t currentPage();

    private:
        SoftwareSerial _serial;
        uint8_t _ndt[3] = {255,255,255};
        uint8_t _currentPage;
        char _buf[100];
       
         
};