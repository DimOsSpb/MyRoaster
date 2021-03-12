#include <SoftwareSerial.h>
#include <Arduino.h>
#include <max6675.h>
#include <Time.h>
#include <Nextion.h>
#include <Roaster.h>
#include <Dispatcher.h>

#define STATESREAD_PERIOD 3000
#define CONTROLREAD_PERIOD 1000
Timer timerRosterStates(STATESREAD_PERIOD);
//Timer timerСontrolEvents(CONTROLREAD_PERIOD);


Roaster roaster;
Dispatcher dispatcher;





void setup() {

  
  dispatcher.init(&roaster);
  
  // открываем последовательный порт для отладки
  Serial.begin(9600);

}

void loop() {

    // if (timerСontrolEvents.isReady()) {           // Handling control events... 


    // }

    if (timerRosterStates.isReady()) {           // Read roster states... 

      dispatcher.updateStates();


      // Serial.println(TZ1);
    }
    
  //delay(300);
}