#include <Nextion.h>

#define CTD_BAUD 38400

Nextion::Nextion(uint8_t RX, uint8_t TX):_serial(RX, TX){};
void Nextion::sendCommand(const char* cmd)
{
   //Serial.println(cmd);
    _serial.print(cmd);
    _serial.write(_ndt,3);
};
void Nextion::init(){

  _serial.begin(9600);

};
uint8_t Nextion::readInput(uint8_t max, byte *buffer){

  uint8_t count = 0, scount = 0, fcount = 0;
  byte cur_byte;

  //char tmp[10];

    while(_serial.available() && count < max)
    {
      cur_byte = _serial.read();
      delay(30);

      if(scount == 2)             // 0xBB 0xBB - command detected, read to the end...
      {
        buffer[count] = cur_byte;          // add byte of command
        count++;
        if(cur_byte == 0xFF)
        {
          if( ++fcount == 3)      // finish of command
          {
            if(count == 3)        // command contains data?
            {
              count = 0, fcount = 0, scount = 0;  // reset all and wait next command
              continue; 
            }
            //buffer[count] = 0x00; // for print as string
            break;                // finish read and return command               
          }
        }
        else if(fcount > 0)
          fcount = 0;             // reset read finish of command
      }
      else if(cur_byte == 0xBB)   // wait for the start of command        
        scount++; 
      else
        scount = 0;


    }

  return count; 
};
void Nextion::line(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby,uint16_t color){
  sprintf_P(_buf, PSTR("line %u,%u,%u,%u,%u"),ltx,lty,rbx,rby,color);
  sendCommand(_buf);
};
void Nextion::cropPic(uint16_t ltx,uint16_t lty,uint16_t rbx,uint16_t rby){
  sprintf_P(_buf, PSTR("picq %u,%u,%u,%u,%u"),ltx,lty,rbx-ltx+1,rby-lty+1,0);
  sendCommand(_buf);

};

// void Nextion::fill(uint16_t ltx,uint16_t lty,uint16_t dx,uint16_t dy,uint16_t color){
//   sprintf(_buf, "fill %u,%u,%u,%u,%u",ltx,lty,dx,dy,color);
//   sendCommand(_buf);
// };

void Nextion::text(char *txt, uint16_t ltx,uint16_t lty,uint16_t dx,uint16_t dy, uint8_t fontN, uint16_t tcolor, uint16_t bcolor, uint8_t bcflag){

  sprintf_P(_buf, PSTR("xstr %u,%u,%u,%u,%u,%u,%u,1,1,%u,\"%s\""),ltx,lty,dx,dy,fontN,tcolor,bcolor,bcflag,txt);
  sendCommand(_buf);
};


void Nextion::onPage(uint8_t pageNum){
  _currentPage = pageNum;    
}

uint8_t Nextion::currentPage(){
  return _currentPage;    
}
