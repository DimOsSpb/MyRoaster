#include <SoftwareSerial.h>
#include <Arduino.h>
#include <max6675.h>
#include <Time.h>
#include <Nextion.h>
#include <Roaster.h>
#include <Dispatcher.h>


#define CONTROL_READ_PERIOD 100




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

  // Refresh all states...
  dispatcher.refreshStates();
  



  //delay(300);
}