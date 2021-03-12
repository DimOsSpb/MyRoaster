#include <Roaster.h>

Roaster::Roaster(): _thermo_z_cb(THERMO_Z_CB_CLK, THERMO_Z_CB_CS, THERMO_Z_CB_DO){};
void Roaster::init(){
    _curSates.State = ROASTER_STATE_STOPPED;    
};
bool Roaster::start(){
    _timerStart = millis();
    _curSates.State = ROASTER_STATE_STARTED;
    return true;    
};
bool Roaster::stop(){
    _curSates.State = ROASTER_STATE_STOPPED;
    return true;    
};
uint8_t Roaster::getCoffeBeanTemperature(){
    return uint8_t(_thermo_z_cb.readCelsius());
};
RoasterStates *Roaster::readStates(){
    
    if(_curSates.State == ROASTER_STATE_STARTED) _curSates.Time = millis() - _timerStart;
    _curSates.LastTP = _curSates.TP;
    _curSates.TP = getCoffeBeanTemperature();
    _curSates.RoR = _curSates.TP + (_curSates.LastTP - _curSates.TP);
    
    return &_curSates;
};

