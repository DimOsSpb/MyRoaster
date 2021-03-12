#include <Roaster.h>

Roaster::Roaster(): _thermo_z_cb(THERMO_Z_CB_CLK, THERMO_Z_CB_CS, THERMO_Z_CB_DO){};
void Roaster::init(){
    _state = ROASTER_STATE_STOPPED;    
};
bool Roaster::start(){
    _timerStart = millis();
    _profile.dataIndex = -1;
    _state = ROASTER_STATE_STARTED;
    return true;    
};
uint8_t Roaster::getCoffeBeanTemperature(){
    return uint8_t(_thermo_z_cb.readCelsius());
};
RoastProfile *Roaster::readStates(){
    
    //if(_profile.dataIndex !=0 && _profile.dataIndex < CHART_DATA_SIZE) _profile.dataIndex++;
    _profile.dataIndex++;
    _profile.chartData[_profile.dataIndex].TP = getCoffeBeanTemperature();
    if(_profile.dataIndex ==0) _profile.chartData[_profile.dataIndex].RoR = _profile.chartData[_profile.dataIndex].TP;
    else
    {
        _profile.chartData[_profile.dataIndex].RoR = (_profile.chartData[_profile.dataIndex].TP - _profile.chartData[_profile.dataIndex-1].TP)+10;
    }
    
    return &_profile;
};