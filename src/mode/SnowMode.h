#ifndef SNOW_MODE_H
#define SNOW_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"

enum SnowType { SNOW_SMALL, SNOW_MEDIUM, SNOW_LARGE };

struct snow_t {
    SnowType type;
    float x, y;
    float speed;
    uint8_t r, g, b;
};

class SnowMode : public IDisplayMode {
public:
    SnowMode(IDisplay* display, TimeService* timeService, bool colored);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

    void resetInitialized() { initialized_ = false; }

private:
    void initializeSnow();

    IDisplay* display_;
    TimeService* timeService_;
    TimeOverlay timeOverlay_;
    bool colored_;
    bool initialized_;

    snow_t* particles_;
    int numParticles_;
};

#endif // SNOW_MODE_H
