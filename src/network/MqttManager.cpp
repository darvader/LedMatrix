#include "MqttManager.h"
#include "../app/Config.h"

const char* MqttManager::haModeNames_[] = {
    "Scoreboard", "Clock 1", "Clock 2", "Clock 3", "Clock 4",
    "Snow", "Plasma", "Colored Snow", "Game of Life",
    "Ellipse", "Star Wars", "Timer", "Counter", "Mandelbrot"
};
const int MqttManager::haModeValues_[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 30, 40, 60};
const int MqttManager::haNumModes_ = sizeof(haModeValues_) / sizeof(haModeValues_[0]);

// Static instance pointer for callbacks (HomeAssistantMQTT uses raw function pointers)
static MqttManager* s_instance = nullptr;

static void haStateCallback(bool on) {
    if (!s_instance) return;
    if (on) {
        s_instance->setOff(false);
    } else {
        s_instance->setOff(true);
    }
}

static void haBrightnessCallback(uint8_t brightness) {
    if (!s_instance) return;
    s_instance->setDisplayDrawTime(brightness / 255.0 * 60);
    s_instance->getDisplay()->setBrightness(brightness);
}

static void haModeCallback(int modeNum) {
    if (!s_instance) return;
    s_instance->getModeManager()->setMode(modeNum);
}

MqttManager::MqttManager(ModeManager* modeManager, IDisplay* display)
    : modeManager_(modeManager), display_(display),
      off_(false), displayDrawTime_(DEFAULT_DISPLAY_DRAW_TIME),
      lastStateUpdate_(0) {
    s_instance = this;
}

void MqttManager::setup() {
    homeAssistant_.setDevice("LED Matrix", "DIY", "ESP LED Matrix");
    homeAssistant_.setDevicePrefix("ledmatrix");
    homeAssistant_.setModes(haModeNames_, haModeValues_, haNumModes_);

    homeAssistant_.onState(haStateCallback);
    homeAssistant_.onBrightness(haBrightnessCallback);
    homeAssistant_.onMode(haModeCallback);

    homeAssistant_.setup();
}

void MqttManager::loop() {
    homeAssistant_.loop();

    if (millis() - lastStateUpdate_ > HA_STATE_UPDATE_INTERVAL) {
        lastStateUpdate_ = millis();
        const char* modeName = getModeNameById(modeManager_->getMode());
        if (strlen(modeName) > 0) {
            homeAssistant_.publishState(!off_, displayDrawTime_, modeName);
        }
    }
}

void MqttManager::publishState(bool on, uint8_t brightness, const char* modeName) {
    homeAssistant_.publishState(on, brightness, modeName);
}

const char* MqttManager::getModeNameById(int modeId) {
    for (int i = 0; i < haNumModes_; i++) {
        if (haModeValues_[i] == modeId) {
            return haModeNames_[i];
        }
    }
    return "";
}
