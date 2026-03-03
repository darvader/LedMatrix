#ifndef ELLIPSE_MODE_H
#define ELLIPSE_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/TimeOverlay.h"
#include "../util/EllipseRenderer.h"

class EllipseMode : public IDisplayMode {
public:
    EllipseMode(IDisplay* display, TimeService* timeService, EllipseRenderer* ellipseRenderer);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 1; }

private:
    IDisplay* display_;
    TimeOverlay timeOverlay_;
    EllipseRenderer* ellipseRenderer_;
};

#endif // ELLIPSE_MODE_H
