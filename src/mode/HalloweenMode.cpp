#include "HalloweenMode.h"
#include <math.h>

namespace {
constexpr float TWO_PI_F = 6.2831853f;
constexpr uint8_t GHOST_WIDTH = 7;
constexpr uint8_t GHOST_HEIGHT = 8;
constexpr uint8_t BAT_WIDTH = 7;
constexpr uint8_t BAT_HEIGHT = 4;
constexpr uint8_t SPIDER_WIDTH = 9;
constexpr uint8_t SPIDER_HEIGHT = 7;
constexpr uint8_t WITCH_WIDTH = 16;
constexpr uint8_t WITCH_HEIGHT = 10;

// Native-size sprites keep the flying characters small on both panel sizes.
// Each row is a bit mask, with the leftmost pixel in the highest used bit.
const uint16_t GHOST_BODY[2][GHOST_HEIGHT] = {
    {0x1c, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x55},
    {0x1c, 0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x2a}
};
const uint16_t GHOST_FACE[2][GHOST_HEIGHT] = {
    {0, 0, 0x14, 0x14, 0, 0x08, 0, 0},
    {0, 0, 0, 0x14, 0, 0x08, 0, 0}
};
const uint16_t BAT_WINGS[3][BAT_HEIGHT] = {
    {0x41, 0x6b, 0x3e, 0x08},
    {0x00, 0x7f, 0x3e, 0x08},
    {0x08, 0x3e, 0x6b, 0x41}
};
const uint16_t MOON[7] = {0x1e, 0x38, 0x70, 0x70, 0x70, 0x38, 0x1e};
const uint16_t SPIDER[2][SPIDER_HEIGHT] = {
    {0x082, 0x044, 0x139, 0x0fe, 0x139, 0x054, 0x082},
    {0x101, 0x082, 0x07c, 0x1ff, 0x07c, 0x092, 0x101}
};
const uint16_t SPIDER_EYES[SPIDER_HEIGHT] = {0, 0, 0x028, 0, 0, 0, 0};
const uint16_t EYES[1] = {0x12};
const uint16_t PUMPKIN_BODY[8] = {0, 0x07c, 0x0fe, 0x1ff, 0x1ff, 0x1ff, 0x0fe, 0x07c};
const uint16_t PUMPKIN_STEM[8] = {0x010, 0x010, 0, 0, 0, 0, 0, 0};
const uint16_t PUMPKIN_FACE[2][8] = {
    {0, 0, 0, 0x044, 0, 0x092, 0x07c, 0},
    {0, 0, 0, 0x004, 0, 0x092, 0x07c, 0}
};
const uint16_t WITCH_HAT[WITCH_HEIGHT] = {0x0200, 0x0700, 0x0f80, 0x1fc0, 0, 0, 0, 0, 0, 0};
const uint16_t WITCH_FACE[WITCH_HEIGHT] = {0, 0, 0, 0, 0x07c0, 0x0700, 0, 0, 0, 0};
const uint16_t WITCH_DRESS[2][WITCH_HEIGHT] = {
    {0, 0, 0, 0, 0, 0, 0x0f80, 0x1fc0, 0x3fc0, 0x0900},
    {0, 0, 0, 0, 0, 0, 0x0f80, 0x1fc0, 0x1fe0, 0x0480}
};
const uint16_t WITCH_BROOM[WITCH_HEIGHT] = {0, 0, 0, 0, 0, 0, 0xe000, 0xffff, 0xe000, 0};
const uint16_t WITCH_EYE[WITCH_HEIGHT] = {0, 0, 0, 0, 0x0100, 0, 0, 0, 0, 0};
}

