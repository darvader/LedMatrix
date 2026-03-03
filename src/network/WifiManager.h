#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "../display/IDisplay.h"
#include "../network/ManualWifiSetup.h"

class WifiManager {
public:
    explicit WifiManager(IDisplay* display);
    ~WifiManager();

    bool setup();
    void handleClient();
    bool isInSetupMode() const { return inSetupMode_; }

    ManualWifiSetup* getWifiSetup() { return wifiSetup_; }
    ManualWifiSetup* getConfigServer() { return mqttConfigServer_; }

    bool isMqttReconnectNeeded() const;
    void clearMqttReconnectFlag();

private:
    IDisplay* display_;
    ManualWifiSetup* wifiSetup_;
    ManualWifiSetup* mqttConfigServer_;
    bool inSetupMode_;
};

#endif // WIFI_MANAGER_H
