#include "TM.h"
#include <SD.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <string.h>
#include <Arduino.h>
#ifndef TINY_SCREEN_UNAVAILABLE
#include <U8g2lib.h>
#endif

void onTxDone();
#ifndef LCD_UNAVAILABLE
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif
#ifndef TINY_SCREEN_UNAVAILABLE
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, A5, A4); 
#endif

TM::TM() {
  // initialize clock
  #ifndef LCD_UNAVAILABLE
  lcd.begin(16,2);
  lcd.clear();
  lcd.print(F("ASTRO CASCO"));
  lcd.setCursor(0,1);
  lcd.print(F("TRANSMITTER"));
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  #else
  Serial.println(F("NO LCD AVAILABLE"));
  #endif

  #ifndef TINY_SCREEN_UNAVAILABLE
  display.begin();
  // Clear the buffer
  /*display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("DISPLAY OK"));
  */
  display.setFont(u8g2_font_ncenB10_tr);
  display.firstPage();
  do {
    display.drawStr(0,20,F("ASTRO CASCO"));
    display.drawStr(0,40,F("TRANSMITTER"));
  } while ( display.nextPage() );
  display.setFont(u8g2_font_6x10_mn);
  delay(1000);
  #endif

  #ifndef RTC_UNAVAILABLE
  Wire.begin();
  #else
  #ifdef DEBUG
  Serial.println(F("RTC SET AS UNAVAILABLE"));
  #endif
  #endif

  // ERROR_PIN to signal an error on the SD card
  pinMode(ERROR_PIN, OUTPUT);
  digitalWrite(ERROR_PIN, LOW);
  // RF_TX_PIN to signal an ongoing transmission
  pinMode(RF_TX_PIN, OUTPUT);
  digitalWrite(RF_TX_PIN, LOW);
  
  /*char foldername[9], logfilename;
  strcat(foldername, getTimestampFolderName());
  strcat(foldername, "/");
  strcat(foldername, getTimestampFolderName());
  strcat(foldername, getTimestampFolderName());*/
  String foldername = getTimestampFolderName();
  logfilename = foldername+"/"+getTimestampFilename()+".tsv";

  // initialize SD card
  #ifndef SD_UNAVAILABLE

  while (!SD.begin(200000,10)) {
    Serial.println(F("Card failed, or not present"));
    digitalWrite(ERROR_PIN, HIGH); // Add this line
    // don't do anything more:
    #ifndef LCD_UNAVAILABLE
    lcd.print(F("SD NOK, "));
    delay(1000);
    lcd.clear();
    delay(1000);
    #endif
    
    #ifndef TINY_SCREEN_UNAVAILABLE
    display.firstPage();
    do {
      display.drawStr(0,0,F("SD NOK"));
      display.drawStr(0,10,F("SD NOK"));
    } while ( display.nextPage() );
    
    #endif
    delay(1000);
  }
  Serial.println(F("SD OK! "));
  digitalWrite(ERROR_PIN, LOW); // Add this line

  #ifndef LCD_UNAVAILABLE
  lcd.print(F("SD OK, "));
  #endif
  #ifndef TINY_SCREEN_UNAVAILABLE
  display.firstPage();
  do {
    display.drawStr(0,0,"SD OK");
  } while ( display.nextPage() );
  #endif
  
  if(!SD.mkdir(foldername)) {
    Serial.println("Problem creating the folder "+foldername);
    digitalWrite(ERROR_PIN, HIGH); // Add this line
    //lcd.print("FOLDER NOK");
  }
  else {
    Serial.println("Folder created!");
    digitalWrite(ERROR_PIN, LOW); // Add this line
    //lcd.print("FOLDER OK");
  }
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
  #else
  //lcd.print("SD SET AS NONE");
  #endif
  #endif

  #ifndef RF_UNAVAILABLE
  LoRa.setPins(9,8,2);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
//    lcd.setCursor(0,1);
//    lcd.print("RF TX NOK");
    while (1);
  } else {
      Serial.println("Starting LoRa OK!");
//    lcd.setCursor(0,1);
//    lcd.print("RF TX OK");
  }
  LoRa.onTxDone(onTxDone);
  #endif
  
}

