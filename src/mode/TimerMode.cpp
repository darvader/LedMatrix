#include "TimerMode.h"
#include <Adafruit_NeoPixel.h>

TimerMode::TimerMode(IDisplay* display)
    : display_(display), subMode_(TIMER_MODE), elapsed_(30.0),
      started_(false), startTime_(millis()), timerDuration_(30),
      blinkingStart_(0) {}

void TimerMode::init() {}

void TimerMode::cleanup() {}

void TimerMode::setTimer(uint16_t seconds) {
    subMode_ = TIMER_MODE;
    timerDuration_ = seconds;
    elapsed_ = seconds;
    started_ = false;
}

void TimerMode::start() {
    subMode_ = TIMER_MODE;
    startTime_ = millis();
    started_ = true;
}

void TimerMode::pause() {
    subMode_ = TIMER_MODE;
    started_ = !started_;
    startTime_ = millis() - (timerDuration_ - elapsed_) * 1000;
}

void TimerMode::stopWatch() {
    subMode_ = STOPWATCH_MODE;
    elapsed_ = 0.0f;
    started_ = false;
}

void TimerMode::stopWatchStart() {
    subMode_ = STOPWATCH_MODE;
    started_ = true;
    startTime_ = millis();
}

void TimerMode::stopWatchStop() {
    subMode_ = STOPWATCH_MODE;
    started_ = !started_;
    elapsed_ = (millis() - startTime_) / 1000.0f;
}

void TimerMode::update() {
    switch (subMode_) {
        case TIMER_MODE: showTimer(); break;
        case STOPWATCH_MODE: showStopWatch(); break;
    }
}

void TimerMode::showTimer() {
    display_->showBuffer();
    display_->clearScreen();
    display_->setTextSize(TEXT_SIZE_LARGE);

    uint32_t c32 = Adafruit_NeoPixel::gamma32(
        Adafruit_NeoPixel::ColorHSV(65536 - 65536L * 3 / 4 * elapsed_ / timerDuration_));
    uint16_t color = display_->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
    display_->setTextColor(color);
    display_->setCursor(3, 8);

    if (started_) {
        elapsed_ = (startTime_ + timerDuration_ * 1000.0f - millis()) / 1000.0f;

        if (timerDuration_ - elapsed_ < 0.3) {
            if (millis() % 200 > 100) {
                display_->fillScreen(myGREEN);
                display_->setTextColor(myBLACK);
            }
        }
        if (elapsed_ < 0 && started_) {
            elapsed_ = 0.0f;
            started_ = false;
            blinkingStart_ = millis();
        }
    }
    if (!started_ && millis() - blinkingStart_ < 2000) {
        if (millis() % 200 > 100) {
            display_->fillScreen(myRED);
            display_->setTextColor(myBLACK);
        }
    }
    display_->printf("%05.1f", elapsed_);
}

void TimerMode::showStopWatch() {
    display_->showBuffer();
    display_->clearScreen();
    display_->setTextSize(TEXT_SIZE_LARGE);

    uint32_t c32 = Adafruit_NeoPixel::gamma32(
        Adafruit_NeoPixel::ColorHSV(65536 * elapsed_ / 30));
    uint16_t color = display_->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
    display_->setTextColor(color);
    display_->setCursor(3, 8);

    if (started_) {
        elapsed_ = (millis() - startTime_) / 1000.0f;
    }

    display_->printf("%05.1f", elapsed_);
}
