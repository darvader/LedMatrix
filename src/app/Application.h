#ifndef APPLICATION_H
#define APPLICATION_H

#include "../display/IDisplay.h"
#include "../display/DisplayFactory.h"
#include "../time/TimeService.h"
#include "ModeManager.h"
#include "../command/CommandExecutor.h"
#include "../network/UdpHandler.h"
#include "../network/WifiManager.h"
#include "../network/MqttManager.h"
#include "../integration/AlexaIntegration.h"
#include "../util/EllipseRenderer.h"

// Forward declarations for mode classes
class ScoreboardMode;
class ClockMode;
class SnowMode;
class PlasmaMode;
class GameOfLifeMode;
class EllipseMode;
class StarWarsMode;
class MandelbrotMode;
class TimerMode;
class CounterMode;

class Application {
public:
    Application();
    ~Application();

    void setup();
    void loop();

private:
    void setupDisplay();
    void setupEEPROM();
    void createModes();
    void processEvents(unsigned long ms);

    // Core subsystems
    DisplayResources displayRes_;
    IDisplay* display_;
    TimeService timeService_;
    ModeManager modeManager_;

    // Network
    WifiManager* wifiManager_;
    MqttManager* mqttManager_;
    UdpHandler* udpHandler_;

    // Integration
    AlexaIntegration* alexa_;

    // Shared utilities
    EllipseRenderer* ellipseRenderer_;

    // Mode instances (owned)
    ScoreboardMode* scoreboardMode_;
    ClockMode* clockMode1_;
    ClockMode* clockMode2_;
    ClockMode* clockMode3_;
    ClockMode* clockMode4_;
    SnowMode* snowMode_;
    SnowMode* coloredSnowMode_;
    PlasmaMode* plasmaMode_;
    GameOfLifeMode* gameOfLifeMode_;
    EllipseMode* ellipseMode_;
    StarWarsMode* starWarsMode_;
    MandelbrotMode* mandelbrotMode_;
    TimerMode* timerMode_;
    CounterMode* counterMode_;

    // Command
    CommandExecutor* cmdExecutor_;

    // State
    bool off_;
    uint8_t displayDrawTime_;

#ifdef ESP8266
    // Display ticker for ESP8266
    void setupDisplayTicker();
#endif
};

#endif // APPLICATION_H
