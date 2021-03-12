#include <Nextion.h>

Nextion::Nextion(uint8_t RX, uint8_t TX):_serial(RX, TX){};
void Nextion::sendCommand(const char* cmd)
{
    // while (Nextion.available())
    // {
    //     Nextion.read();
    // }
    
    _serial.print(cmd);
    _serial.write(_ndt,3);
};
void Nextion::init(){
  _serial.begin(9600);
}
