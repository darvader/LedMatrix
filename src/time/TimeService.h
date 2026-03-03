#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <Arduino.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFiUdp.h>

class TimeService {
public:
    TimeService();

    void begin();
    void update();

    void getLocalTime(int& h, int& m, int& s);
    time_t getLocalEpoch();
    NTPClient* getTimeClient() { return &timeClient_; }

private:
    WiFiUDP ntpUdp_;
    NTPClient timeClient_;
    Timezone tz_;
    TimeChangeRule* tcr_;
    unsigned long lastNtpUpdate_ = 0;
    static constexpr unsigned long NTP_UPDATE_INTERVAL_MS = 60000; // 60s

    void setupCompileTime();
};

#endif // TIME_SERVICE_H
