#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
struct AstronautData {
  char lastTimeStamp[20], tm_Ok[4], t_ext[10], t_1[10], t_2[10]; 
} astronaut1, astronaut2;
void testscrolltext(String);
String buffer = "";
char bug[200];



//void testscrolltext(AstronautData&);
void tmParser(String&, AstronautData*);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("LoRa Receiver"));

  if (!LoRa.begin(433E6)) {
    Serial.println(F("Starting LoRa failed!"));
    while (1);
  }

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();

}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  buffer = "";
  if (packetSize) {
    // received a packet
    Serial.print(F("Received packet "));

    // read packet
    while (LoRa.available()) {
      buffer += (char)LoRa.read();
    }
    Serial.print(buffer);
    tmParser(buffer, &astronaut1);
    //testscrolltext(buffer);
    testscrolltext(astronaut1);
    // print RSSI of packet
    //Serial.print(F(" with RSSI "));
    //Serial.println(LoRa.packetRssi());
    //Serial.println(buffer);
    
  }
}

void tmParser(String& payload, AstronautData* a) {
  sscanf(payload.c_str(), "%s\t%s\t%s\t%s\t%s", (a->lastTimeStamp), (a->tm_Ok), (a->t_ext), (a->t_1), (a->t_2));
  Serial.println(String(a->lastTimeStamp)+" "+ a->t_ext +" "+ a->t_1 +" "+ a->t_2);
}

void testscrolltext(String payload) {
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Astronaut 1:"));
  display.println(F("  T_0:   21.3 degC"));
  display.println(payload);
  display.println(F("  T_1:   21.3 degC"));
  display.println(F("  T_2:   21.3 degC"));
  /*display.println(F("Astronaut 2:"));
  display.println(F("  T_ext: 21.3 degC"));
  display.println(F("  T_1:   21.3 degC"));
  display.println(F("  T_2:   21.3 degC"));*/
  //display.setRotation(45);
  //display.setCursor(0, 0);
  //display.println(F("AstroCasco"));
  display.display();      // Show initial text
  delay(100);
}

void testscrolltext(AstronautData& ast) {
  display.clearDisplay();
  display.setRotation(0);
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Astronaut 1:"));
  display.print("T_ext: ");
  display.print(ast.t_ext);
  display.println(" degC");
  display.print("  T_1: ");
  display.print(ast.t_1);
  display.println(" degC");
  display.print("  T_2: ");
  display.print(ast.t_2);
  display.println(" degC");
  char dateStr[11];
  char timeStr[9];
  memset(dateStr, '\0', sizeof(dateStr));
  memset(timeStr, '\0', sizeof(timeStr));
  strncpy(dateStr, ast.lastTimeStamp, 10);
  strncpy(timeStr, ast.lastTimeStamp+11, 8);
  //sscanf(ast.lastTimeStamp, "%sT%s", dateStr, timeStr);
  display.print("\nRX CLK: ");
  display.println(ast.tm_Ok);
  display.println(dateStr);
  display.println(timeStr);
  /*display.println(F("Astronaut 2:"));
  display.println(F("  T_ext: 21.3 degC"));
  display.println(F("  T_1:   21.3 degC"));
  display.println(F("  T_2:   21.3 degC"));*/
  display.setRotation(45);
  display.setCursor(0, 0);
  //display.setFont(&FreeSansBold9pt7b);

  display.println(F("AstroCasco"));
  display.display();      // Show initial text
  delay(100);
}
