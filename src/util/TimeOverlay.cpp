#include "TimeOverlay.h"
#include "../app/Config.h"

void TimeOverlay::drawTimeWithBackground() {
    int w = display_->width();
    int h = display_->height();
#ifdef ESP32
    display_->fillRect(39, h / 2 - 5, 49, 9, myBLACK);
#else
    display_->fillRect(9, 10, 49, 9, myBLACK);
#endif
    drawTime();
}

void TimeOverlay::drawTime() {
    int h, m, s;
    timeService_->getLocalTime(h, m, s);

#ifdef ESP32
    display_->setTextSize(1);
    display_->setCursor(40, (display_->height() / 2) - 4);
#else
    display_->setTextSize(1);
    display_->setCursor(10, 11);
#endif
    display_->setTextColor(myMAGENTA);
    display_->printf("%02d:%02d:%02d", h, m, s);
}
