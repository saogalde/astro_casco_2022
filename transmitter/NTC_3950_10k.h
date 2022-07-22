class NTC_3950_10k {
  public:
    NTC_3950_10k(int);
    float readTemperature();
    char* readTemperatureStr();
    char lastReadTemperature[10];
  private:
    int pin;
    float raw2celsius(unsigned int x);
};
