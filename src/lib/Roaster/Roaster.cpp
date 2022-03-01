#include <Roaster.h>

Roaster::Roaster(): _thermo_z_cb(THERMO_Z_CB_CLK, THERMO_Z_CB_CS, THERMO_Z_CB_DO){};
void Roaster::init(){
    _curStates.Stage = STOPPED;    
};
void Roaster::initRoRTimer(){
    _timerRoR = millis()+_profile.RoRFreq*1000;
    _curStates.RoRFlag = true;  
};

bool Roaster::start(RoastProfile profile){
    _profile = profile;    
    _timerStart = millis();
    _timerStop = 0;
    _timerRoR = 0;
    _plannedStop = _profile.PDT * 60000;                        //Duration (0 -> Stop) Min -> Ms 
    _plannedFC = _plannedStop * 0.01 * (100 - _profile.DTR);    //Duration (0 -> FC) in Ms

    //initRoRTimer();
    _curStates.FC = 0;
    _curStates.LeftTime = 0;
    _curStates.StopTime = 0;
    _curStates.StopFlag = false;
    _curStates.RoRFlag = false;
    _curStates.Stage = STARTED;    

    return true;    
};
bool Roaster::stop(){
    //readStates();
    Serial.println("4.Roaster::stop()");
    if(_curStates.Stage >= STARTED){
        if(_curStates.Stage >= ONFINISH)
            _curStates.Stage = STOPPED;
        else
            _curStates.Stage = ONFINISH;
    }
    return true;    
};
RoastStage Roaster::Stage(){
    return _curStates.Stage;    
};

uint8_t Roaster::getCoffeBeanTemperature(){
    return uint8_t(_thermo_z_cb.readCelsius());
};
RoasterStates *Roaster::readStates(){
    uint32_t _curMillis =  millis();
    if(_curStates.Stage >= STARTED) {
        _curStates.Time = _curMillis - _timerStart;
        if(_timerStop > 0) 
        {
            if(_curMillis < _timerStop)      
                _curStates.LeftTime = _timerStop - _curMillis;
            else{
                _timerStop = 0;
                _curStates.LeftTime = 0;
                _curStates.StopFlag = true;
            } 
        }   // Stop roast 
    }
    _curStates.BT = getCoffeBeanTemperature();
    _curMillis =  millis();
    if(_curMillis >= _timerRoR){
        if(_curStates.LastRoRBT!=0)
            _curStates.RoR = _curStates.LastRoRBT >= _curStates.BT ? _curStates.LastRoRBT - _curStates.BT :  _curStates.BT - _curStates.LastRoRBT;
        _curStates.LastRoRBT = _curStates.BT;
        initRoRTimer();
    }
    _curStates.PDT = _plannedStop;
    _curStates.PFC = _plannedFC;
    
 
    return &_curStates;
};
void Roaster::FC(uint8_t DevelopmentTimeRatio){
    uint32_t _curMillis;
    if(_curStates.Stage >= RoastStage::STARTED) {
        _curMillis = millis();
        _curStates.FC =  _curMillis - _timerStart;
        _curStates.FCT = getCoffeBeanTemperature();

        _curStates.FCE = _curStates.FCET = _curStates.SC = _curStates.SCT = 0;

        _timerStop = _curMillis + (_curStates.FC / (100-DevelopmentTimeRatio)) * DevelopmentTimeRatio;
        _curStates.StopTime = _timerStop - _timerStart;
        _curStates.StopFlag = false;

        _curStates.Stage = RoastStage::FC;
    }
};
void Roaster::FCE(){
    uint32_t _curMillis;
    if(_curStates.Stage >= RoastStage::FC) {
        _curMillis = millis();
        _curStates.FCE =  _curMillis - _timerStart;
        _curStates.FCET = getCoffeBeanTemperature();

        _curStates.SC = _curStates.SCT = 0;
        _curStates.Stage = RoastStage::FCEND;
    }
};
void Roaster::SC(){
    uint32_t _curMillis;
    if(_curStates.Stage >= RoastStage::FCEND) {
        _curMillis = millis();
        _curStates.SC =  _curMillis - _timerStart;
        _curStates.SCT = getCoffeBeanTemperature();

        _curStates.Stage = RoastStage::SC;
    }
};

void Roaster::RL(uint8_t valRL){
    _curStates.RL = valRL;
};

