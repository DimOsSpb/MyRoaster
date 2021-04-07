#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Dispatcher.h>
#include <Roaster.h>
#include <Time.h>
#include <Chart.h>

// RX - digital output 9, connect on Nextian TX 
// TX - digital output 10, connect on Nextian RX
// Nextian display pins config


const uint8_t RL_TEMPS[10] ={196,205,210,219,224,225,234,239,243,246};
const String RL_GROUPE[4] ={"Light","Medium","Medium-dark","Dark"};
const RoastLevels _RL[10] ={
    {0,"Cinnamon"},
    {0,"New England"},
    {1,"American"},
    {1,"City"},
    {1,"City+"},
    {2,"Full City"},
    {2,"Full City+"},
    {3,"Vienna"},
    {3,"French"},
    {3,"Italian"}
};

Dispatcher::Dispatcher(): _nextion(NEXTIAN_RX, NEXTIAN_TX), _chart(&_nextion,0,25,CHART_DX,CHART_DY+25){};

void Dispatcher::init(){
    _profile.RoRFreq = DEFAULT_ROR_FREQ;
    _nextion.init();

    _nextion.sendCommand("rest");
    _roaster.init();

    _initChart();
    
    _chartIndex = 0;
    changeRoastLevel(DEFAULT_RL,false);
    changeRoastTime(DEFAULT_PDT,false);
    refreshStates();

};

void Dispatcher::_initChart(){
    _chart.init();
    _chart.initChanel(0,0,_profile.PDT*CHART_X_MAX_RATIO,50,CHART_BT_MAX,CHART_BT_COLOR,2);
    _chart.initChanel(1,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_ROR_MAX,CHART_ROR_COLOR,2);
    _chart.initChanel(2,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_BT_MAX,CHART_MAX_BT_COLOR,2);
    _chart.initChanel(3,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_DX,CHART_FC_COLOR,1);
    _chart.initChanel(4,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_DX,CHART_MAX_BT_COLOR,1);
    _chart.initChanel(5,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_DX,CHART_PDT_COLOR,3);
    _chart.initChanel(6,0,_profile.PDT*CHART_X_MAX_RATIO,0,CHART_DX,CHART_PFC_COLOR,1);
}

void Dispatcher::startRoast(){
    sprintf(_buf, "ref page0");  //cls
    _nextion.sendCommand(_buf);    
    _initChart();
    _chartIndex = 0;
    _roaster.start(_profile);
}
void Dispatcher::stopRoast(){
    sprintf(_buf, "page0.b_st.bco2=19188");
    _nextion.sendCommand(_buf);    
    _roaster.stop();
}


void Dispatcher::startFirstCrack(){
    _roaster.FC(DEFAULT_DTR);
    _chartFCIndex=_chartIndex;
}

void Dispatcher::stopFirstCrack(){
}

void Dispatcher::startSecondCrack(){
}

void Dispatcher::stopSecondCrack(){
}

void Dispatcher::changeRoastLevel(uint8_t level, bool _reflect = true){
    if(level > 0 && level < 11){
        _profile.RL = level;
        _roaster.RL(level);
        if(_reflect) _reflectChanges_RL();
    }
}

void Dispatcher::changeRoastTime(uint8_t value, bool _reflect = true){
    if(value > 0 && value < DEFAULT_DTR){
        _profile.PDT = value;
        if(_reflect) _reflectChanges_PDT();
    }
}

void Dispatcher::_reflectChanges_RL(){
    uint8_t _pr_i = _profile.RL-1;
    sprintf(_buf, "t_rl.txt=\"L%u %s/%s\"",_profile.RL, RL_GROUPE[_RL[_pr_i].GroupeIndex].c_str(), _RL[_pr_i].Name.c_str());
    _nextion.sendCommand(_buf); 
    sprintf(_buf, "t_max.txt=\"%u\"",RL_TEMPS[_pr_i]);
    _nextion.sendCommand(_buf);
}

void Dispatcher::_reflectChanges_PDT(){
    sprintf(_buf, "t_rt.txt=\"%u\"",_profile.PDT);
    _nextion.sendCommand(_buf); 
}

