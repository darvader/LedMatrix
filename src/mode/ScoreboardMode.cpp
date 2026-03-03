#include "ScoreboardMode.h"
#include <Fonts/Picopixel.h>

ScoreboardMode::ScoreboardMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeService_(timeService),
      pointsLeft_(0), pointsRight_(0),
      setsLeft_(0), setsRight_(0),
      teamLeftServes_(false), scrollX_(0),
      timeoutStart_(0), timeoutOn_(false) {
    strcpy(scrollingText_, "VSV Jena 90 e.V. : Gastmannschaft");
}

void ScoreboardMode::init() {
    scrollX_ = 0;
}

void ScoreboardMode::update() {
    display_->showBuffer();
    display_->clearScreen();
    showScore();
    if (timeoutOn_)
        showTimeout();
    else
        showScrollingText();
    showTime();
}

void ScoreboardMode::cleanup() {}

void ScoreboardMode::showScore() {
#ifdef ESP32
    display_->setTextSize(4);
    display_->setCursor(3, 1);
#else
    display_->setTextSize(2);
    display_->setCursor(3, 0);
#endif
    display_->setTextColor(myWHITE);
    display_->printf("%02d:%02d", pointsLeft_, pointsRight_);

    display_->setTextColor(myRED);
#ifdef ESP32
    display_->setCursor(2, 31);
    display_->setTextSize(2);
    display_->print(setsLeft_);
    display_->setCursor(116, 31);
    display_->print(setsRight_);
#else
    display_->setCursor(0, 15);
    display_->setTextSize(1);
    display_->print(setsLeft_);
    display_->setCursor(120, 30);
    display_->print(setsRight_);
#endif

#ifdef ESP32
    if (teamLeftServes_) {
        display_->fillCircle(17, 38, 4, myYELLOW);
    } else {
        display_->fillCircle(110, 38, 4, myYELLOW);
    }
#else
    if (teamLeftServes_) {
        display_->fillCircle(11, 18, 3, myYELLOW);
    } else {
        display_->fillCircle(51, 18, 3, myYELLOW);
    }
#endif
}

void ScoreboardMode::showTime() {
    int h, m, s;
    timeService_->getLocalTime(h, m, s);

    display_->setTextSize(1);
    display_->setTextColor(myMAGENTA);
#ifdef ESP32
    display_->setCursor(40, 33);
#else
    display_->setFont(&Picopixel);
    display_->setCursor(18, 20);
#endif
    display_->printf("%02d:%02d:%02d", h, m, s);
    display_->setFont();
}

void ScoreboardMode::showScrollingText() {
    display_->setTextColor(myGREEN);
#ifdef ESP32
    display_->setTextSize(2);
    display_->setCursor(MATRIX_WIDTH + 1 - (int)scrollX_, MATRIX_HEIGHT - 16);
    int size = display_->print(scrollingText_) * 12 + MATRIX_WIDTH;
#else
    display_->setTextSize(1);
    display_->setCursor(MATRIX_WIDTH + 1 - (int)scrollX_, MATRIX_HEIGHT - 7);
    int size = display_->print(scrollingText_) * 6 + MATRIX_WIDTH;
#endif
    scrollX_ += 1.0f;
    if (scrollX_ > size) {
        scrollX_ = 0;
    }
}

void ScoreboardMode::showTimeout() {
    unsigned long secs = millis() - timeoutStart_;
    if (secs > 30000) {
        timeoutOn_ = false;
    }
    int w = secs / 30000.0 * MATRIX_WIDTH;
    display_->fillRect(0, MATRIX_HEIGHT - 8, w, 8, timeoutColor);

    display_->setTextSize(1);
    display_->setTextColor(myOrange);
#ifdef ESP32
    display_->setCursor(40, MATRIX_HEIGHT - 7);
#else
    display_->setCursor(4, MATRIX_HEIGHT - 7);
#endif
    display_->printf("T-Out:%ds", 30 - (int)(secs / 1000));
}

void ScoreboardMode::updateScore(const char* packet) {
    pointsLeft_ = packet[12];
    pointsRight_ = packet[13];
    setsLeft_ = packet[14];
    setsRight_ = packet[15];
    teamLeftServes_ = packet[16];
}

void ScoreboardMode::updateScrollingText(const char* packet) {
    int j = 0;
    for (int i = 11; packet[i] != 0; i++) {
        scrollingText_[j++] = packet[i];
    }
    scrollingText_[j] = 0;
}

void ScoreboardMode::startTimeout() {
    timeoutStart_ = millis();
    timeoutOn_ = true;
}
