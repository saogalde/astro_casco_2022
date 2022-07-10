#include "TM.h"

TM *tm;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9700);
  Serial.println("Init");
  tm = new TM();
}

void loop() {
  // put your main code here, to run repeatedly:
  tm->transmitTelemetry("21\t21\t21");
  delay(2000);
  tm->transmitTelemetry("22\t22\t22");
  delay(2000);
  tm->transmitTelemetry("23\t23\t23");
  delay(2000);
  tm->transmitTelemetry("24\t24\t24");
  delay(2000);
}
