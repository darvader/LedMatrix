#include "AlexaIntegration.h"
#include <cstring>

AlexaIntegration::AlexaIntegration(ModeManager* modeManager, IDisplay* display)
    : modeManager_(modeManager), display_(display), off_(false), displayDrawTime_(40) {}

void AlexaIntegration::setup() {
    fauxmo_.addDevice("Uhr 1");
    fauxmo_.addDevice("Uhr 2");
    fauxmo_.addDevice("Uhr 3");
    fauxmo_.addDevice("Uhr 4");
    fauxmo_.addDevice("Schnee");
    fauxmo_.addDevice("Plasma");
    fauxmo_.addDevice("Uhr");

    fauxmo_.setPort(80);
    fauxmo_.enable(false);

    fauxmo_.onSetState([this](unsigned char device_id, const char* device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n",
                      device_id, device_name, state ? "ON" : "OFF", value);

        if (std::strcmp(device_name, "Uhr") == 0) {
            if (!state) {
                off_ = true;
                display_->showBuffer();
                display_->clearScreen();
            }
            displayDrawTime_ = value / 255.0 * 60;
            return;
        }

        off_ = false;

        if (std::strcmp(device_name, "Uhr 1") == 0) { modeManager_->setMode(2); return; }
        if (std::strcmp(device_name, "Uhr 2") == 0) { modeManager_->setMode(3); return; }
        if (std::strcmp(device_name, "Uhr 3") == 0) { modeManager_->setMode(4); return; }
        if (std::strcmp(device_name, "Uhr 4") == 0) { modeManager_->setMode(5); return; }
        if (std::strcmp(device_name, "Schnee") == 0) { modeManager_->setMode(6); return; }
    });
}

void AlexaIntegration::handle() {
    fauxmo_.handle();
}
