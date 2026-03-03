#include "Application.h"

#include "../mode/ScoreboardMode.h"
#include "../mode/ClockMode.h"
#include "../mode/SnowMode.h"
#include "../mode/PlasmaMode.h"
#include "../mode/GameOfLifeMode.h"
#include "../mode/EllipseMode.h"
#include "../mode/StarWarsMode.h"
#include "../mode/MandelbrotMode.h"
#include "../mode/TimerMode.h"
#include "../mode/CounterMode.h"
#include "../util/Colors.h"

#include <ArduinoOTA.h>
#include <EEPROM.h>

#ifdef ESP32
#include "../display/DisplayESP32.h"
#endif

#ifdef ESP8266
#include <Ticker.h>
#include "../display/DisplayESP8266.h"
static Ticker displayTicker;
static IDisplay* g_display = nullptr;
static uint8_t g_displayDrawTime = DEFAULT_DISPLAY_DRAW_TIME;

void displayUpdater() {
    auto* espDisplay = static_cast<DisplayESP8266*>(g_display);
    espDisplay->getPxDisplay()->display(g_displayDrawTime * 0.7);
}
#endif

Application::Application()
    : display_(nullptr),
      wifiManager_(nullptr), mqttManager_(nullptr), udpHandler_(nullptr),
      alexa_(nullptr), ellipseRenderer_(nullptr),
      scoreboardMode_(nullptr), clockMode1_(nullptr), clockMode2_(nullptr),
      clockMode3_(nullptr), clockMode4_(nullptr),
      snowMode_(nullptr), coloredSnowMode_(nullptr),
      plasmaMode_(nullptr), gameOfLifeMode_(nullptr),
      ellipseMode_(nullptr), starWarsMode_(nullptr),
      mandelbrotMode_(nullptr), timerMode_(nullptr), counterMode_(nullptr),
      cmdExecutor_(nullptr),
      off_(false), displayDrawTime_(DEFAULT_DISPLAY_DRAW_TIME) {}

Application::~Application() {
    delete scoreboardMode_;
    delete clockMode1_;
    delete clockMode2_;
    delete clockMode3_;
    delete clockMode4_;
    delete snowMode_;
    delete coloredSnowMode_;
    delete plasmaMode_;
    delete gameOfLifeMode_;
    delete ellipseMode_;
    delete starWarsMode_;
    delete mandelbrotMode_;
    delete timerMode_;
    delete counterMode_;
    delete ellipseRenderer_;
    delete cmdExecutor_;
    delete udpHandler_;
    delete mqttManager_;
    delete alexa_;
    delete wifiManager_;
}

void Application::setupDisplay() {
    displayRes_ = DisplayFactory::create();
    display_ = displayRes_.display;

#ifdef ESP8266
    g_display = display_;
    g_displayDrawTime = displayDrawTime_;

    display_->clearScreen();
    display_->setTextColor(myCYAN);
    display_->setCursor(2, 0);
    display_->print("Pixel");
    display_->setTextColor(myMAGENTA);
    display_->setCursor(2, 8);
    display_->print("Time");
    displayTicker.attach(0.004, displayUpdater);
    display_->showBuffer();
#endif

#ifdef ESP32
    auto* esp32Display = static_cast<DisplayESP32*>(display_);
    auto* vDisplay = esp32Display->getVirtualDisplay();

    vDisplay->fillScreen(vDisplay->color444(0, 0, 0));
    vDisplay->drawDisplayTest();
    vDisplay->flipDMABuffer();

    Serial.println(F("Chain of 4x 64x32 panels for this example:"));
    Serial.println(F("+---------+---------+"));
    Serial.println(F("|    4    |    3    |"));
    Serial.println(F("|         |         |"));
    Serial.println(F("+---------+---------+"));
    Serial.println(F("|    1    |    2    |"));
    Serial.println(F("| (ESP32) |         |"));
    Serial.println(F("+---------+---------+"));

    display_->setTextColor(display_->color565(0, 0, 255));
    display_->setTextSize(3);
    display_->setCursor(0, display_->height() / 2 - 10);
    display_->print("ABCDEFGHI");

    display_->drawRect(1, 1, display_->width() - 2, display_->height() - 2, display_->color565(255, 0, 0));
    display_->drawLine(0, 0, display_->width() - 1, display_->height() - 1, display_->color565(255, 255, 255));
    esp32Display->getVirtualDisplay()->flipDMABuffer();
#endif

    display_->setTextWrap(false);
}

void Application::setupEEPROM() {
    int mode = EEPROM.read(EEPROM_MODE_ADDR);

    if (mode == 40 && counterMode_) {
#ifdef ESP32
        counterMode_->setCounter(EEPROM.readUShort(EEPROM_COUNTER_ADDR));
#else
        uint8_t lowByte = EEPROM.read(EEPROM_COUNTER_ADDR);
        uint8_t highByte = EEPROM.read(EEPROM_COUNTER_ADDR + 1);
        counterMode_->setCounter((highByte << 8) | lowByte);
#endif
    }

    if (mode > 0 && mode < 64) {
        modeManager_.setMode(mode);
    }
}