int TM::transmitTelemetry(char* packet) {
  char buffer[100];
  uint8_t errCount = 0;
  memset(buffer,0,sizeof(buffer));
  strcat(buffer, getTimestamp().c_str());
  strcat(buffer, "\t");
  strcat(buffer, clockReliability());
  strcat(buffer, "\t");
  strcat(buffer, packet);

  #ifndef SD_UNAVAILABLE
  /*// (O_APPEND | O_WRITE)
  uint8_t O_APPEND = 0X04;*/
  //pinMode(10, OUTPUT); // change this to 53 on a mega  // don't follow this!!
  //digitalWrite(10, HIGH); // Add this line
  if (!SD.begin(200000,10)) {
    Serial.println("Card failed, or not present");
    errCount++;
    // don't do anything more:
    /*lcd.print("SD NOK, ");
    delay(1000);
    lcd.clear();
    delay(1000);
    while (1);*/
    //display.println(F("SD NOK"));
  }
  logfile = SD.open(logfilename, FILE_WRITE);
  if(logfile) {
    logfile.println(buffer);
    logfile.close();
  } else {
    Serial.println("error opening "+logfilename);
    errCount++;
  }
  SD.end();
  #endif
  errCount > 0 ? digitalWrite(ERROR_PIN, HIGH) : digitalWrite(ERROR_PIN, LOW);

  #ifndef RF_UNAVAILABLE
  digitalWrite(RF_TX_PIN,HIGH);
  Serial.print("Sending packet non-blocking: ");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    
    /*lcd.clear();
    lcd.print("RF TX NOK");
    delay(1000);
    while (1);*/
//    display.println(F("RF TX NOK"));
  }
  LoRa.beginPacket();
  LoRa.println(buffer);
  
  /*
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PACKET TX");
  lcd.setCursor(0,1);
  lcd.print(getTimestamp(true));
  */
//  display.clearDisplay();
//  display.setTextSize(1); // Draw 2X-scale text
//  display.setTextColor(SSD1306_WHITE);
//  display.setCursor(0, 0);
//  display.println(F("PACKET TX"));
//  display.println(getTimestamp(true));
  #ifndef TINY_SCREEN_UNAVAILABLE
  display.firstPage();
  do {
    display.drawStr(0,0,"PACKET TX");
    display.drawStr(0,10,getTimestamp(true).c_str());
  } while ( display.nextPage() );
  #endif
  LoRa.endPacket(true); // true = async / non-blocking mode
  digitalWrite(RF_TX_PIN,LOW);
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

String TM::getTimestamp(bool screen = false) {
  String ts = "";
  #ifndef RTC_UNAVAILABLE
  bool a = false;
  int buff = clock.getYear();
  if(!screen){
    
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
  }
  buff = clock.getHour(a, a);
  if(buff<10) ts.concat("0");
  ts.concat(buff);
  ts.concat(":");
  
  buff = clock.getMinute();
  if(buff<10) ts.concat("0");
  ts.concat(clock.getMinute());
  ts.concat(":");
  
  buff = clock.getSecond();
  if(buff<10) ts.concat("0");
  ts.concat(clock.getSecond());
  #else
  if(!screen){
  ts = String("2022-06-28T23") + ":" + "56" +  ":"+ "00";
  } else {
    ts = String("16:16:56");
  }
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

const char* TM::clockReliability() {
  if(TM::isClockReliable()) return "OK";
  else return "NOK";
}

void onTxDone() {
  //Serial.println("TxDone");
  digitalWrite(RF_TX_PIN,LOW);
  LoRa.end();
}
