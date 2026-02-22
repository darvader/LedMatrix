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

class ManualWifiSetup {
public:
    ManualWifiSetup(
#ifdef ESP8266
        PxMATRIX* display
#else
        VirtualMatrixPanel* display
#endif
    );
    bool isConfigured();
    void startSetup();
    void handleClient();
    bool connectToWifi();
    void saveCredentials(String ssid, String password);
    void loadCredentials(String& ssid, String& password);

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
    const int maxSSIDLen = 32;
    const int maxPassLen = 64;
    String scannedSSIDs[20];
    int numScannedSSIDs;

    void handleRoot();
    void handleSave();
    String generateHTML();
};

#endif