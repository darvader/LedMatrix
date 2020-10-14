#define double_buffer

#include <Timer.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>


Timer::Timer(PxMATRIX *display)
{
    this->display = display;
    this->timer = 30;
    this->elapsed = timer;
}

Timer::~Timer()
{
}

void Timer::setTimer(uint16_t timer) {
    this->timer = timer;
    this->elapsed = timer;
    this->started = false;
}

void Timer::start() {
    this->startTime = millis();
    this->started = true;
}

void Timer::pause() {
    started = !started;
    this->startTime = millis() - (timer - elapsed)*1000;
}

void Timer::show() {
    static long blinkingStart = 0;

    display->clearDisplay();
    display->setTextSize(2);
    uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536 - 65536L*3/4*elapsed/timer));
    uint16_t color = display->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
    display->setTextColor(color);
    display->setCursor(3,8);
    if (started) {
        elapsed = (startTime + timer*1000.0f - millis())/1000.0f;

        if (timer - elapsed < 1.0) {
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
    display->showBuffer();
}