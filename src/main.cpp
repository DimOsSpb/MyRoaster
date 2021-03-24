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

void setup() {
  Serial1.println("Hello");
  dispatcher.init();
  
  // открываем последовательный порт для отладки
  Serial.begin(9600);
  //Serial.println("In setup");
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