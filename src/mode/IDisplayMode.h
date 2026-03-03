#ifndef IDISPLAY_MODE_H
#define IDISPLAY_MODE_H

#include <Arduino.h>

class IDisplayMode {
public:
    virtual ~IDisplayMode() = default;
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void cleanup() = 0;
    virtual uint16_t frameDelayMs() const = 0;
};

#endif // IDISPLAY_MODE_H
