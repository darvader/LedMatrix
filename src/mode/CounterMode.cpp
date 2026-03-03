#include "CounterMode.h"

CounterMode::CounterMode(IDisplay* display, TimeService* timeService, EllipseRenderer* ellipseRenderer)
    : display_(display), timeService_(timeService),
      ellipseRenderer_(ellipseRenderer), counter_(0) {}

void CounterMode::init() {
    ellipseRenderer_->init();
}

void CounterMode::update() {
    display_->showBuffer();
    display_->clearScreen();
    ellipseRenderer_->drawRunners();
    display_->setTextSize(TEXT_SIZE_LARGE);
    display_->setTextColor(myGREEN);
    display_->setCursor(13, 4);
    display_->printf("%04u", counter_);
    drawTime();
}

void CounterMode::cleanup() {
    ellipseRenderer_->cleanup();
}

void CounterMode::drawTime() {
    int h, m, s;
    timeService_->getLocalTime(h, m, s);

#ifdef ESP32
    display_->setTextSize(1);
    display_->setCursor(40, 50);
#else
    display_->setTextSize(1);
    display_->setCursor(10, 11);
#endif
    display_->setTextColor(myMAGENTA);
    display_->printf("%02d:%02d:%02d", h, m, s);
}
