#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Dispatcher.h>
#include <Roaster.h>
#include <Time.h>

// RX - digital output 9, connect on Nextian TX 
// TX - digital output 10, connect on Nextian RX
// Nextian display pins config
#define NEXTIAN_RX 9
#define NEXTIAN_TX 10

Dispatcher::Dispatcher(): _nextion(NEXTIAN_RX, NEXTIAN_TX){
};

void Dispatcher::init(){
  _nextion.init();
  _nextion.sendCommand("cle 2,255");
  _roaster.init();    
};

void Dispatcher::refreshStates(){
    
    RoasterStates *_curRoasterStates = _roaster.readStates();
    DHMS time = getDHMS(_curRoasterStates->Time);


    sprintf(_buf, "page0.t_tp.txt=\"TP: %u\"",_curRoasterStates->TP);
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "page0.t_st.txt=\"%02u:%02u\"",time.Mins,time.Secs);
    _nextion.sendCommand(_buf);  

    if(_curRoasterStates->State == ROASTER_STATE_STARTED){
        sprintf(_buf, "add 2,0,%u", _curRoasterStates->TP/2);
        _nextion.sendCommand(_buf);
        sprintf(_buf, "add 2,1,%u", _curRoasterStates->RoR/2);
        _nextion.sendCommand(_buf);
    }



    Serial.print("Time: "); Serial.print(time.Mins);  Serial.print(":");  Serial.print(time.Secs);
    Serial.print(" TP: ");
    Serial.print(_curRoasterStates->TP);
    Serial.print(" RoR: ");
    Serial.println(_curRoasterStates->RoR);
     
};

void Dispatcher::listEvents(){
    String input = _nextion.readInput();
    Serial.print("input :");
    Serial.println(input);
    for(unsigned int i=0; i<input.length(); i++){
        if(memcmp(&input[i],"Start" ,5)==0){
            _roaster.start();
            _nextion.sendCommand("cle 2,255");            
        }
        else if(memcmp(&input[i],"Stop" ,5)==0){
            _roaster.stop();
        }
    }
}





