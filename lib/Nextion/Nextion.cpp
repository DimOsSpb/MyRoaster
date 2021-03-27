#include <Nextion.h>

#define CTD_BAUD 115200

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

  // _serial.begin(CTD_BAUD);  //if arduino rebooted, anticipated baud=CTD_BAUD => Start serial comunication at baud=CTD_BAUD
  // _serial.print("rest");    //reset Nextion
  // _serial.write(_ndt,3);
  // delay(500);  // This delay is just in case the nextion display didn't start yet, to be sure it will receive the following command.
  _serial.begin(9600);
  
  // _serial.print("baud=115200");
  // _serial.write(_ndt,3);
  // _serial.end();  // End the serial comunication of baud=9600
  // _serial.begin(115200);  // Start serial comunication at baud=115200

};
String Nextion::readInput(){
  String str;
  if(_serial.available()>0){                                                              
    while(_serial.available())
    {
      str+=char(_serial.read());
      delay(20);
    }
  }
  return str; 
};
void Nextion::line(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby,uint16_t color){
  sprintf(_buf, "line %u,%u,%u,%u,%u",ltx,lty,rbx,rby,color);
  sendCommand(_buf);
};
void Nextion::cropPic(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby){
  sprintf(_buf, "picq %u,%u,%u,%u,%u",ltx,lty,rbx-ltx+1,rby-lty+1,0);
  sendCommand(_buf);

};

void Nextion::fill(uint16_t ltx,uint16_t lty,uint16_t dx,uint16_t dy,uint16_t color){
  sprintf(_buf, "fill %u,%u,%u,%u,%u",ltx,lty,dx,dy,color);
  sendCommand(_buf);
};

void Nextion::onPage(uint8_t pageNum){
  _currentPage = pageNum;    
}

uint8_t Nextion::currentPage(){
  return _currentPage;    
}
