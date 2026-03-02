#include "ManualWifiSetup.h"
#include <functional>

ManualWifiSetup::ManualWifiSetup(
#ifdef ESP8266
    PxMATRIX* disp
#else
    VirtualMatrixPanel* disp
#endif
) : display(disp), server(80) {
}

bool ManualWifiSetup::isConfigured() {
    return getNetworkCount() > 0;
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
    server.on("/add", HTTP_POST, std::bind(&ManualWifiSetup::handleAddNetwork, this));
    server.on("/delete", HTTP_POST, std::bind(&ManualWifiSetup::handleDeleteNetwork, this));
    server.on("/savemqtt", HTTP_POST, std::bind(&ManualWifiSetup::handleSaveMQTT, this));
    server.begin();

    // Set scrolling text
    extern char scrollingText[];
    strcpy(scrollingText, "Connect to LedMatrix-Setup and go to 192.168.4.1 to setup WiFi");
}

void ManualWifiSetup::handleClient() {
    server.handleClient();
}

bool ManualWifiSetup::connectToWifi() {
    int count = getNetworkCount();
    if (count == 0) return false;

    // Try each saved network
    for (int i = 0; i < count; i++) {
        String ssid, pass;
        bool active;
        if (!loadNetworkCredentials(i, ssid, pass, active)) continue;
        if (!active) continue;

        WiFi.begin(ssid.c_str(), pass.c_str());
        Serial.print("Trying WiFi network: ");
        Serial.println(ssid);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("\nConnected to ");
            Serial.println(ssid);
            return true;
        } else {
            Serial.print("\nFailed to connect to ");
            Serial.println(ssid);
            WiFi.disconnect();
            delay(100);
        }
    }

    Serial.println("Failed to connect to any saved network");
    return false;
}

void ManualWifiSetup::saveCredentials(String ssid, String password) {
    // Legacy: save as first network entry
    saveNetworkCredentials(0, ssid, password, true);
    if (getNetworkCount() == 0) {
        EEPROM.write(eepromNetworkCountAddr, 1);
        EEPROM.commit();
    }
}

void ManualWifiSetup::loadCredentials(String& ssid, String& password) {
    // Legacy: load first network entry
    bool active;
    if (!loadNetworkCredentials(0, ssid, password, active)) {
        ssid = "";
        password = "";
    }
}

void ManualWifiSetup::saveNetworkCredentials(int index, String ssid, String password, bool active) {
    if (index < 0 || index >= MAX_WIFI_NETWORKS) return;

    int baseAddr = eepromNetworksBaseAddr + index * NETWORK_DATA_SIZE;

    // Write SSID
    for (int i = 0; i < maxSSIDLen; i++) {
        EEPROM.write(baseAddr + i, i < (int)ssid.length() ? ssid[i] : 0);
    }
    // Write password
    for (int i = 0; i < maxPassLen; i++) {
        EEPROM.write(baseAddr + maxSSIDLen + i, i < (int)password.length() ? password[i] : 0);
    }
    // Write active flag
    EEPROM.write(baseAddr + maxSSIDLen + maxPassLen, active ? 1 : 0);

    EEPROM.commit();
}

bool ManualWifiSetup::loadNetworkCredentials(int index, String& ssid, String& password, bool& active) {
    if (index < 0 || index >= MAX_WIFI_NETWORKS || index >= getNetworkCount()) return false;

    int baseAddr = eepromNetworksBaseAddr + index * NETWORK_DATA_SIZE;

    ssid = "";
    password = "";
    for (int i = 0; i < maxSSIDLen; i++) {
        char c = EEPROM.read(baseAddr + i);
        if (c == 0) break;
        ssid += c;
    }
    for (int i = 0; i < maxPassLen; i++) {
        char c = EEPROM.read(baseAddr + maxSSIDLen + i);
        if (c == 0) break;
        password += c;
    }
    active = EEPROM.read(baseAddr + maxSSIDLen + maxPassLen) == 1;

    return ssid.length() > 0;
}

