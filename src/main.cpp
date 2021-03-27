#include <SoftwareSerial.h>
#include <Arduino.h>
#include <max6675.h>
#include <Time.h>
#include <Nextion.h>
#include <Roaster.h>
#include <Dispatcher.h>

#define STATES_REFRESH_PERIOD 500
#define CONTROL_READ_PERIOD 100

Timer timerStatesRefresh(STATES_REFRESH_PERIOD);
Timer timerEvents(CONTROL_READ_PERIOD);

Dispatcher dispatcher;
//Serial.print("forecastVal=");Serial.print(forecastVal);Serial.print(" OverVal=");Serial.print(overVal);Serial.print(" x1=");Serial.print(x1);Serial.print(" y1=");Serial.print(y1);Serial.print(" x2=");Serial.print(x2);Serial.print(" y2=");Serial.println(y2);

void setup() {

  dispatcher.init();
  
  // открываем последовательный порт для отладки
  Serial.begin(9600);

}

void loop() {


    //if (timerEvents.isReady()) {           // Handling control events... 
      dispatcher.listEvents();
    //}

    if (timerStatesRefresh.isReady()) {             // Refresh all states... 
      dispatcher.refreshStates();
    }
    
  //delay(300);
}