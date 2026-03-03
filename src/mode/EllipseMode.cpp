#include "EllipseMode.h"

EllipseMode::EllipseMode(IDisplay* display, TimeService* timeService, EllipseRenderer* ellipseRenderer)
    : display_(display), timeOverlay_(display, timeService), ellipseRenderer_(ellipseRenderer) {}

void EllipseMode::init() {
    ellipseRenderer_->init();
}

void EllipseMode::update() {
    display_->showBuffer();
    display_->clearScreen();
    ellipseRenderer_->drawRunners();
    timeOverlay_.drawTime();
}

void EllipseMode::cleanup() {
    ellipseRenderer_->cleanup();
}
