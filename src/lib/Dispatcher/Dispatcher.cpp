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

    
    changeRoastLevel(DEFAULT_RL,false);
    changeRoastTime(DEFAULT_PDT,false);
    changeDTR(DEFAULT_DTR,false);

    sprintf(_buf, "page0.b_fc.pco=%u",CHART_FC_COLOR);
    _nextion.sendCommand(_buf);
//Serial.println(_buf); 
    sprintf(_buf, "page0.b_fc.pco2=%u",CHART_FC_COLOR);
    _nextion.sendCommand(_buf); 

    _initChart();
    refreshStates();

};

void Dispatcher::_initChart(){
    uint32_t pdt_plus = (_profile.PDT+_profile.PDT*0.01*CHART_X_ADD_PROCENT) * CHART_X_MAX_RATIO;

    _chart.init();
    _chart.initChanel(0,0,pdt_plus,50,CHART_BT_MAX,CHART_BT_COLOR,2);
    _chart.initChanel(1,0,pdt_plus,0,CHART_ROR_MAX,CHART_ROR_COLOR,2);
    _chart.initChanel(2,0,pdt_plus,0,CHART_BT_MAX,CHART_MAX_BT_COLOR,2);
    _chart.initChanel(3,0,pdt_plus,0,CHART_DX,CHART_FC_COLOR,1);
    _chart.initChanel(4,0,pdt_plus,0,CHART_DX,CHART_MAX_BT_COLOR,1);
    _chart.initChanel(5,0,pdt_plus,0,CHART_DX,CHART_PDT_COLOR,3);
    _chart.initChanel(6,0,pdt_plus,0,CHART_DX,CHART_PFC_COLOR,1);
}

void Dispatcher::startRoast(){
    sprintf(_buf, "ref page0");  //cls
    _nextion.sendCommand(_buf);    
    _initChart();
    _roaster.start(_profile);
}
void Dispatcher::stopRoast(){
    if(_roaster.isStarted()){
        sprintf(_buf, "page0.b_st.pco2=61277");
        _nextion.sendCommand(_buf);    
        sprintf(_buf, "page0.b_st.val=0");
        _nextion.sendCommand(_buf);    
        _roaster.stop();
    }
}


void Dispatcher::startFirstCrack(){
    _roaster.FC(DEFAULT_DTR);
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
    if(value > 0 && value <= MAX_PDT){
        _profile.PDT = value;
        if(_reflect) _reflectChanges_PDT();
    }
}
void Dispatcher::changeDTR(uint8_t value, bool _reflect = true){
    if(value > 0 && value <= MAX_DTR){
        _profile.DTR = value;
        if(_reflect) _reflectChanges_DTR();
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

void Dispatcher::_reflectChanges_DTR(){
    sprintf(_buf, "t_rr.txt=\"%u\"",_profile.DTR);
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
    DHMS leftTime,_pt;
    String _btn_down_text;
    char PFC_Flag[] = "PFC";    
    char PDT_Flag[] = "PStop";    
    char FC_Flag[] = "FC";    
    char FIN_Flag[] = "Finish";


    if( _nextion.currentPage()==0){
            
        if(_curRoasterStates->State == ROASTER_STATE_STARTED){

            //Chart-------------------------------------
            if(_chart.fieldIsOver()){
                stopRoast();  
            }
            if(_curRoasterStates->Time - _lastChartTime >= GRAPH_FREQUENCY){

                _chart.clearRight();
                _chart.lineG(2,RL_TEMPS[_profile.RL-1]);



                _chart.addChanelValue(0, _curRoasterStates->Time, _curRoasterStates->BT);            
                _chart.chanelForecast(0, CHART_BT_FCAST_COLOR);
                _chart.addChanelValue(1, _curRoasterStates->Time, _curRoasterStates->RoR);

                _pt = getDHMS(_curRoasterStates->PDT);    
                sprintf(_buf, "%02u:%02u", _pt.Mins, _pt.Secs);

                _chart.lineV(5,_curRoasterStates->PDT, _buf, PDT_Flag, CHART_LSTYLE_PFINISH);         // Planned Finish 

                _pt = getDHMS(_curRoasterStates->PFC);    
                sprintf(_buf, "%02u:%02u", _pt.Mins, _pt.Secs);

                _chart.lineV(6,_curRoasterStates->PFC, _buf, PFC_Flag, CHART_LSTYLE_PFC);         // Planned FC

                    

                _lastChartTime = _curRoasterStates->Time;

                if(_curRoasterStates->FC > 0){


                    sprintf(_buf, "%02u:%02u", timeFC.Mins,timeFC.Secs);
                    _chart.lineV(3,_curRoasterStates->FC, _buf, FC_Flag, CHART_LSTYLE_FC, 0);
                    _pt = getDHMS(_curRoasterStates->StopTime);
                    sprintf(_buf, "%02u:%02u", _pt.Mins, _pt.Secs);
                    _chart.lineV(4,_curRoasterStates->StopTime, _buf, FIN_Flag, CHART_LSTYLE_FINISH);

                }
            }
    
            //chart---------------------------------

            sprintf(_buf, "page0.b_fc.txt=\"%02u:%02u\r\n  FC\"",timeFC.Mins,timeFC.Secs);
            _nextion.sendCommand(_buf);

            if(_curRoasterStates->FC > 0){
                if(_curRoasterStates->StopFlag){
                    sprintf(_buf, "page0.b_st.pco2=%s", _refreshCounter & 1 ? "RED" : "61277");
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
    _refreshCounter++;
     
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
            case TR_COMMAND:
                //Serial.print("RL="); Serial.println(input[1]);
                changeDTR(input[1]);
                break;                                
            default:
                break;
        }
    }


}





