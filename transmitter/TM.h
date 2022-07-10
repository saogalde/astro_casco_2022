#include <string.h>
#include <DS3231.h>
#include <SD.h>

//#define RTC_UNAVAILABLE
//#define SD_UNAVAILABLE
#define DEBUG

class TM {
  public:
    TM(String filename = "");
    int transmitTelemetry(String);
    DS3231 clock;
    File logfile;
    String logfilename;
    //void onTxDone();
  private:
    String buffer;
    String getTimestamp(bool filename = false);
    String getTimestampFilename();
    String getTimestampFolderName();
    String clockReliability();
    bool isClockReliable();
    
};
