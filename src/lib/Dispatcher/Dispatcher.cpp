#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Dispatcher.h>
#include <Roaster.h>
#include <Time.h>
#include <Chart.h>
#include <parser.h>

// RX - digital output 9, connect on Nextian TX
// TX - digital output 10, connect on Nextian RX
// Nextian display pins config

const char RL_Light[] PROGMEM = "Light";
const char RL_Medium[] PROGMEM = "Medium";
const char RL_Medium_dark[] PROGMEM = "Medium-dark";
const char RL_Dark[] PROGMEM = "Dark";
// const char *const RL_GROUPE[4] PROGMEM = {RL_Light, RL_Medium, RL_Medium_dark, RL_Dark};

// const String RL_GROUPE[4] ={"Light","Medium","Medium-dark","Dark"};

const char RLN_Cinnamon[] PROGMEM = "Cinnamon";
const char RLN_New_England[] PROGMEM = "New England";
const char RLN_American[] PROGMEM = "American";
const char RLN_City[] PROGMEM = "City";
const char RLN_CityP[] PROGMEM = "City+";
const char RLN_Full_City[] PROGMEM = "Full City";
const char RLN_Full_CityP[] PROGMEM = "Full City+";
const char RLN_Vienna[] PROGMEM = "Vienna";
const char RLN_French[] PROGMEM = "French";
const char RLN_Italian[] PROGMEM = "Italian";
const uint8_t RL_TEMPS[10] = {196, 205, 210, 219, 224, 225, 234, 239, 243, 246};
const char *const _RL[] PROGMEM = {RL_Light, RL_Light, RL_Medium, RL_Medium, RL_Medium, RL_Medium_dark, RL_Medium_dark, RL_Dark, RL_Dark, RL_Dark};
const char *const _RLN[] PROGMEM = {RLN_Cinnamon, RLN_New_England, RLN_American, RLN_City, RLN_CityP, RLN_Full_City, RLN_Full_CityP, RLN_Vienna, RLN_French, RLN_Italian};
// const RoastLevels _RL[10] ={
//     {0,"Cinnamon"},
//     {0,"New England"},
//     {1,"American"},
//     {1,"City"},
//     {1,"City+"},
//     {2,"Full City"},
//     {2,"Full City+"},
//     {3,"Vienna"},
//     {3,"French"},
//     {3,"Italian"}
// };

Dispatcher::Dispatcher() : _statesRefreshPeriod(STATES_REFRESH_PERIOD),
                           _statesForHubRefreshPeriod(0),
                           _nextion(NEXTIAN_RX, NEXTIAN_TX),
                           _chart(&_nextion, CHART_X, CHART_Y, CHART_DX, CHART_DY + CHART_Y){};

void Dispatcher::init()
{

    _profile.RoRFreq = DEFAULT_ROR_FREQ;
    _nextion.init();

    _nextion.sendCommand(("rest"));
    _roaster.init();

    changeRoastLevel(DEFAULT_RL, false);
    changeRoastTime(DEFAULT_PDT, false);
    changeDTR(DEFAULT_DTR, false);

    sprintf_P(_buf, PSTR("page0.b_fc.pco=%u"), CHART_FC_COLOR);
    _nextion.sendCommand(_buf);

    sprintf_P(_buf, PSTR("page0.b_fc.pco2=%u"), CHART_FC_COLOR);
    _nextion.sendCommand(_buf);

    _initChart();
    refreshStates();
};

void Dispatcher::changeForHubStatesRefresh(uint32_t period)
{
    _statesForHubRefreshPeriod.start(period);
};
void Dispatcher::_sendStatesToHub(RoasterStates *curRoasterStates){

    // _doc.clear();
    // _doc[F("type")] = MSG_TYPE_STATES;
    // _doc[F("json")][F("ms")] = curRoasterStates->Time;
    // _doc[F("json")][F("bt")] = curRoasterStates->BT;

    // serializeJson(_doc, Serial);
};

void Dispatcher::_initChart()
{
    uint32_t pdt_plus = (_profile.PDT + _profile.PDT * 0.01 * CHART_X_ADD_PROCENT) * CHART_X_MAX_RATIO;

    _chart.init(0, pdt_plus);
    _chart.initChanel(0, CHART_BT_MIN, CHART_BT_MAX, CHART_BT_COLOR, 1);
    _chart.initChanel(1, CHART_ROR_MIN, CHART_ROR_MAX, CHART_ROR_COLOR, 1);
    _chart.initChanel(2, CHART_BT_MIN, CHART_BT_MAX, CHART_MAX_BT_COLOR, 1);
    _chart.initChanel(3, CHART_DX, CHART_FC_COLOR, 1);
    _chart.initChanel(4, CHART_DX, CHART_MAX_BT_COLOR, 1);
    _chart.initChanel(5, 0, CHART_DX, CHART_PDT_COLOR, 1);
    _chart.initChanel(6, 0, CHART_DX, CHART_PFC_COLOR, 1);
}