HalloweenMode::HalloweenMode(IDisplay* display, TimeService* timeService)
    : display_(display), timeOverlay_(display, timeService), ghosts_{},
      bats_{}, stars_{}, fog_{}, ghostCount_(0), starCount_(0), fogCount_(0), scale_(1),
      initialized_(false), lastUpdateMs_(0), scenePhase_(0), batWaitMs_(0),
      spiderState_(SpiderState::Hidden), spiderX_(0), spiderTargetY_(0),
      spiderY_(0), spiderLegPhase_(0), spiderWaitMs_(0),
      encounter_(Encounter::None), encounterX_(0), encounterY_(0),
      encounterSpeed_(0), encounterPhase_(0), encounterWaitMs_(0), nextWitch_(true) {}

int16_t HalloweenMode::clockTop() const {
#ifdef ESP32
    return display_->height() / 2 - 5;
#else
    return 10;
#endif
}

void HalloweenMode::init() {
    scale_ = display_->width() >= 128 && display_->height() >= 64 ? 2 : 1;
    ghostCount_ = scale_ == 2 ? 8 : 4;
    starCount_ = scale_ == 2 ? 32 : 12;
    fogCount_ = scale_ == 2 ? 6 : 3;
    scenePhase_ = 0;
    for (uint8_t i = 0; i < ghostCount_; ++i) {
        resetGhost(ghosts_[i], true);
        // Spread the initial ghosts out rather than starting in a clump.
        ghosts_[i].x = i * display_->width() / ghostCount_;
    }
    for (uint8_t i = 0; i < starCount_; ++i) {
        stars_[i].x = random(display_->width());
        stars_[i].y = random(clockTop());
        stars_[i].phase = random(628) / 100.0f;
    }
    for (Bat& bat : bats_) bat.active = false;
    batWaitMs_ = random(1500, 3501);
    startSpider();
    for (uint8_t i = 0; i < fogCount_; ++i) {
        fog_[i].x = random(display_->width());
        fog_[i].phase = random(628) / 100.0f;
        fog_[i].length = random(8, 16) * scale_;
    }
    encounter_ = Encounter::None;
    encounterWaitMs_ = random(8000, 12001);
    nextWitch_ = true;
    lastUpdateMs_ = millis();
    // Clear the old mode in both buffers before the first Halloween frame.
    display_->clearScreen();
    display_->showBuffer();
    display_->clearScreen();
    display_->setFont();
    display_->setTextWrap(false);
    initialized_ = true;
}

void HalloweenMode::cleanup() {
    initialized_ = false;
    ghostCount_ = 0;
    starCount_ = 0;
    fogCount_ = 0;
    for (Bat& bat : bats_) bat.active = false;
    batWaitMs_ = 0;
    spiderState_ = SpiderState::Hidden;
    spiderWaitMs_ = 0;
    encounter_ = Encounter::None;
    encounterWaitMs_ = 0;
}

void HalloweenMode::resetGhost(Ghost& ghost, bool initial) {
    ghost.speed = (random(2) == 0 ? 1 : -1) * random(5, 12) * (scale_ == 2 ? 1.5f : 1.0f);
    ghost.x = initial ? 0 : (ghost.speed > 0 ? -GHOST_WIDTH : display_->width());
    // Use the full height, leaving room for bobbing at the display edges.
    ghost.baseY = random(1, display_->height() - GHOST_HEIGHT);
    ghost.phase = random(628) / 100.0f;
}

void HalloweenMode::update() {
    if (!initialized_) init();

    const uint32_t now = millis();
    uint32_t elapsedMs = now - lastUpdateMs_;
    lastUpdateMs_ = now;
    // Network activity or switching the display off must not cause large jumps.
    if (elapsedMs > 100) elapsedMs = 100;
    const float seconds = elapsedMs / 1000.0f;
    scenePhase_ = fmodf(scenePhase_ + seconds, TWO_PI_F);

    display_->showBuffer();
    display_->clearScreen();
    drawSky();
    updateFog(elapsedMs);
    drawEyes();
    for (uint8_t i = 0; i < ghostCount_; ++i) {
        Ghost& ghost = ghosts_[i];
        ghost.x += ghost.speed * seconds;
        ghost.phase = fmodf(ghost.phase + seconds * (1.5f + i * 0.17f), TWO_PI_F);
        if (ghost.x > display_->width() || ghost.x < -GHOST_WIDTH) {
            resetGhost(ghost, false);
        }
        drawGhost(ghost, i);
    }
    updateBats(elapsedMs);
    updateEncounter(elapsedMs);
    updateSpider(elapsedMs);
    drawPumpkins();
    timeOverlay_.drawTimeWithBackground();
}

