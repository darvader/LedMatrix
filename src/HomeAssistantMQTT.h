#ifndef HOME_ASSISTANT_MQTT_H
#define HOME_ASSISTANT_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <EEPROM.h>

#define MQTT_BUFFER_SIZE 512

class HomeAssistantMQTT {
public:
    // Callback types for controlling the LED Matrix
    typedef void (*StateCallback)(bool on);
    typedef void (*BrightnessCallback)(uint8_t brightness);
    typedef void (*ModeCallback)(int mode);

    HomeAssistantMQTT();

    void setup();
    void loop();
    bool isConnected();

    // Set callbacks
    void onState(StateCallback cb) { stateCb = cb; }
    void onBrightness(BrightnessCallback cb) { brightnessCb = cb; }
    void onMode(ModeCallback cb) { modeCb = cb; }

    // Publish current state to Home Assistant
    void publishState(bool on, uint8_t brightness, const char* effect);

    // MQTT broker configuration
    void setBroker(const char* host, uint16_t port = 1883);
    void setCredentials(const char* user, const char* password);

    // EEPROM storage for MQTT config
    void saveMQTTConfig(String host, uint16_t port, String user, String password);
    bool loadMQTTConfig(String& host, uint16_t& port, String& user, String& password);

    // EEPROM layout for MQTT config (starting at address 600)
    static const int eepromMQTTBaseAddr = 600;
    static const int maxHostLen = 40;
    static const int maxUserLen = 32;
    static const int maxMQTTPassLen = 32;
    // Total: 1 (configured flag) + 40 (host) + 2 (port) + 32 (user) + 32 (pass) = 107 bytes

private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    StateCallback stateCb = nullptr;
    BrightnessCallback brightnessCb = nullptr;
    ModeCallback modeCb = nullptr;

    char mqttHost[41] = "";
    uint16_t mqttPort = 1883;
    char mqttUser[33] = "";
    char mqttPass[33] = "";
    bool configured = false;

    unsigned long lastReconnectAttempt = 0;

    String deviceId;

    void mqttCallback(char* topic, byte* payload, unsigned int length);
    bool reconnect();
    void publishDiscovery();
    void publishSelectDiscovery();
};

#endif
