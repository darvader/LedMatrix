#ifndef MANUAL_WIFI_SETUP_H
#define MANUAL_WIFI_SETUP_H

#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define PxMATRIX_double_buffer true
#define double_buffer
#include <PxMatrix.h>
#else
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#endif
#include <EEPROM.h>
#include "HomeAssistantMQTT.h"

#define MAX_WIFI_NETWORKS 5
#define NETWORK_DATA_SIZE (32 + 64 + 1) // SSID (32) + Password (64) + Active flag (1)

class ManualWifiSetup {
public:
    ManualWifiSetup(
#ifdef ESP8266
        PxMATRIX* display
#else
        VirtualMatrixPanel* display
#endif
    , uint16_t port = 80);
    bool isConfigured();
    void startSetup();
    void startConfigServer();
    void handleClient();
    bool connectToWifi();
    void saveCredentials(String ssid, String password);
    void loadCredentials(String& ssid, String& password);

    // New methods for multiple WiFi networks
    void saveNetworkCredentials(int index, String ssid, String password, bool active);
    bool loadNetworkCredentials(int index, String& ssid, String& password, bool& active);
    int getNetworkCount();
    bool deleteNetwork(int index);

    // MQTT reconnect flag
    bool isMqttReconnectNeeded() const { return mqttReconnectNeeded; }
    void clearMqttReconnectFlag() { mqttReconnectNeeded = false; }

private:
#ifdef ESP8266
    PxMATRIX* display;
    ESP8266WebServer server;
#else
    VirtualMatrixPanel* display;
    WebServer server;
#endif
    const char* apSSID = "LedMatrix-Setup";
    const char* apPassword = ""; // Open AP
    const int eepromSSIDAddr = 3;
    const int eepromPassAddr = 35; // Assuming SSID max 32 chars
    const int eepromNetworksBaseAddr = 100; // Base address for multiple networks
    const int eepromNetworkCountAddr = 99; // Address to store network count
    const int maxSSIDLen = 32;
    const int maxPassLen = 64;
    String scannedSSIDs[20];
    int numScannedSSIDs;
    bool configMode = false; // false = setup mode (AP), true = config mode (station)
    bool mqttReconnectNeeded = false;

    void handleRoot();
    void handleSave();
    void handleAddNetwork();
    void handleDeleteNetwork();
    void handleSaveMQTT();
    String generateHTML();
};

#endif