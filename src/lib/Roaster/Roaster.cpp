#include <Roaster.h>

Roaster::Roaster(): _thermo_z_cb(THERMO_Z_CB_CLK, THERMO_Z_CB_CS, THERMO_Z_CB_DO){};
void Roaster::init(){
    _curSates.State = ROASTER_STATE_STOPPED;    
};
void Roaster::initRoRTimer(){
    _timerRoR = millis()+_profile.RoRFreq*1000;
    _curSates.RoRFlag = true;  
};

bool Roaster::start(RoastProfile profile){
    _profile = profile;    
    _timerStart = millis();
    _timerStop = 0;
    _timerRoR = 0;
    //initRoRTimer();
    _curSates.FC = 0;
    _curSates.LeftTime = 0;
    _curSates.StopTime = 0;
    _curSates.StopFlag = false;
    _curSates.RoRFlag = false;
    _curSates.State = ROASTER_STATE_STARTED;    

    return true;    
};
bool Roaster::stop(){
    //readStates();
    _curSates.State = ROASTER_STATE_STOPPED;
    return true;    
};
uint8_t Roaster::getCoffeBeanTemperature(){
    return uint8_t(_thermo_z_cb.readCelsius());
};
RoasterStates *Roaster::readStates(){
    uint32_t _curMillis =  millis();
    if(_curSates.State == ROASTER_STATE_STARTED) {
        _curSates.Time = _curMillis - _timerStart;
        if(_timerStop > 0) 
        {
            if(_curMillis < _timerStop)      
                _curSates.LeftTime = _timerStop - _curMillis;
            else{
                _timerStop = 0;
                _curSates.LeftTime = 0;
                _curSates.StopFlag = true;
            } 
        }   // Stop roast 
    }
    _curSates.BT = getCoffeBeanTemperature();
    _curMillis =  millis();
    if(_curMillis >= _timerRoR){
        if(_curSates.LastRoRBT!=0)
            _curSates.RoR = _curSates.LastRoRBT >= _curSates.BT ? _curSates.LastRoRBT - _curSates.BT :  _curSates.BT - _curSates.LastRoRBT;
        _curSates.LastRoRBT = _curSates.BT;
        initRoRTimer();
    }
    
    

    
    return &_curSates;
};
void Roaster::FC(uint8_t DevelopmentTimeRatio){
    uint32_t _curMillis;
    if(_curSates.State == ROASTER_STATE_STARTED) {
        _curMillis = millis();
        _curSates.FC =  _curMillis - _timerStart;
        _timerStop = _curMillis + (_curSates.FC / (100-DevelopmentTimeRatio)) * DevelopmentTimeRatio;
        _curSates.StopTime = _timerStop - _timerStart;
        _curSates.StopFlag = false;
        Serial.print("TStart:"); Serial.print(_timerStart); Serial.print(" TFC:");  Serial.print(_curMillis);  Serial.print(" TStop");  Serial.print(_timerStop);
        
    }
};
void Roaster::RL(uint8_t valRL){
    _curSates.RL = valRL;
};