int ManualWifiSetup::getNetworkCount() {
    int count = EEPROM.read(eepromNetworkCountAddr);
    if (count > MAX_WIFI_NETWORKS || count < 0) return 0;
    return count;
}

bool ManualWifiSetup::deleteNetwork(int index) {
    int count = getNetworkCount();
    if (index < 0 || index >= count) return false;

    // Shift remaining networks down
    for (int i = index; i < count - 1; i++) {
        int srcAddr = eepromNetworksBaseAddr + (i + 1) * NETWORK_DATA_SIZE;
        int dstAddr = eepromNetworksBaseAddr + i * NETWORK_DATA_SIZE;
        for (int j = 0; j < NETWORK_DATA_SIZE; j++) {
            EEPROM.write(dstAddr + j, EEPROM.read(srcAddr + j));
        }
    }

    // Clear last slot
    int lastAddr = eepromNetworksBaseAddr + (count - 1) * NETWORK_DATA_SIZE;
    for (int j = 0; j < NETWORK_DATA_SIZE; j++) {
        EEPROM.write(lastAddr + j, 0);
    }

    EEPROM.write(eepromNetworkCountAddr, count - 1);
    EEPROM.commit();
    return true;
}

void ManualWifiSetup::handleRoot() {
    server.send(200, "text/html", generateHTML());
}

void ManualWifiSetup::handleSave() {
    // Legacy handler - redirects to add
    handleAddNetwork();
}

void ManualWifiSetup::handleAddNetwork() {
    String ssid;
    if (server.hasArg("ssid") && server.arg("ssid").length() > 0) {
        ssid = server.arg("ssid");
    } else if (server.hasArg("manual_ssid") && server.arg("manual_ssid").length() > 0) {
        ssid = server.arg("manual_ssid");
    } else {
        server.send(400, "text/html", "<h1>Error: No SSID provided</h1><a href='/'>Back</a>");
        return;
    }
    String password = server.arg("password");

    int count = getNetworkCount();
    if (count >= MAX_WIFI_NETWORKS) {
        server.send(400, "text/html", "<h1>Error: Maximum number of networks reached. Delete one first.</h1><a href='/'>Back</a>");
        return;
    }

    saveNetworkCredentials(count, ssid, password, true);
    EEPROM.write(eepromNetworkCountAddr, count + 1);
    EEPROM.commit();

    server.send(200, "text/html", "<h1>Network added! Rebooting...</h1>");
    delay(2000);
    ESP.restart();
}

void ManualWifiSetup::handleDeleteNetwork() {
    if (!server.hasArg("index")) {
        server.send(400, "text/html", "<h1>Error: No network index</h1><a href='/'>Back</a>");
        return;
    }
    int index = server.arg("index").toInt();
    if (deleteNetwork(index)) {
        server.send(200, "text/html", "<h1>Network deleted!</h1><a href='/'>Back</a>");
    } else {
        server.send(400, "text/html", "<h1>Error: Invalid network index</h1><a href='/'>Back</a>");
    }
}

void ManualWifiSetup::handleSaveMQTT() {
    String mqttHost = server.arg("mqtt_host");
    String mqttPortStr = server.arg("mqtt_port");
    String mqttUser = server.arg("mqtt_user");
    String mqttPass = server.arg("mqtt_pass");

    if (mqttHost.length() == 0) {
        server.send(400, "text/html", "<h1>Error: MQTT host required</h1><a href='/'>Back</a>");
        return;
    }

    uint16_t mqttPort = mqttPortStr.length() > 0 ? mqttPortStr.toInt() : 1883;
    if (mqttPort == 0) mqttPort = 1883;

    HomeAssistantMQTT ha;
    ha.saveMQTTConfig(mqttHost, mqttPort, mqttUser, mqttPass);

    server.send(200, "text/html", "<h1>MQTT configuration saved! Rebooting...</h1>");
    delay(2000);
    ESP.restart();
}

