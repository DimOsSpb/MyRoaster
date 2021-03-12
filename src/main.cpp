#include <SoftwareSerial.h>
#include <Arduino.h>
#include <max6675.h>
#include <Time.h>
#include <Nextion.h>
#include <Roaster.h>
#include <Dispatcher.h>

#define STATESREFRESH_PERIOD 3000
#define CONTROLREAD_PERIOD 1000

Timer timerStatesRefresh(STATESREFRESH_PERIOD);
Timer timerEvents(CONTROLREAD_PERIOD);

Dispatcher dispatcher;

void setup() {
  
  dispatcher.init();
  
  // открываем последовательный порт для отладки
  Serial.begin(9600);

}

void loop() {

    if (timerEvents.isReady()) {           // Handling control events... 
      dispatcher.listEvents();
    }

    if (timerStatesRefresh.isReady()) {             // Refresh all states... 
      dispatcher.refreshStates();
    }
    
  //delay(300);
}