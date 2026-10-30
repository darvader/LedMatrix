#ifndef HALLOWEEN_MODE_H
#define HALLOWEEN_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../util/TimeOverlay.h"

class HalloweenMode : public IDisplayMode {
public:
    HalloweenMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

private:
    struct Ghost {
        float x, baseY, speed, phase;
    };

    struct Bat {
        float x, baseY, speed, phase, drift;
        bool active;
    };

    struct Star {
        int16_t x, y;
        float phase;
    };

    struct FogWisp {
        float x, phase;
        uint8_t length;
    };

    enum class SpiderState { Descending, Waiting, Ascending, Hidden };
    enum class Encounter { None, Witch, Chase };

    int16_t clockTop() const;
    void resetGhost(Ghost& ghost, bool initial);
    void drawGhost(const Ghost& ghost, uint8_t index);
    void drawBat(const Bat& bat);
    void updateBats(uint32_t elapsedMs);
    void updateFog(uint32_t elapsedMs);
    void updateEncounter(uint32_t elapsedMs);
    void drawWitch();
    void drawSky();
    void startSpider();
    void updateSpider(uint32_t elapsedMs);
    void drawEyes();
    void drawPumpkins();
    void drawPoint(int16_t x, int16_t y, uint16_t color);
    void drawMask(const uint16_t* rows, uint8_t width, uint8_t height,
                  int16_t x, int16_t y, uint16_t color, uint8_t scale = 1,
                  bool mirror = false);

    IDisplay* display_;
    TimeOverlay timeOverlay_;
    Ghost ghosts_[8];
    Bat bats_[3];
    Star stars_[32];
    FogWisp fog_[6];
    uint8_t ghostCount_;
    uint8_t starCount_;
    uint8_t fogCount_;
    uint8_t scale_;
    bool initialized_;
    uint32_t lastUpdateMs_;
    float scenePhase_;
    uint32_t batWaitMs_;
    SpiderState spiderState_;
    int16_t spiderX_, spiderTargetY_;
    float spiderY_, spiderLegPhase_;
    uint32_t spiderWaitMs_;
    Encounter encounter_;
    float encounterX_, encounterY_, encounterSpeed_, encounterPhase_;
    uint32_t encounterWaitMs_;
    bool nextWitch_;
};

#endif // HALLOWEEN_MODE_H
