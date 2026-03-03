#include "WifiManager.h"

// ManualWifiSetup uses raw display types, so we need to pass through
// We use a workaround since ManualWifiSetup hasn't been refactored yet
#ifdef ESP8266
#include "../display/DisplayESP8266.h"
#else
#include "../display/DisplayESP32.h"
#endif

WifiManager::WifiManager(IDisplay* display)
    : display_(display), wifiSetup_(nullptr), mqttConfigServer_(nullptr), inSetupMode_(false) {}

WifiManager::~WifiManager() {
    delete wifiSetup_;
    delete mqttConfigServer_;
}

bool WifiManager::setup() {
    // ManualWifiSetup needs raw display pointer - get it from adapter
#ifdef ESP8266
    auto* espDisplay = static_cast<DisplayESP8266*>(display_);
    wifiSetup_ = new ManualWifiSetup(espDisplay->getPxDisplay());
#else
    auto* espDisplay = static_cast<DisplayESP32*>(display_);
    wifiSetup_ = new ManualWifiSetup(espDisplay->getVirtualDisplay());
#endif

    if (!wifiSetup_->isConfigured()) {
        inSetupMode_ = true;
        wifiSetup_->startSetup();
        return false;
    }

    if (!wifiSetup_->connectToWifi()) {
        inSetupMode_ = true;
        wifiSetup_->startSetup();
        return false;
    }

    // Start MQTT config web server on port 8080
#ifdef ESP8266
    mqttConfigServer_ = new ManualWifiSetup(espDisplay->getPxDisplay(), 8080);
#else
    mqttConfigServer_ = new ManualWifiSetup(espDisplay->getVirtualDisplay(), 8080);
#endif
    mqttConfigServer_->startConfigServer();
    return true;
}

void WifiManager::handleClient() {
    if (inSetupMode_ && wifiSetup_) {
        wifiSetup_->handleClient();
    }
    if (mqttConfigServer_) {
        mqttConfigServer_->handleClient();
    }
}

bool WifiManager::isMqttReconnectNeeded() const {
    return mqttConfigServer_ && mqttConfigServer_->isMqttReconnectNeeded();
}

void WifiManager::clearMqttReconnectFlag() {
    if (mqttConfigServer_) {
        mqttConfigServer_->clearMqttReconnectFlag();
    }
}
