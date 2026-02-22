#include <Arduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
#endif
#ifdef ESP32
  #include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WifiUpdate.h>
#include "config.h"

// Static IP configuration variables
static bool useStaticIP = false;
static IPAddress staticIP;
static IPAddress gateway;
static IPAddress subnet;
static IPAddress dns;


void setupWifiUpdate()
{
  // scan....
  bool found = false;
  Serial.println("Start scanning");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i)
  {
    Serial.println(WiFi.SSID(i));
    if (WiFi.SSID(i) == WIFI_SSID_HOTSPOT)
    {
      WiFi.begin(WIFI_SSID_HOTSPOT, WIFI_PASS_HOTSPOT);
      found = true;
      break;
    }
    if (WiFi.SSID(i) == WIFI_SSID_KERTSCHUETZ)
    {
      WiFi.begin(WIFI_SSID_KERTSCHUETZ, WIFI_PASS_KERTSCHUETZ);
      found = true;
      break;
    }
    if (WiFi.SSID(i) == WIFI_SSID_CHRISTIAN)
    {
      WiFi.begin(WIFI_SSID_CHRISTIAN, WIFI_PASS_CHRISTIAN);
      found = true;
      break;
    }
    if (WiFi.SSID(i) == WIFI_SSID_WENIGENAUMA_EXT)
    {
      WiFi.begin(WIFI_SSID_WENIGENAUMA_EXT, WIFI_PASS_WENIGENAUMA_EXT);
      found = true;
      break;
    }
    if (WiFi.SSID(i) == WIFI_SSID_WENIGENAUMA)
    {
      WiFi.begin(WIFI_SSID_WENIGENAUMA, WIFI_PASS_WENIGENAUMA);
      found = true;
      break;
    }
    if (WiFi.SSID(i) == WIFI_SSID_LEDPUSHBUTTONS)
    {
      WiFi.begin(WIFI_SSID_LEDPUSHBUTTONS, WIFI_PASS_LEDPUSHBUTTONS);
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.print("Setting soft-AP ... ");
    Serial.println(WiFi.softAP(WIFI_SSID_LEDPUSHBUTTONS, WIFI_PASS_LEDPUSHBUTTONS, 1, false, 20) ? "Ready" : "Failed!");
  }

  WiFi.mode(WIFI_STA);
  
  // Configure static IP if requested
  if (useStaticIP) {
    Serial.println("Configuring static IP...");
    if (WiFi.config(staticIP, gateway, subnet, dns)) {
      Serial.print("Static IP configured: ");
      Serial.println(staticIP);
    } else {
      Serial.println("Failed to configure static IP!");
    }
  }
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupWifiUpdate(IPAddress ip, IPAddress gw, IPAddress sn) {
  setupWifiUpdate(ip, gw, sn, gw); // Use gateway as DNS by default
}

void setupWifiUpdate(IPAddress ip, IPAddress gw, IPAddress sn, IPAddress dnsServer) {
  useStaticIP = true;
  staticIP = ip;
  gateway = gw;
  subnet = sn;
  dns = dnsServer;
  setupWifiUpdate();
}
