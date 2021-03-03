#include <SoftwareSerial.h>
#include <Arduino.h>
#include <max6675.h>

//max6675 connection
#define THERMO_DO     4
#define THERMO_CS     5
#define THERMO_CLK    6


MAX6675 thermocouple(THERMO_DO, THERMO_CS, THERMO_CLK);

// создаём объект mySerial и передаём номера управляющих пинов RX и TX
// RX - цифровой вывод 8, необходимо соединить с выводом TX дисплея
// TX - цифровой вывод 9, необходимо соединить с выводом RX дисплея
SoftwareSerial Nextion(8, 9);


// функция отправки конца команды
// команда поступающая в дисплей должна кончаться символами «0xFF0xFF0xFF»
void comandEnd() {
  for (int i = 0; i < 3; i++) {
    Nextion.write(0xff);
  }
}

void setup() {
   // открываем последовательный порт
   Nextion.begin(9600);
  //  Nextion.print("baud=115200");
  //  comandEnd();
  //  Nextion.begin(115200);
  // thermocouple.begin(9600);
   //pinMode(LED_BUILTIN, OUTPUT);
}
void sendCommand(const char* cmd)
{
    while (Nextion.available())
    {
        Nextion.read();
    }
    
    Nextion.print(cmd);
    Nextion.write(0xFF);
    Nextion.write(0xFF);
    Nextion.write(0xFF);
}
void loop() {
    char buf[15];
    uint8_t plotValues[1000];
    float TZ1 = thermocouple.readCelsius();
    //sprintf(floatBuff,"Tz1: %.2f",thermocouple.readCelsius());
    Nextion.print("page0.x0.vvs0=0");
    comandEnd();
    Nextion.print("page0.x0.vvs1=2");
    comandEnd();
    Nextion.print("page0.x0.val=");
    Nextion.print(TZ1*100,0);    
    // Nextion.print("\"");    
    comandEnd();

    // for (size_t i = 1; i < 1000; i++)
    // {
    //   sprintf(buf, "addt 3,0,%u", int(i/3));
       sprintf(buf, "addt 3,0,%u", TZ1);
       sendCommand(buf);
    // }


    


    delay(500);
}