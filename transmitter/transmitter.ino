#include "TM.h"
#include "NTC_3950_10k.h"
//#include <LiquidCrystal.h>
//#include <DS3231.h>


TM *tm;
NTC_3950_10k *tempSensorExt, *tempSensor1, *tempSensor2;
char telemetry[30];
//int i = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9700);
  Serial.println(F("Init"));
  tm = new TM();
  
  
  tempSensor1 = new NTC_3950_10k(A0);
  tempSensor2 = new NTC_3950_10k(A1);
  tempSensorExt = new NTC_3950_10k(A2);
  /*tempSensorExt = new NTC_3950_10k(3);
  tempSensor1 = new NTC_3950_10k(2);
  tempSensor2 = new NTC_3950_10k(1);*/
  memset(telemetry, 0, sizeof(telemetry));
}

void loop() {
  // put your main code here, to run repeatedly:
  memset(telemetry, 0, sizeof(telemetry));
  strcat(telemetry,tempSensorExt->readTemperatureStr());
  strcat(telemetry,"\t");
  strcat(telemetry,tempSensor1->readTemperatureStr());
  strcat(telemetry,"\t");
  strcat(telemetry,tempSensor2->readTemperatureStr());

  Serial.println(telemetry);

  tm->transmitTelemetry(telemetry);
  delay(2000);
}
