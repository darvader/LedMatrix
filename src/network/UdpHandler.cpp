#include "UdpHandler.h"
#include "../util/Colors.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#ifdef ESP32
static const uint8_t replyPacket[] = "LedMatrix";
#else
static const char replyPacket[] = "LedMatrix";
#endif

UdpHandler::UdpHandler(CommandExecutor* executor, IDisplay* display)
    : executor_(executor), display_(display) {
    memset(incomingPacket_, 0, sizeof(incomingPacket_));
}

void UdpHandler::begin() {
    udp_.begin(UDP_PORT);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UDP_PORT);
}

void UdpHandler::receive() {
    int packetSize = udp_.parsePacket();
    if (!packetSize) return;

    int len = udp_.read(incomingPacket_, 255);
    if (len > 0) {
        incomingPacket_[len] = 0;
    }

    Command cmd = CommandParser::parse(incomingPacket_, len);

    if (cmd.type == CommandType::Detect) {
        handleDetect();
        return;
    }

    executor_->execute(cmd);

    // Handle picture data if in picture mode (mode 20)
    if (cmd.type == CommandType::Unknown) {
        // Could be picture data
        handlePicture();
    }
}

void UdpHandler::handleDetect() {
    masterIp_ = udp_.remoteIP();
    udp_.beginPacket(masterIp_, UDP_REPLY_PORT);
    udp_.write(replyPacket, sizeof(replyPacket));
    udp_.endPacket();
    Serial.println(F("Detect received."));

    display_->showBuffer();
    display_->clearScreen();
    for (int i = 0; i < MATRIX_WIDTH; i += 2) {
        display_->drawFastVLine(i, 0, MATRIX_HEIGHT - 1, myRED);
        display_->setCursor(10, 10);
        display_->setTextColor(myGREEN);
        display_->setTextSize(1);
        display_->print("Detect");
        delay(10);
        display_->showBuffer();
        display_->clearScreen();
    }
    display_->showBuffer();
    display_->clearScreen();
}

void UdpHandler::handlePicture() {
    int i = 0;
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        yield();
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            uint8_t r = incomingPacket_[i++];
            uint8_t g = incomingPacket_[i++];
            uint8_t b = incomingPacket_[i++];
            display_->drawPixelRGB888(x, y, r, g, b);
        }
    }
    display_->showBuffer();
}
