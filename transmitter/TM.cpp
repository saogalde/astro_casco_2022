#include "TM.h"
#include <SD.h>
#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include <Arduino.h>

void onTxDone();

TM::TM(String filename = "") {
  // initialize clock
  #ifndef RTC_UNAVAILABLE
  Wire.begin();
  #else
  #ifdef DEBUG
  Serial.println("RTC SET AS UNAVAILABLE");
  #endif
  #endif

  //if( filename == "" ) filename = getTimestamp(true)+".csv";
  //logfilename = filename;
  //logfilename = "DATOS.TXT";
  String foldername = getTimestampFolderName();
  logfilename = foldername+"/"+getTimestampFilename()+".tsv";
  // initialize SD card
  #ifndef SD_UNAVAILABLE
  //pinMode(10, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  //digitalWrite(10, HIGH); // Add this line
  if (!SD.begin(200000, 10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  } else {
    Serial.println("SD OK!");
  }
  delay(1);
  if(!SD.mkdir(foldername)) {
    Serial.println("Problem creating the folder "+foldername);
  }
  else {
    Serial.println("Folder created!");
  }
  delay(1);
  //SD.end();
  /*uint8_t O_WRITE = 0X02;
  uint8_t O_APPEND = 0X04;
  //(O_APPEND | O_WRITE)
  // testing the file opening
  logfile = SD.open(logfilename, FILE_WRITE);
  Serial.println(logfilename);
  Serial.println("card initialized.");
  logfile.close();*/
  #else 
  #ifdef DEBUG
  Serial.println("SD CARD SET AS UNAVAILABLE");
  #endif
  #endif

  #ifndef RF_UNAVAILABLE
  LoRa.setPins(9,8,2);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.onTxDone(onTxDone);
  #endif
  
  
  #ifdef DEBUG
  Serial.println("Attempting SD card initialization (debug mode)");
  #endif
}

int TM::transmitTelemetry(String packet) {
  buffer = "";
  buffer.concat(getTimestamp());
  buffer.concat('\t');
  buffer.concat(clockReliability());
  buffer.concat('\t');
  buffer.concat(packet);

  #ifndef SD_UNAVAILABLE
  /*// (O_APPEND | O_WRITE)
  uint8_t O_APPEND = 0X04;*/
  //pinMode(10, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  //digitalWrite(10, HIGH); // Add this line
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  logfile = SD.open(logfilename, FILE_WRITE);
  if(logfile) {
    logfile.println(buffer);
    logfile.close();
  } else {
    Serial.println("error opening "+logfilename);
  }
  SD.end();
  #endif

  #ifndef RF_UNAVAILABLE
  Serial.print("NOT Sending packet non-blocking: ");
  LoRa.beginPacket();
  LoRa.println(buffer);
  LoRa.endPacket(true); // true = async / non-blocking mode
  #endif
  
  
  #ifdef DEBUG
  Serial.println(buffer);
  #endif
  return 0;
}

String TM::getTimestampFolderName(){
  String ts = "";
  #ifndef RTC_UNAVAILABLE
  bool a = false;
  int buff = clock.getYear();
  ts.concat(buff+2000);

  buff = clock.getMonth(a);
  if(buff<10) ts.concat("0");
  ts.concat(buff);

  buff = clock.getMonth(a);
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  #else
  ts = "20220630";
  #endif
  return ts;
}

String TM::getTimestampFilename(){
  String ts = "";
  
  #ifndef RTC_UNAVAILABLE
  bool a = false;
  int buff = clock.getHour(a, a);
  ts.concat(buff);
  
  buff = clock.getMinute();
  if(buff<10) ts.concat("0");
  ts.concat(clock.getMinute());

  buff = clock.getDate();
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  #else
  ts = "170000";
  #endif
  return ts;
}

String TM::getTimestamp(bool filename = false) {
  String ts = "";
  #ifndef RTC_UNAVAILABLE
  bool a = false;
  int buff = clock.getYear();
  ts.concat(buff+2000);
  ts.concat("-");
  
  buff = clock.getMonth(a);
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  ts.concat("-");
  
  buff = clock.getDate();
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  ts.concat("T");
  
  buff = clock.getHour(a, a);
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  ts.concat(filename ? "_" : ":");
  
  buff = clock.getMinute();
  if(buff<10) ts.concat("0");
  ts.concat(clock.getMinute());
  ts.concat(filename ? "_" : ":");
  
  buff = clock.getSecond();
  if(buff<10) ts.concat("0");
  ts.concat(clock.getSecond());
  #else
  ts = "2022-06-28T23"+ (filename ? "_" : ":") + "56" + (filename ? "_" : ":")+ "00";
  #endif
  return ts;
}

bool TM::isClockReliable() {
  #ifndef RTC_UNAVAILABLE
  return clock.oscillatorCheck();
  #else
  return true;
  #endif
}

String TM::clockReliability() {
  if(TM::isClockReliable()) return "OK";
  else return "NOK";
}

void onTxDone() {
  Serial.println("TxDone");
}