void Application::createModes() {
    ellipseRenderer_ = new EllipseRenderer(display_);

    scoreboardMode_ = new ScoreboardMode(display_, &timeService_);
    clockMode1_ = new ClockMode(display_, &timeService_, 1);
    clockMode2_ = new ClockMode(display_, &timeService_, 2);
    clockMode3_ = new ClockMode(display_, &timeService_, 3);
    clockMode4_ = new ClockMode(display_, &timeService_, 4);
    snowMode_ = new SnowMode(display_, &timeService_, false);
    coloredSnowMode_ = new SnowMode(display_, &timeService_, true);
    plasmaMode_ = new PlasmaMode(display_, &timeService_);
    gameOfLifeMode_ = new GameOfLifeMode(display_, &timeService_);
    ellipseMode_ = new EllipseMode(display_, &timeService_, ellipseRenderer_);
    starWarsMode_ = new StarWarsMode(display_, &timeService_);
    mandelbrotMode_ = new MandelbrotMode(display_, &timeService_);
    timerMode_ = new TimerMode(display_);
    counterMode_ = new CounterMode(display_, &timeService_, ellipseRenderer_);

    modeManager_.registerMode(1, scoreboardMode_);
    modeManager_.registerMode(2, clockMode1_);
    modeManager_.registerMode(3, clockMode2_);
    modeManager_.registerMode(4, clockMode3_);
    modeManager_.registerMode(5, clockMode4_);
    modeManager_.registerMode(6, snowMode_);
    modeManager_.registerMode(7, plasmaMode_);
    modeManager_.registerMode(8, coloredSnowMode_);
    modeManager_.registerMode(9, gameOfLifeMode_);
    modeManager_.registerMode(11, ellipseMode_);
    modeManager_.registerMode(12, starWarsMode_);
    modeManager_.registerMode(30, timerMode_);
    modeManager_.registerMode(40, counterMode_);
    modeManager_.registerMode(60, mandelbrotMode_);
}

void Application::setup() {
    Serial.begin(115200);
    Serial.println(F("Booting"));

    EEPROM.begin(EEPROM_SIZE);

    setupDisplay();
    Serial.println(F("Display initialized"));

    createModes();

    ArduinoOTA.setHostname("LedMatrix4x4");

    wifiManager_ = new WifiManager(display_);
    if (!wifiManager_->setup()) {
        // In setup mode - set scrolling text to instruct user
        strcpy(scoreboardMode_->getScrollingText(),
               "Connect to LedMatrix-Setup and go to 192.168.4.1 to setup WiFi");
    }

    alexa_ = new AlexaIntegration(&modeManager_, display_);
    alexa_->setup();

    mqttManager_ = new MqttManager(&modeManager_, display_);
    mqttManager_->setup();

    // Set mode change callback to publish to MQTT
    modeManager_.setModeChangeCallback([](int newMode) {
        // Mode change notification handled in MQTT periodic update
    });

    cmdExecutor_ = new CommandExecutor(&modeManager_, scoreboardMode_,
                                        timerMode_, counterMode_, mandelbrotMode_,
                                        snowMode_, coloredSnowMode_, gameOfLifeMode_,
                                        display_);

    udpHandler_ = new UdpHandler(cmdExecutor_, display_);
    udpHandler_->begin();

    timeService_.begin();

    setupEEPROM();

    // Log memory
#ifdef ESP32
    log_d("Total heap: %d", ESP.getHeapSize());
    log_d("Free heap: %d", ESP.getFreeHeap());
    log_d("Min Free heap: %d", ESP.getMinFreeHeap());
#else
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
#endif

    Serial.println(F("Setup complete"));
}

void Application::processEvents(unsigned long ms) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        yield();
        alexa_->handle();
        mqttManager_->loop();
        ArduinoOTA.handle();
        wifiManager_->handleClient();
        udpHandler_->receive();
    }
}

void Application::loop() {
    // Sync off state from subsystems
    if (cmdExecutor_->isOff() || alexa_->isOff() || mqttManager_->isOff()) {
        off_ = true;
    }
    if (!cmdExecutor_->isOff() && !alexa_->isOff() && !mqttManager_->isOff()) {
        off_ = false;
    }

#ifdef ESP8266
    g_displayDrawTime = displayDrawTime_;
#endif

    if (wifiManager_->isInSetupMode()) {
        display_->showBuffer();
        display_->clearScreen();
        scoreboardMode_->update(); // shows scrolling text
        wifiManager_->handleClient();
        processEvents(10);
        return;
    }

    if (!off_) {
        modeManager_.update();
        processEvents(modeManager_.getFrameDelay());
    } else {
        processEvents(20);
    }

    // Check MQTT reconnection
    if (wifiManager_->isMqttReconnectNeeded()) {
        Serial.println("MQTT config changed, reconnecting...");
        mqttManager_->setup();
        wifiManager_->clearMqttReconnectFlag();
    }
}
