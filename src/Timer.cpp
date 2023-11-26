#include <Timer.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>

#ifdef ESP8266
Timer::Timer(PxMATRIX *display)
#define TEXTSIZE 2
#endif
#ifdef ESP32
#define TEXTSIZE 4
Timer::Timer(VirtualMatrixPanel *display)
#endif
{
    this->display = display;
    this->timer = 30;
    this->elapsed = timer;
}

Timer::~Timer()
{
}

void Timer::setTimer(uint16_t timer) {
    mode = 0;
    this->timer = timer;
    this->elapsed = timer;
    this->started = false;
}

void Timer::start() {
    mode = 0;
    this->startTime = millis();
    this->started = true;
}

void Timer::pause() {
    mode = 0;
    started = !started;
    this->startTime = millis() - (timer - elapsed)*1000;
}

void Timer::showTimer() {
    static long blinkingStart = 0;

    clear();
    display->setTextSize(TEXTSIZE);
    uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536 - 65536L*3/4*elapsed/timer));
    uint16_t color = display->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
    display->setTextColor(color);
    display->setCursor(3,8);
    if (started) {
        elapsed = (startTime + timer*1000.0f - millis())/1000.0f;

        if (timer - elapsed < 0.3) {
            if (millis() % 200 > 100) {
                display->fillScreen(myGREEN);
                display->setTextColor(myBLACK);
            }
        }
        if (elapsed<0 && started) {
            elapsed = 0.0f;
            started = false;
            blinkingStart = millis();
        }
    } 
    if (!started && millis() - blinkingStart<2000) {
        if (millis() % 200 > 100) {
            display->fillScreen(myRED);
            display->setTextColor(myBLACK);
        }
    }
    display->printf("%05.1f", elapsed);
    showBuffer();
}

void Timer::showStopWatch() {
    clear();
    display->setTextSize(TEXTSIZE);
    uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536 * elapsed / 30));
    uint16_t color = display->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);

    display->setTextColor(color);
    display->setCursor(3,8);

    if (started) {
        elapsed = (millis() - startTime)/1000.0f;
    }

    display->printf("%05.1f", elapsed);
    showBuffer();
}

void Timer::show() {
    switch (mode)
    {
    case 0:
        showTimer();
        break;
    case 1:
        showStopWatch();
        break;
    
    default:
        break;
    }
}

void Timer::stopWatch() {
    mode = 1;
    elapsed = 0.0f;
    started = false;
}

void Timer::stopWatchStart() {
    mode = 1;
    started = true;
    startTime = millis();
}

void Timer::stopWatchStop() {
    mode = 1;
    started = !started;
    elapsed = (millis() - startTime) / 1000.0f;
}