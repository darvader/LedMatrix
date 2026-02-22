#include "ManualWifiSetup.h"
#include <functional>

ManualWifiSetup::ManualWifiSetup(VirtualMatrixPanel* disp) : display(disp), server(80) {
}

bool ManualWifiSetup::isConfigured() {
    String ssid, pass;
    loadCredentials(ssid, pass);
    return ssid.length() > 0;
}

void ManualWifiSetup::startSetup() {
    // Start AP
    WiFi.softAP(apSSID, apPassword);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Scan for networks
    Serial.println("Scanning for WiFi networks...");
    numScannedSSIDs = WiFi.scanNetworks();
    Serial.print("Found ");
    Serial.print(numScannedSSIDs);
    Serial.println(" networks");
    for (int i = 0; i < numScannedSSIDs && i < 20; i++) {
        scannedSSIDs[i] = WiFi.SSID(i);
        Serial.println(scannedSSIDs[i]);
    }

    // Setup web server
    server.on("/", HTTP_GET, std::bind(&ManualWifiSetup::handleRoot, this));
    server.on("/save", HTTP_POST, std::bind(&ManualWifiSetup::handleSave, this));
    server.begin();

    // Set scrolling text
    extern char scrollingText[];
    strcpy(scrollingText, "Connect to LedMatrix-Setup and go to 192.168.4.1 to setup WiFi");
}

void ManualWifiSetup::handleClient() {
    server.handleClient();
}

bool ManualWifiSetup::connectToWifi() {
    String ssid, pass;
    loadCredentials(ssid, pass);
    if (ssid.length() == 0) return false;

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("Connecting to ");
    Serial.println(ssid);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected!");
        return true;
    } else {
        Serial.println("Failed to connect");
        return false;
    }
}

void ManualWifiSetup::saveCredentials(String ssid, String password) {
    for (int i = 0; i < maxSSIDLen; i++) {
        EEPROM.write(eepromSSIDAddr + i, i < ssid.length() ? ssid[i] : 0);
    }
    for (int i = 0; i < maxPassLen; i++) {
        EEPROM.write(eepromPassAddr + i, i < password.length() ? password[i] : 0);
    }
    EEPROM.commit();
}

void ManualWifiSetup::loadCredentials(String& ssid, String& password) {
    ssid = "";
    password = "";
    for (int i = 0; i < maxSSIDLen; i++) {
        char c = EEPROM.read(eepromSSIDAddr + i);
        if (c == 0) break;
        ssid += c;
    }
    for (int i = 0; i < maxPassLen; i++) {
        char c = EEPROM.read(eepromPassAddr + i);
        if (c == 0) break;
        password += c;
    }
}

void ManualWifiSetup::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void ManualWifiSetup::handleSave() {
    String ssid;
    if (server.hasArg("ssid") && server.arg("ssid").length() > 0) {
        ssid = server.arg("ssid");
    } else if (server.hasArg("manual_ssid") && server.arg("manual_ssid").length() > 0) {
        ssid = server.arg("manual_ssid");
    } else {
        server.send(400, "text/html", "<h1>Error: No SSID provided</h1>");
        return;
    }
    String password = server.arg("password");
    saveCredentials(ssid, password);
    server.send(200, "text/html", "<h1>Credentials saved! Rebooting...</h1>");
    delay(2000);
    ESP.restart();
}

String ManualWifiSetup::generateHTML() {
    String html = "<!DOCTYPE html><html><head><title>WiFi Setup</title></head><body>";
    html += "<h1>LedMatrix WiFi Setup</h1>";
    html += "<form action='/save' method='POST'>";
    html += "WiFi Network: <select name='ssid'>";
    for (int i = 0; i < numScannedSSIDs; i++) {
        html += "<option value='" + scannedSSIDs[i] + "'>" + scannedSSIDs[i] + "</option>";
    }
    html += "</select><br>";
    html += "Or enter manually: <input type='text' name='manual_ssid' placeholder='SSID'><br>";
    html += "Password: <input type='password' name='password'><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form>";
    html += "</body></html>";
    return html;
}