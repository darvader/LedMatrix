#ifndef MANUAL_WIFI_SETUP_H
#define MANUAL_WIFI_SETUP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

#ifdef ESP32
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#endif

class ManualWifiSetup {
public:
    ManualWifiSetup(VirtualMatrixPanel* display);
    bool isConfigured();
    void startSetup();
    void handleClient();
    bool connectToWifi();
    void saveCredentials(String ssid, String password);
    void loadCredentials(String& ssid, String& password);

private:
    VirtualMatrixPanel* display;
    WebServer server;
    const char* apSSID = "LedMatrix-Setup";
    const char* apPassword = ""; // Open AP
    const int eepromSSIDAddr = 3;
    const int eepromPassAddr = 35; // Assuming SSID max 32 chars
    const int maxSSIDLen = 32;
    const int maxPassLen = 64;

    void handleRoot();
    void handleSave();
    String generateHTML();
};

#endif