void Dispatcher::startRoast()
{
    sprintf_P(_buf, PSTR("ref page0")); // cls
    _nextion.sendCommand(_buf);
    _initChart();
    _roaster.start(_profile);
}
void Dispatcher::stopRoast()
{
    if (_roaster.isStarted())
    {
        sprintf_P(_buf, PSTR("page0.b_st.pco2=61277"));
        _nextion.sendCommand(_buf);
        sprintf_P(_buf, PSTR("page0.b_st.val=0"));
        _nextion.sendCommand(_buf);
        _roaster.stop();
    }
}

void Dispatcher::startFirstCrack()
{
    _roaster.FC(DEFAULT_DTR);
}

void Dispatcher::stopFirstCrack()
{
}

void Dispatcher::startSecondCrack()
{
}

void Dispatcher::stopSecondCrack()
{
}

void Dispatcher::changeRoastLevel(uint8_t level, bool _reflect = true)
{
    if (level > 0 && level < 11)
    {
        _profile.RL = level;
        _roaster.RL(level);
        if (_reflect)
            _reflectChanges_RL();
    }
}

void Dispatcher::changeRoastTime(uint8_t value, bool _reflect = true)
{
    if (value > 0 && value <= MAX_PDT)
    {
        _profile.PDT = value;
        if (_reflect)
            _reflectChanges_PDT();
    }
}
void Dispatcher::changeDTR(uint8_t value, bool _reflect = true)
{
    if (value > 0 && value <= MAX_DTR)
    {
        _profile.DTR = value;
        if (_reflect)
            _reflectChanges_DTR();
    }
}

void Dispatcher::_reflectChanges_RL()
{
    uint8_t _pr_i = _profile.RL - 1;

    // strcpy_P(_buf15_1, (char *)pgm_read_word(&RL_GROUPE[(_RL[_pr_i].GroupeIndex)]));
    strcpy_P(_buf15_1, (char *)pgm_read_word(&_RL[_pr_i]));
    strcpy_P(_buf15_2, (char *)pgm_read_word(&_RLN[_pr_i]));
    sprintf_P(_buf, PSTR("t_rl.txt=\"L%u %s/%s\""), _profile.RL, _buf15_1, _buf15_2);
    // sprintf(_buf, "t_rl.txt=\"L%u %s/%s\"",_profile.RL, RL_GROUPE[_RL[_pr_i].GroupeIndex].c_str(), _RL[_pr_i].Name.c_str());
    _nextion.sendCommand(_buf);
    sprintf_P(_buf, PSTR("t_max.txt=\"%u\""), RL_TEMPS[_pr_i]);
    _nextion.sendCommand(_buf);
}

void Dispatcher::_reflectChanges_PDT()
{
    sprintf_P(_buf, PSTR("t_rt.txt=\"%u\""), _profile.PDT);
    _nextion.sendCommand(_buf);
}

void Dispatcher::_reflectChanges_DTR()
{
    sprintf_P(_buf, PSTR("t_rr.txt=\"%u\""), _profile.DTR);
    _nextion.sendCommand(_buf);
}

