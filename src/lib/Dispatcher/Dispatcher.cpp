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
#define GRAPH_FREQUENCY 2500    //in MS - 2,5secs
#define CHART_DX 320 
#define CHART_DY 120 

#define CHART_BT_MAX 260
#define CHART_RoR_MAX 25


#define BTN_ST_ON_COMMAND "Start"
#define BTN_ST_OFF_COMMAND "Stop"
#define BTN_FC_COMMAND "FC"
#define BTN_DR_COMMAND "DR"
#define BTN_ST_ON_DOWN_TEXT "\r\n Start" 
#define BTN_ST_OFF_DOWN_TEXT "\r\n Stop"

#define DEFAULT_ROR_FREQ 3     //in secs
#define DEFAULT_DTR 22                  //Development Time Ratio (DTR) 15-25% (20-25% rec. https://www.scottrao.com/blog/2016/8/25/development-time-ratio )

const uint8_t DR_TEMPS[10]={196,205,210,219,225,230,240,245,245,250};
const String DR_N[10]={"Cinnamon ->196 °C","New England 196->205 °C","American 205->210°C","City 221 - 226 °C","Full City ->225 °C","Vienna 225->230 °C","French ->240 °C","Italian ->245","",""};

Dispatcher::Dispatcher(): _nextion(NEXTIAN_RX, NEXTIAN_TX){

};

void Dispatcher::init(){
    _profile.RoRFreq = DEFAULT_ROR_FREQ;
    _profile.DTR = DEFAULT_DTR;
    _nextion.init();
    // _nextion.sendCommand("cle 2,255");
    // _nextion.sendCommand("cls");
    // _nextion.sendCommand("b_st.val=0");
    _nextion.sendCommand("rest");
    _roaster.init();
    refreshStates();
};

void Dispatcher::refreshStates(){
    
    RoasterStates *_curRoasterStates = _roaster.readStates();
    DHMS time = getDHMS(_curRoasterStates->Time);
    DHMS timeFC = getDHMS(_curRoasterStates->FC);
    DHMS leftTime;
    String _btn_down_text;
    uint16_t x,_tmp_Xval,_tmp_BTval,_tmp_RoRval;

    if(_curRoasterStates->State == ROASTER_STATE_STARTED){

        //Chart
        if(_curRoasterStates->Time - _lastChartTim >= GRAPH_FREQUENCY){

            _tmp_BTval = CHART_DY-_curRoasterStates->BT*CHART_DY/CHART_BT_MAX;
            _tmp_RoRval = CHART_DY-_curRoasterStates->RoR*CHART_DY/CHART_RoR_MAX;

            if(_chartIndex)  _tmp_Xval =  _chartIndex - 1;
            else
            {
                _chartLastBT = _tmp_BTval;
                _chartLastRoR = _tmp_RoRval;

            }
            
            sprintf(_buf, "line %u,%u,%u,%u,GREEN",_tmp_Xval,_chartLastBT,_chartIndex,_tmp_BTval);
            _chartLastBT = _tmp_BTval;
            _nextion.sendCommand(_buf);

            if(_curRoasterStates->RoRFlag){
                sprintf(_buf, "line %u,%u,%u,%u,WHITE",_tmp_Xval,_chartLastRoR,_chartIndex,_tmp_RoRval);
                _nextion.sendCommand(_buf);

                _chartLastRoR = _tmp_RoRval;
                
                _curRoasterStates->RoRFlag = false;
            }

            _lastChartTim = _curRoasterStates->Time;
            _chartIndex++;

        }


        sprintf(_buf, "page0.b_fc.txt=\"%02u:%02u\r\n  FC\"",timeFC.Mins,timeFC.Secs);
        _nextion.sendCommand(_buf);

        if(_curRoasterStates->FC > 0){
            leftTime = getDHMS(_curRoasterStates->LeftTime);
            sprintf(_buf, "page0.t_left.txt=\"L %02u:%02u\"",leftTime.Mins,leftTime.Secs);
            _nextion.sendCommand(_buf);

            x = _curRoasterStates->StopTime/ GRAPH_FREQUENCY;
            Serial.print(x); 
            sprintf(_buf, "line %u,%u,%u,%u,2016",_chartFCIndex,0,_chartFCIndex,CHART_DY);
            _nextion.sendCommand(_buf); 
            sprintf(_buf, "line %u,%u,%u,%u,RED",x,0,x,CHART_DY);
            _nextion.sendCommand(_buf); 
        }
        else 
            sprintf(_buf, "page0.t_left.txt=\"L %02u:%02u\"",0,0);

        _btn_down_text = BTN_ST_OFF_DOWN_TEXT;
    }
    else
        _btn_down_text = BTN_ST_ON_DOWN_TEXT;



    sprintf(_buf, "page0.t_bt.txt=\"BT:%u\"",_curRoasterStates->BT);
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "page0.t_dr.txt=\"DR:%u\"",_curRoasterStates->DR);
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "page0.t_max.txt=\"Max:%uC\"",DR_TEMPS[_curRoasterStates->DR-1]);
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "page0.t_drn.txt=\"%s\"",DR_N[_curRoasterStates->DR-1].c_str());
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "page0.b_st.txt=\"%02u:%02u%s\"",time.Mins,time.Secs, _btn_down_text.c_str());
    _nextion.sendCommand(_buf);
    sprintf(_buf, "_st_t.txt=\"%02u:%02u\"",time.Mins,time.Secs);
    _nextion.sendCommand(_buf);  



    // Serial.print("Time: "); Serial.print(time.Mins);  Serial.print(":");  Serial.print(time.Secs);
    // Serial.print(" BT: ");
    // Serial.print(_curRoasterStates->BT);
    // Serial.print(" ChartBT: ");
    // Serial.print(CHART_DY-_curRoasterStates->BT*CHART_DY/CHART_BT_MAX);
    // Serial.print(" RoR: ");
    // Serial.print(_curRoasterStates->RoR);
    // Serial.print(" ChartBT: ");
    // Serial.println(CHART_DY-_curRoasterStates->RoR*CHART_DY/CHART_RoR_MAX);
    // Serial.print(" FC: ");
    // Serial.println(_curRoasterStates->FC);
     
};

void Dispatcher::listEvents(){
    String input = _nextion.readInput();
    //Serial.print("input :");
    //Serial.println(input);
    for(unsigned int i=0; i<input.length(); i++){
        if(memcmp(&input[i],BTN_ST_ON_COMMAND ,sizeof(BTN_ST_ON_COMMAND)-1)==0){
            i+=sizeof(BTN_ST_ON_COMMAND)-2;
            _nextion.sendCommand("cle 2,255");            
            _roaster.start(_profile);
            //refreshStates();
        }
        else if(memcmp(&input[i],BTN_ST_OFF_COMMAND ,sizeof(BTN_ST_OFF_COMMAND)-1)==0){
            i+=sizeof(BTN_ST_OFF_COMMAND)-2;
            _roaster.stop();
            //refreshStates();
        }
        else if(memcmp(&input[i],BTN_FC_COMMAND ,sizeof(BTN_FC_COMMAND)-1)==0){
            i+=sizeof(BTN_FC_COMMAND)-2;
            _roaster.FC(DEFAULT_DTR);
            _chartFCIndex=_chartIndex;
            //refreshStates();
        }
        else if(memcmp(&input[i],BTN_DR_COMMAND ,sizeof(BTN_DR_COMMAND)-1)==0){
            
            _roaster.DR(input[i+sizeof(BTN_DR_COMMAND)-1]);
            i+=sizeof(BTN_DR_COMMAND+2);
            //refreshStates();
        }        
    }
}