void Dispatcher::_onNextionPage(uint8_t page){
    _nextion.onPage(page);
    _reflectChanges_RL();
    _reflectChanges_PDT();
}
void Dispatcher::refreshStates(){
    
    RoasterStates *_curRoasterStates = _roaster.readStates();
    DHMS time = getDHMS(_curRoasterStates->Time);
    DHMS timeFC = getDHMS(_curRoasterStates->FC);
    DHMS leftTime;
    String _btn_down_text;
    uint16_t x;


    if( _nextion.currentPage()==0){
            
        if(_curRoasterStates->State == ROASTER_STATE_STARTED){



            //Chart-------------------------------------
            if(_chart.fieldIsOver()){
                stopRoast();  
            }
            if(_curRoasterStates->Time - _lastChartTim >= GRAPH_FREQUENCY){

                _chart.clearRight();
                _chart.lineG(2,RL_TEMPS[_profile.RL-1]);
                _chart.addChanelValue(0,_curRoasterStates->BT);            
                _chart.chanelForecast(0);
                _chart.addChanelValue(1,_curRoasterStates->RoR);
                //_chart.lineV(5,_chartFCIndex);
                //_chart.lineV(6,_chartFCIndex);

                    

                _lastChartTim = _curRoasterStates->Time;
                _chartIndex++;

                if(_curRoasterStates->FC > 0){
                    x = _curRoasterStates->StopTime / (_curRoasterStates->FC / _chartFCIndex);
                    _chart.lineV(3,_chartFCIndex);
                    _chart.lineV(4, x);

                }
            }
    
            //chart---------------------------------

            sprintf(_buf, "page0.b_fc.txt=\"%02u:%02u\r\n  FC\"",timeFC.Mins,timeFC.Secs);
            _nextion.sendCommand(_buf);

            if(_curRoasterStates->FC > 0){
                if(_curRoasterStates->StopFlag){
                    sprintf(_buf, "page0.b_st.pco2=%s", _chartIndex & 1 ? "RED" : "61277");
                    _nextion.sendCommand(_buf);                
                }


            }
            else 
                sprintf(_buf, "page0.t_left.txt=\"%02u:%02u\"",0,0);

            _btn_down_text = BTN_ST_OFF_DOWN_TEXT;
        }
        else
            _btn_down_text = BTN_ST_ON_DOWN_TEXT;

        sprintf(_buf, "page0.b_st.txt=\"%02u:%02u%s\"",time.Mins,time.Secs, _btn_down_text.c_str());
        _nextion.sendCommand(_buf);

        sprintf(_buf, "page0.t_ror.txt=\"%u\"",_curRoasterStates->RoR);
        _nextion.sendCommand(_buf); 
        sprintf(_buf, "page0.t_bt.txt=\"%u\"",_curRoasterStates->BT);
        _nextion.sendCommand(_buf);


        leftTime = getDHMS(_curRoasterStates->LeftTime);
        sprintf(_buf, "page0.t_left.txt=\"%02u:%02u\"",leftTime.Mins,leftTime.Secs);
        _nextion.sendCommand(_buf);         

    }

     
};



void Dispatcher::listEvents(){
    byte input[10];
    if(_nextion.readInput(9,&input[0]) > 0){

        switch ((int)input[0])
        {
            case ON_PAGE_COMMAND:
                //Serial.print("onPage="); Serial.println(input[1]);
                _onNextionPage(input[1]);
                break;
            case BTN_ST_ON_COMMAND:
                //Serial.println("Start");
                startRoast();
                break;
            case BTN_ST_OFF_COMMAND:
                //Serial.println("Stop");
                stopRoast();
                break;
            case BTN_FC_ON_COMMAND:
                //Serial.println("FC");
                startFirstCrack();
                break;
            case BTN_FC_OFF_COMMAND:
                //Serial.println("SFC");
                stopFirstCrack();
                break;
            case RL_COMMAND:
                //Serial.print("RL="); Serial.println(input[1]);
                changeRoastLevel(input[1]);
                break;                                
            case RT_COMMAND:
                //Serial.print("RL="); Serial.println(input[1]);
                changeRoastTime(input[1]);
             
                break;                                
            default:
                break;
        }
    }


}