void Dispatcher::_onNextionPage(uint8_t page)
{
    _nextion.onPage(page);
    _reflectChanges_RL();
    _reflectChanges_PDT();
}
void Dispatcher::refreshStates()
{
    bool _refresh = _statesRefreshPeriod.isReady();
    bool _refreshForHub = _statesForHubRefreshPeriod.isReady();
    RoasterStates *_curRoasterStates;

    if (_refresh | _refreshForHub)
        // Serial.println("if (_refresh | _refreshForHub)");
        _curRoasterStates = _roaster.readStates();

    if (_refreshForHub)
    {

        // Serial.println("if (_refreshForHub)");
        _sendStatesToHub(_curRoasterStates);
    }
    if (_refresh)
    {

        // Serial.println("if (_refresh)");
        DHMS time = getDHMS(_curRoasterStates->Time);
        DHMS timeFC = getDHMS(_curRoasterStates->FC);
        DHMS leftTime, _pt;
        String _btn_down_text;

        if (_nextion.currentPage() == 0)
        {

            if (_curRoasterStates->State == ROASTER_STATE_STARTED)
            {

                // Chart-------------------------------------
                if (_chart.fieldIsOver())
                {
                    stopRoast();
                }
                if (_curRoasterStates->Time - _lastChartTime >= GRAPH_FREQUENCY)
                {

                    _chart.clearRight();
                    _chart.lineG(2, RL_TEMPS[_profile.RL - 1]);

                    _chart.addChanelValue(0, _curRoasterStates->Time, _curRoasterStates->BT);
                    _chart.chanelForecast(0, CHART_BT_FCAST_COLOR);
                    _chart.addChanelValue(1, _curRoasterStates->Time, _curRoasterStates->RoR);

                    _pt = getDHMS(_curRoasterStates->PDT);
                    sprintf_P(_buf, PSTR("%02u:%02u"), _pt.Mins, _pt.Secs);

                    _chart.lineV(_curRoasterStates->PDT, CHART_VLSTYLE_PFINISH, CHART_PDT_COLOR, 0, _buf); // Planned Finish

                    _pt = getDHMS(_curRoasterStates->PFC);
                    sprintf_P(_buf, PSTR("%02u:%02u"), _pt.Mins, _pt.Secs);

                    _chart.lineV(_curRoasterStates->PFC, CHART_VLSTYLE_PFC, CHART_PFC_COLOR, 0, _buf); // Planned FC

                    _lastChartTime = _curRoasterStates->Time;

                    if (_curRoasterStates->FC > 0)
                    {

                        sprintf_P(_buf, PSTR("%02u:%02u"), timeFC.Mins, timeFC.Secs);
                        _chart.lineV(_curRoasterStates->FC, CHART_VLSTYLE_FC, CHART_FC_COLOR, 0, _buf);

                        sprintf_P(_buf, PSTR("FC %u°C"), _curRoasterStates->FCT);
                        _chart.label(0, _curRoasterStates->FC, _curRoasterStates->FCT, _buf, CHART_LSTYLE_FC, CHART_FC_COLOR);

                        _pt = getDHMS(_curRoasterStates->StopTime);
                        sprintf_P(_buf, PSTR("%02u:%02u"), _pt.Mins, _pt.Secs);
                        _chart.lineV(_curRoasterStates->StopTime, CHART_VLSTYLE_FINISH, CHART_FINISH_COLOR, 0, _buf);
                    }
                }

                // chart---------------------------------

                sprintf_P(_buf, PSTR("page0.b_fc.txt=\"%02u:%02u\r\n  FC\""), timeFC.Mins, timeFC.Secs);
                _nextion.sendCommand(_buf);

                if (_curRoasterStates->FC > 0)
                {
                    if (_curRoasterStates->StopFlag)
                    {
                        sprintf_P(_buf, PSTR("page0.b_st.pco2=%s"), _refreshCounter & 1 ? F("RED") : F("61277"));
                        _nextion.sendCommand(_buf);
                    }
                }
                else
                    sprintf_P(_buf, PSTR("page0.t_left.txt=\"%02u:%02u\""), 0, 0);

                _btn_down_text = BTN_ST_OFF_DOWN_TEXT;
            }
            else
                _btn_down_text = BTN_ST_ON_DOWN_TEXT;

            sprintf_P(_buf, PSTR("page0.b_st.txt=\"%02u:%02u%s\""), time.Mins, time.Secs, _btn_down_text.c_str());
            _nextion.sendCommand(_buf);

            sprintf_P(_buf, PSTR("page0.t_ror.txt=\"%u\""), _curRoasterStates->RoR);
            _nextion.sendCommand(_buf);
            sprintf_P(_buf, PSTR("page0.t_bt.txt=\"%u\""), _curRoasterStates->BT);
            _nextion.sendCommand(_buf);

            leftTime = getDHMS(_curRoasterStates->LeftTime);
            sprintf_P(_buf, PSTR("page0.t_left.txt=\"%02u:%02u\""), leftTime.Mins, leftTime.Secs);
            _nextion.sendCommand(_buf);
        }
        _refreshCounter++;
    }
};

void Dispatcher::listEvents()
{
    // From HUB
    if (Serial.available() > 0)
    {

        String inData = Serial.readStringUntil('\n');

        //{"type":32323,"json":{"model":"RHUB","id":"cd881408-179d-23c2-7237-546e394f6e9a","timestatusupdate":1000}}

        if (inData.length() > 0)
        {
            Parser p = Parser(&inData);
            uint32_t _type = p.getInt("type");

            String _id, _json;
            uint32_t _refreshPeriod;

            Serial.println(_type);
            switch (_type)
            {
                case MSG_TYPE_HI_R:
                    Serial.println("MSG_TYPE_HI_R");

                    _id = p.getString("id");
                    _refreshPeriod = p.getInt("timestatusupdate");

                    Serial.flush();
                    sprintf_P(_buf, PSTR("{\"type\":%u,\"json\":{\"model\":\"%s\",\"id\":\"%s\"}}"), MSG_TYPE_HI_H, "v1", _id);
                    Serial.println(_buf);
                    changeForHubStatesRefresh(_refreshPeriod);

                    break;
                case BTN_ST_ON_COMMAND:
                    Serial.println("BTN_ST_ON_COMMAND");
                    break;
                case BTN_ST_OFF_COMMAND:
                    Serial.println("BTN_ST_OFF_COMMAND");
                    break;
                default:
                    break;
            }

        }
    }

    // From Nexion
    byte input[10];
    if (_nextion.readInput(9, &input[0]) > 0)
    {

        switch ((int)input[0])
        {
        case ON_PAGE_COMMAND:
            _onNextionPage(input[1]);
            break;
        case BTN_ST_ON_COMMAND:
            startRoast();
            break;
        case BTN_ST_OFF_COMMAND:
            stopRoast();
            break;
        case BTN_FC_ON_COMMAND:
            startFirstCrack();
            break;
        case BTN_FC_OFF_COMMAND:
            stopFirstCrack();
            break;
        case RL_COMMAND:
            changeRoastLevel(input[1]);
            break;
        case RT_COMMAND:
            changeRoastTime(input[1]);
            break;
        case TR_COMMAND:
            changeDTR(input[1]);
            break;
        default:
            break;
        }
    }
}
