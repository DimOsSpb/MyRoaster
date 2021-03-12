#include <Nextion.h>
#include <Dispatcher.h>

// RX - digital output 9, connect on Nextian TX 
// TX - digital output 10, connect on Nextian RX
// Nextian display pins config
#define NEXTIAN_RX 9
#define NEXTIAN_TX 10

Dispatcher::Dispatcher(): _nextion(NEXTIAN_RX, NEXTIAN_TX){};
void Dispatcher::init(Roaster *roaster){
  _nextion.init();
  _nextion.sendCommand("cle 2,255");
  _roaster = roaster;
  _roaster->init();    
};

bool Dispatcher::updateStates(){
    
    RoastProfile *_curProfRec;

    _curProfRec = _roaster->readStates();
    sprintf(_buf, "page0.t_tp.txt=\"TP: %u\"",_curProfRec->chartData[_curProfRec->dataIndex].TP);
    _nextion.sendCommand(_buf);      
    sprintf(_buf, "add 2,0,%u", _curProfRec->chartData[_curProfRec->dataIndex].TP);
    _nextion.sendCommand(_buf);
    sprintf(_buf, "add 2,1,%u", _curProfRec->chartData[_curProfRec->dataIndex].RoR);
    _nextion.sendCommand(_buf);

    Serial.print("TP: ");
    Serial.print(_curProfRec->chartData[_curProfRec->dataIndex].TP);
    Serial.print(" RoR: ");
    Serial.print(_curProfRec->chartData[_curProfRec->dataIndex].RoR);
    Serial.print(" Index: ");
    Serial.println(_curProfRec->dataIndex);
    return true;    
};