void HalloweenMode::drawPoint(int16_t x, int16_t y, uint16_t color) {
    if (x >= 0 && x < display_->width() && y >= 0 && y < display_->height()) {
        display_->drawPixel(x, y, color);
    }
}

void HalloweenMode::drawMask(const uint16_t* rows, uint8_t width, uint8_t height,
                             int16_t x, int16_t y, uint16_t color, uint8_t scale,
                             bool mirror) {
    for (uint8_t row = 0; row < height; ++row) {
        for (uint8_t col = 0; col < width; ++col) {
            if ((rows[row] & (1U << (width - col - 1))) == 0) continue;
            const uint8_t pixelCol = mirror ? width - col - 1 : col;
            for (uint8_t dy = 0; dy < scale; ++dy) {
                for (uint8_t dx = 0; dx < scale; ++dx) {
                    drawPoint(x + pixelCol * scale + dx, y + row * scale + dy, color);
                }
            }
        }
    }
}

void HalloweenMode::updateFog(uint32_t elapsedMs) {
    const float seconds = elapsedMs / 1000.0f;
    for (uint8_t i = 0; i < fogCount_; ++i) {
        FogWisp& wisp = fog_[i];
        wisp.x += seconds * (i % 2 == 0 ? 1 : -1) * (1.5f + (i % 3) * 0.5f) * scale_;
        if (wisp.x > display_->width()) wisp.x = -wisp.length;
        if (wisp.x < -wisp.length) wisp.x = display_->width();
        const int16_t baseY = display_->height() - (2 + (i % 3) * 2) * scale_;
        for (uint8_t col = 0; col < wisp.length; ++col) {
            int distance = 2 * col - wisp.length;
            if (distance < 0) distance = -distance;
            const uint8_t glow = 8 + (wisp.length - distance) * 20 / wisp.length;
            const int16_t x = static_cast<int16_t>(floorf(wisp.x)) + col;
            const int16_t y = baseY + static_cast<int16_t>(roundf(sinf(scenePhase_ + wisp.phase + col * 0.3f) * scale_));
            drawPoint(x, y, display_->color565(glow / 2, glow / 4, glow));
            const uint16_t edge = display_->color565(glow / 4, glow / 8, glow / 2);
            drawPoint(x, y - 1, edge);
            drawPoint(x, y + 1, edge);
        }
    }
}

void HalloweenMode::updateEncounter(uint32_t elapsedMs) {
    if (encounter_ == Encounter::None) {
        if (elapsedMs < encounterWaitMs_) {
            encounterWaitMs_ -= elapsedMs;
            return;
        }
        // Alternate the two little scenes, with quiet intervals between them.
        encounter_ = nextWitch_ ? Encounter::Witch : Encounter::Chase;
        const int direction = random(2) == 0 ? 1 : -1;
        encounterSpeed_ = direction * (scale_ == 2 ? 28.0f : 20.0f);
        encounterX_ = direction > 0 ? -WITCH_WIDTH : display_->width();
        encounterPhase_ = 0;
        if (encounter_ == Encounter::Witch) {
            // The hat and broom stay visible above the clock on the small panel.
            encounterY_ = scale_ == 2 ? random(2, clockTop() - WITCH_HEIGHT - 1) : 1;
        } else {
            encounterY_ = random(3, display_->height() - GHOST_HEIGHT - 2);
        }
    }

    const float seconds = elapsedMs / 1000.0f;
    encounterX_ += encounterSpeed_ * seconds;
    encounterPhase_ = fmodf(encounterPhase_ + seconds * 3.0f, TWO_PI_F);
    const int margin = encounter_ == Encounter::Witch ? WITCH_WIDTH : 24;
    if ((encounterSpeed_ > 0 && encounterX_ > display_->width() + margin) ||
        (encounterSpeed_ < 0 && encounterX_ < -margin)) {
        encounter_ = Encounter::None;
        nextWitch_ = !nextWitch_;
        encounterWaitMs_ = random(10000, 18001);
        return;
    }

    if (encounter_ == Encounter::Witch) {
        drawWitch();
    } else {
        const int direction = encounterSpeed_ > 0 ? 1 : -1;
        // The gap contracts and opens again, as if the ghost almost catches up.
        const float gap = 11.0f + 3.0f * sinf(encounterPhase_);
        Ghost chaser{encounterX_ - direction * gap, encounterY_ - 2,
                     encounterSpeed_, encounterPhase_ - 0.8f};
        Bat leader{encounterX_, encounterY_, encounterSpeed_,
                   fmodf(encounterPhase_ * 4, TWO_PI_F), encounterPhase_, true};
        drawGhost(chaser, 0);
        drawBat(leader);
    }
}

