#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <max6675.h>
#include <Time.h>
#include <Nextion.h>
#include <Roaster.h>
#include <Dispatcher.h>

#define STATES_REFRESH_PERIOD 1000
#define CONTROL_READ_PERIOD 100

#define MSG_TYPE_HI_R 32323
#define MSG_TYPE_HI_H 23232U

Timer timerStatesRefresh(STATES_REFRESH_PERIOD);
Timer timerEvents(CONTROL_READ_PERIOD);

Dispatcher dispatcher;
  


void setup()
{

  dispatcher.init();

  // открываем последовательный порт для отладки
  Serial.begin(9600);
}

void loop()
{

  // Handling control events...
  dispatcher.listEvents();


  if (timerStatesRefresh.isReady())
  { // Refresh all states...
    dispatcher.refreshStates();
  }

  if (Serial.available() > 0)
  {

    //String inData = Serial.readStringUntil('\n'); 


    delay(40);
    
    //DynamicJsonBuffer jsonBuffer;
    // Allocate the JsonDocument in the heap
    StaticJsonDocument<280> doc;
    
    DeserializationError error = deserializeJson(doc, Serial);

    // // Test if parsing succeeds.
    if (!error){

      uint16_t _type = doc["type"];


      if(_type == MSG_TYPE_HI_R)
      {
          doc.clear();
          doc["type"] = MSG_TYPE_HI_H;
          doc["model"] = "v1";
          doc["id"] = "undef";
          serializeJson(doc,Serial);
      }
    }


  }


    

  //delay(300);
}