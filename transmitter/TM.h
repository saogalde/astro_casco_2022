//#include <string.h>
#include <DS3231.h>
#include <SD.h>
//#include <LiquidCrystal.h>

//#define RTC_UNAVAILABLE
//#define SD_UNAVAILABLE
//#define RF_UNAVAILABLE
#define LCD_UNAVAILABLE
#define TINY_SCREEN_UNAVAILABLE
#define DEBUG

#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_RS A0
#define LCD_EN A1

#define ERROR_PIN 3
#define RF_TX_PIN 4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

class TM {
  public:
    TM();
    int transmitTelemetry(char*);
    DS3231 clock;
    File logfile;
    String logfilename;
    //void onTxDone();
  private:
    String buffer;
    String getTimestamp(bool screen = false);
    String getTimestampFilename();
    String getTimestampFolderName();
    const char* clockReliability();
    bool isClockReliable();
    
};