void HalloweenMode::drawWitch() {
    const int16_t x = static_cast<int16_t>(floorf(encounterX_));
    const int16_t y = static_cast<int16_t>(roundf(encounterY_ + sinf(encounterPhase_)));
    const bool mirror = encounterSpeed_ < 0;
    const uint8_t flutter = sinf(encounterPhase_ * 2) >= 0 ? 0 : 1;
    drawMask(WITCH_BROOM, WITCH_WIDTH, WITCH_HEIGHT, x, y, display_->color565(150, 85, 20), 1, mirror);
    drawMask(WITCH_DRESS[flutter], WITCH_WIDTH, WITCH_HEIGHT, x, y, display_->color565(100, 30, 135), 1, mirror);
    drawMask(WITCH_FACE, WITCH_WIDTH, WITCH_HEIGHT, x, y, display_->color565(80, 175, 50), 1, mirror);
    drawMask(WITCH_HAT, WITCH_WIDTH, WITCH_HEIGHT, x, y, display_->color565(115, 45, 175), 1, mirror);
    drawMask(WITCH_EYE, WITCH_WIDTH, WITCH_HEIGHT, x, y, display_->color565(240, 180, 60), 1, mirror);
    for (uint8_t i = 0; i < 3; ++i) {
        const int16_t sparkX = mirror ? x + WITCH_WIDTH + 2 + i * 4 : x - 2 - i * 4;
        const int16_t sparkY = y + 7 + static_cast<int16_t>(roundf(sinf(encounterPhase_ * 2 + i)));
        const uint8_t glow = 60 - i * 15;
        drawPoint(sparkX, sparkY, display_->color565(glow, glow / 2, 5));
    }
}

void HalloweenMode::drawSky() {
    for (uint8_t i = 0; i < starCount_; ++i) {
        const Star& star = stars_[i];
        // Short bright peaks at different rates make individual stars sparkle.
        float twinkle = 0.5f + 0.5f * sinf(scenePhase_ * (2 + i % 3) + star.phase);
        twinkle *= twinkle;
        twinkle *= twinkle;
        const uint8_t glow = static_cast<uint8_t>(35 + 220 * twinkle);
        drawPoint(star.x, star.y, display_->color565(glow * 3 / 4, glow * 7 / 8, glow));
        if ((scale_ == 2 || i % 4 == 0) && glow > 190) {
            const uint8_t haloGlow = (glow - 180) * 2 / 3;
            const uint16_t halo = display_->color565(haloGlow * 3 / 4, haloGlow * 7 / 8, haloGlow);
            drawPoint(star.x - 1, star.y, halo);
            drawPoint(star.x + 1, star.y, halo);
            drawPoint(star.x, star.y - 1, halo);
            drawPoint(star.x, star.y + 1, halo);
        }
    }
    drawMask(MOON, 7, 7, display_->width() - 8 * scale_, scale_,
             display_->color565(155, 120, 35), scale_);
}

