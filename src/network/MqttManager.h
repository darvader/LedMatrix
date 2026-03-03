#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <HomeAssistantMQTT.h>
#include "../app/ModeManager.h"
#include "../display/IDisplay.h"

class MqttManager {
public:
    MqttManager(ModeManager* modeManager, IDisplay* display);

    void setup();
    void loop();
    void publishState(bool on, uint8_t brightness, const char* modeName);

    void setOff(bool off) { off_ = off; }
    bool isOff() const { return off_; }
    void setDisplayDrawTime(uint8_t val) { displayDrawTime_ = val; }

    HomeAssistantMQTT& getHA() { return homeAssistant_; }
    ModeManager* getModeManager() { return modeManager_; }
    IDisplay* getDisplay() { return display_; }

    // Mode name lookup
    static const char* getModeNameById(int modeId);

private:
    HomeAssistantMQTT homeAssistant_;
    ModeManager* modeManager_;
    IDisplay* display_;
    bool off_;
    uint8_t displayDrawTime_;
    unsigned long lastStateUpdate_;

    static const char* haModeNames_[];
    static const int haModeValues_[];
    static const int haNumModes_;
};

#endif // MQTT_MANAGER_H