String ManualWifiSetup::generateHTML() {
    String html = "<!DOCTYPE html><html><head><title>WiFi Setup</title>";
    html += "<style>";
    html += "body{font-family:sans-serif;margin:20px;}";
    html += "h1{color:#333;}";
    html += "table{border-collapse:collapse;margin:10px 0;}";
    html += "td,th{border:1px solid #ccc;padding:8px;text-align:left;}";
    html += "th{background:#f0f0f0;}";
    html += "input,select{padding:5px;margin:5px 0;}";
    html += ".btn{padding:8px 16px;cursor:pointer;margin:2px;}";
    html += ".btn-del{background:#e44;color:white;border:none;border-radius:3px;}";
    html += ".btn-add{background:#4a4;color:white;border:none;border-radius:3px;}";
    html += "</style></head><body>";
    html += "<h1>LedMatrix WiFi Setup</h1>";

    // Show saved networks
    int count = getNetworkCount();
    if (count > 0) {
        html += "<h2>Saved Networks</h2>";
        html += "<table><tr><th>#</th><th>SSID</th><th>Action</th></tr>";
        for (int i = 0; i < count; i++) {
            String ssid, pass;
            bool active;
            loadNetworkCredentials(i, ssid, pass, active);
            html += "<tr><td>" + String(i + 1) + "</td>";
            html += "<td>" + ssid + (active ? "" : " (disabled)") + "</td>";
            html += "<td><form action='/delete' method='POST' style='display:inline;'>";
            html += "<input type='hidden' name='index' value='" + String(i) + "'>";
            html += "<input type='submit' value='Delete' class='btn btn-del'>";
            html += "</form></td></tr>";
        }
        html += "</table>";
    }

    // Add new network form
    if (count < MAX_WIFI_NETWORKS) {
        html += "<h2>Add Network</h2>";
        html += "<form action='/add' method='POST'>";
        html += "WiFi Network: <select name='ssid'><option value=''>-- Select --</option>";
        for (int i = 0; i < numScannedSSIDs && i < 20; i++) {
            html += "<option value='" + scannedSSIDs[i] + "'>" + scannedSSIDs[i] + "</option>";
        }
        html += "</select><br>";
        html += "Or enter manually: <input type='text' name='manual_ssid' placeholder='SSID'><br>";
        html += "Password: <input type='password' name='password'><br>";
        html += "<input type='submit' value='Add Network' class='btn btn-add'>";
        html += "</form>";
    } else {
        html += "<p>Maximum number of networks (" + String(MAX_WIFI_NETWORKS) + ") reached. Delete one to add a new network.</p>";
    }

    // MQTT Configuration section
    html += "<h2>Home Assistant MQTT Configuration</h2>";

    // Check if MQTT is already configured
    HomeAssistantMQTT ha;
    String mqttHost;
    uint16_t mqttPort;
    String mqttUser, mqttPass;
    bool mqttConfigured = ha.loadMQTTConfig(mqttHost, mqttPort, mqttUser, mqttPass);

    if (mqttConfigured) {
        html += "<p><strong>Current MQTT Broker:</strong> " + mqttHost + ":" + String(mqttPort);
        if (mqttUser.length() > 0) {
            html += " (User: " + mqttUser + ")";
        }
        html += "</p>";
    }

    html += "<form action='/savemqtt' method='POST'>";
    html += "MQTT Broker Host/IP: <input type='text' name='mqtt_host' value='" +
            (mqttConfigured ? mqttHost : "") + "' placeholder='192.168.1.100' required><br>";
    html += "MQTT Port: <input type='number' name='mqtt_port' value='" +
            String(mqttConfigured ? mqttPort : 1883) + "' placeholder='1883'><br>";
    html += "MQTT Username: <input type='text' name='mqtt_user' value='" +
            (mqttConfigured ? mqttUser : "") + "' placeholder='(optional)'><br>";
    html += "MQTT Password: <input type='password' name='mqtt_pass' placeholder='(optional)'><br>";
    html += "<input type='submit' value='Save MQTT Config' class='btn btn-add'>";
    html += "</form>";

    html += "<p><small><strong>Note:</strong> Configure your Home Assistant MQTT broker first. ";
    html += "After saving, the LED Matrix will appear as a 'Light' entity with multiple effects/modes.</small></p>";

    html += "</body></html>";
    return html;
}