void HalloweenMode::startSpider() {
    // Pick a new horizontal position only while the entire spider is off-screen.
    spiderX_ = random(display_->width() - SPIDER_WIDTH * scale_ + 1);
    spiderY_ = -SPIDER_HEIGHT * scale_;
    spiderTargetY_ = random(display_->height() / 4,
                            display_->height() - SPIDER_HEIGHT * scale_ + 1);
    spiderLegPhase_ = 0;
    spiderWaitMs_ = 0;
    spiderState_ = SpiderState::Descending;
}

void HalloweenMode::updateSpider(uint32_t elapsedMs) {
    const float seconds = elapsedMs / 1000.0f;
    switch (spiderState_) {
        case SpiderState::Hidden:
            if (elapsedMs < spiderWaitMs_) {
                spiderWaitMs_ -= elapsedMs;
                return;
            }
            startSpider();
            break;
        case SpiderState::Descending:
            spiderY_ += seconds * 3.5f * scale_;
            if (spiderY_ >= spiderTargetY_) {
                spiderY_ = spiderTargetY_;
                spiderState_ = SpiderState::Waiting;
                spiderWaitMs_ = random(1000, 2201);
            }
            break;
        case SpiderState::Waiting:
            if (elapsedMs < spiderWaitMs_) {
                spiderWaitMs_ -= elapsedMs;
            } else {
                spiderState_ = SpiderState::Ascending;
            }
            break;
        case SpiderState::Ascending:
            spiderY_ -= seconds * 4.5f * scale_;
            if (spiderY_ <= -SPIDER_HEIGHT * scale_) {
                spiderY_ = -SPIDER_HEIGHT * scale_;
                spiderState_ = SpiderState::Hidden;
                spiderWaitMs_ = random(2000, 5001);
                return;
            }
            break;
    }

    // Alternate the leg poses while climbing; keep a slower twitch when hanging.
    const float legSpeed = spiderState_ == SpiderState::Waiting ? 3.0f : 10.0f;
    spiderLegPhase_ = fmodf(spiderLegPhase_ + seconds * legSpeed, TWO_PI_F);
    const uint8_t frame = sinf(spiderLegPhase_) >= 0 ? 0 : 1;
    const int16_t y = static_cast<int16_t>(floorf(spiderY_));
    const uint16_t thread = display_->color565(30, 20, 38);
    for (int16_t sy = 0; sy < y + 2 * scale_; ++sy) {
        drawPoint(spiderX_ + 4 * scale_, sy, thread);
    }
    drawMask(SPIDER[frame], SPIDER_WIDTH, SPIDER_HEIGHT, spiderX_, y,
             display_->color565(135, 45, 100), scale_);
    drawMask(SPIDER_EYES, SPIDER_WIDTH, SPIDER_HEIGHT, spiderX_, y,
             display_->color565(240, 170, 70), scale_);
}

void HalloweenMode::drawEyes() {
    const uint8_t count = scale_ == 2 ? 2 : 1;
    for (uint8_t i = 0; i < count; ++i) {
        const float phase = fmodf(scenePhase_ + i * 2.3f, TWO_PI_F);
        // A short blink interrupts each appearance before the eyes vanish again.
        if (phase < 1.0f || phase > 2.6f || (phase > 1.7f && phase < 1.85f)) continue;
        const float fade = sinf((phase - 1.0f) * PI / 1.6f);
        const uint8_t glow = static_cast<uint8_t>(140 * fade);
        const int16_t x = (i + 1) * display_->width() / (count + 1) - 2 * scale_;
        drawMask(EYES, 5, 1, x, display_->height() - 3 * scale_,
                 display_->color565(glow, glow / 2, 0), scale_);
    }
}

