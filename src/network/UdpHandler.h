#ifndef UDP_HANDLER_H
#define UDP_HANDLER_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include "../command/CommandParser.h"
#include "../command/CommandExecutor.h"
#include "../display/IDisplay.h"
#include "../app/Config.h"

class UdpHandler {
public:
    UdpHandler(CommandExecutor* executor, IDisplay* display);

    void begin();
    void receive();

    const char* getIncomingPacket() const { return incomingPacket_; }
    int getCurrentMode() const;

private:
    void handleDetect();
    void handlePicture();

    WiFiUDP udp_;
    CommandExecutor* executor_;
    IDisplay* display_;
    char incomingPacket_[255];
    IPAddress masterIp_;
};

#endif // UDP_HANDLER_H
