#ifndef STAR_WARS_MODE_H
#define STAR_WARS_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"
#include "../app/Config.h"

struct star_t {
    float x, y;
    float z;
    float vz;
    uint8_t brightness;
};

class StarWarsMode : public IDisplayMode {
public:
    StarWarsMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 20; }

private:
    IDisplay* display_;
    TimeOverlay timeOverlay_;

    star_t* stars_;
    bool initialized_;
    static const int NUM_STARS = 500;
};

#endif // STAR_WARS_MODE_H