void HalloweenMode::drawGhost(const Ghost& ghost, uint8_t index) {
    const int16_t x = static_cast<int16_t>(floorf(ghost.x));
    const int16_t y = static_cast<int16_t>(roundf(ghost.baseY + sinf(ghost.phase)));
    const uint8_t frame = sinf(ghost.phase * 2) >= 0 ? 0 : 1;
    const uint8_t blink = sinf(ghost.phase * 3) > 0.96f ? 1 : 0;
    const float fade = 0.7f + 0.3f * sinf(ghost.phase);
    const uint16_t color = index % 2 == 0 ? display_->color565(210 * fade, 225 * fade, 240 * fade)
                                        : display_->color565(120 * fade, 190 * fade, 230 * fade);
    drawMask(GHOST_BODY[frame], GHOST_WIDTH, GHOST_HEIGHT, x, y, color);
    drawMask(GHOST_FACE[blink], GHOST_WIDTH, GHOST_HEIGHT, x, y, 0);
}

void HalloweenMode::updateBats(uint32_t elapsedMs) {
    bool flying = false;
    for (const Bat& bat : bats_) flying = flying || bat.active;
    if (!flying) {
        if (elapsedMs < batWaitMs_) {
            batWaitMs_ -= elapsedMs;
            return;
        }
        const uint8_t count = scale_ == 2 ? 3 : 2;
        const float speed = (random(2) == 0 ? 1 : -1) * (scale_ == 2 ? 25.0f : 18.0f);
        for (uint8_t i = 0; i < count; ++i) {
            Bat& bat = bats_[i];
            bat.active = true;
            bat.speed = speed;
            bat.x = speed > 0 ? -BAT_WIDTH - i * 10 : display_->width() + i * 10;
            bat.baseY = random(2, display_->height() - BAT_HEIGHT - 1);
            bat.phase = i * 1.7f;
            bat.drift = random(628) / 100.0f;
        }
        batWaitMs_ = random(3000, 6501);
    }

    const float seconds = elapsedMs / 1000.0f;
    for (Bat& bat : bats_) {
        if (!bat.active) continue;
        bat.x += bat.speed * seconds;
        bat.phase = fmodf(bat.phase + seconds * 12.0f, TWO_PI_F);
        bat.drift = fmodf(bat.drift + seconds * 2.0f, TWO_PI_F);
        // Trailing bats start off-screen; retire only beyond the far edge.
        if ((bat.speed > 0 && bat.x > display_->width()) ||
            (bat.speed < 0 && bat.x < -BAT_WIDTH)) {
            bat.active = false;
            continue;
        }
        drawBat(bat);
    }
}

void HalloweenMode::drawBat(const Bat& bat) {
    const uint8_t frame = static_cast<uint8_t>(bat.phase * 4 / TWO_PI_F);
    const uint8_t wing = frame == 3 ? 1 : frame;
    const int16_t y = static_cast<int16_t>(roundf(bat.baseY + sinf(bat.drift) * 1.5f));
    drawMask(BAT_WINGS[wing], BAT_WIDTH, BAT_HEIGHT,
             static_cast<int16_t>(floorf(bat.x)), y, display_->color565(150, 55, 210));
}

void HalloweenMode::drawPumpkins() {
    const uint8_t count = scale_ == 2 ? 3 : 2;
    const int16_t y = display_->height() - 8 * scale_;
    for (uint8_t i = 0; i < count; ++i) {
        const int16_t x = scale_ + i * (display_->width() - 11 * scale_) / (count - 1);
        const float phase = scenePhase_ + i * 2.1f;
        const uint8_t glow = static_cast<uint8_t>(180 + 35 * sinf(phase * 3) + 20 * sinf(phase * 7));
        const uint8_t wink = sinf(phase * 2) > 0.92f ? 1 : 0;
        drawMask(PUMPKIN_BODY, 9, 8, x, y, display_->color565(190, 55, 0), scale_);
        drawMask(PUMPKIN_STEM, 9, 8, x, y, display_->color565(40, 100, 10), scale_);
        drawMask(PUMPKIN_FACE[wink], 9, 8, x, y, display_->color565(glow, glow * 3 / 4, 0), scale_);
    }
}
