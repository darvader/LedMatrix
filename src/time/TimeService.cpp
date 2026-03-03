#include "TimeService.h"

// Central European Time Zone (Germany)
static const TimeChangeRule myDST PROGMEM = {"CEST", Last, Sun, Mar, 2, 120};
static const TimeChangeRule mySTD PROGMEM = {"CET", Last, Sun, Oct, 3, 60};

TimeService::TimeService()
    : timeClient_(ntpUdp_, "pool.ntp.org", 0, 600000),
      tz_(myDST, mySTD),
      tcr_(nullptr) {}

void TimeService::begin() {
    setupCompileTime();
    timeClient_.begin();
}

void TimeService::update() {
    timeClient_.update();
}

void TimeService::getLocalTime(int& h, int& m, int& s) {
    unsigned long now = millis();
    if (now - lastNtpUpdate_ >= NTP_UPDATE_INTERVAL_MS) {
        timeClient_.update();
        lastNtpUpdate_ = now;
    }
    time_t utc = timeClient_.getEpochTime();
    time_t local = tz_.toLocal(utc, &tcr_);
    h = hour(local);
    m = minute(local);
    s = second(local);
}

time_t TimeService::getLocalEpoch() {
    unsigned long now = millis();
    if (now - lastNtpUpdate_ >= NTP_UPDATE_INTERVAL_MS) {
        timeClient_.update();
        lastNtpUpdate_ = now;
    }
    time_t utc = timeClient_.getEpochTime();
    return tz_.toLocal(utc, &tcr_);
}

// Function to return the compile date and time as a time_t value
static time_t compileTime() {
    const time_t FUDGE(10);
    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[4], *m;
    tmElements_t tm;

    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);
    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    time_t t = makeTime(tm);
    return t + FUDGE;
}

void TimeService::setupCompileTime() {
    setTime(tz_.toUTC(compileTime()));
}
