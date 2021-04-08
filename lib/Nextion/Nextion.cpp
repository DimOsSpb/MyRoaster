#include <Nextion.h>

#define CTD_BAUD 38400

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
  //Serial.println("baud="+CTD_BAUD);
  // _serial.print("baud=38400");
  // _serial.write(_ndt,3);
  // _serial.end();  // End the serial comunication of baud=9600
  // _serial.begin(115200);  // Start serial comunication at baud=115200

};
uint8_t Nextion::readInput(uint8_t max, byte *buffer){

  uint8_t count = 0, scount = 0, fcount = 0;
  byte cur_byte;

  //char tmp[10];

    while(_serial.available() && count < max)
    {
      cur_byte = _serial.read();
      delay(30);

      // sprintf(tmp,"%X ",cur_byte);
      // Serial.print(tmp);
      // Serial.print("count="); Serial.println(count);

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

void Nextion::text(String txt, uint16_t ltx,uint16_t lty,uint16_t dx,uint16_t dy, uint8_t fontN, uint16_t color){


  
  sprintf(_buf, "xstr %u,%u,%u,%u,%u,%u,0,1,1,0,\"%s\"",ltx,lty,dx,dy,fontN,color,txt.c_str());

//Serial.print("_buf=");Serial.println(_buf);

  sendCommand(_buf);
};


void Nextion::onPage(uint8_t pageNum){
  _currentPage = pageNum;    
}

uint8_t Nextion::currentPage(){
  return _currentPage;    
}
