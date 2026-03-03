#ifndef SCOREBOARD_MODE_H
#define SCOREBOARD_MODE_H

#include "IDisplayMode.h"
#include "../display/IDisplay.h"
#include "../time/TimeService.h"
#include "../util/Colors.h"
#include "../app/Config.h"

class ScoreboardMode : public IDisplayMode {
public:
    ScoreboardMode(IDisplay* display, TimeService* timeService);

    void init() override;
    void update() override;
    void cleanup() override;
    uint16_t frameDelayMs() const override { return 1; }

    // Score setters
    void setPointsLeft(int pts) { pointsLeft_ = pts; }
    void setPointsRight(int pts) { pointsRight_ = pts; }
    void setSetsLeft(int sets) { setsLeft_ = sets; }
    void setSetsRight(int sets) { setsRight_ = sets; }
    void setTeamLeftServes(bool serves) { teamLeftServes_ = serves; }

    int getPointsLeft() const { return pointsLeft_; }
    int getPointsRight() const { return pointsRight_; }
    int getSetsLeft() const { return setsLeft_; }
    int getSetsRight() const { return setsRight_; }
    bool getTeamLeftServes() const { return teamLeftServes_; }

    void updateScore(const char* packet);
    void updateScrollingText(const char* packet);
    void startTimeout();

    char* getScrollingText() { return scrollingText_; }

private:
    void showScore();
    void showTime();
    void showScrollingText();
    void showTimeout();

    IDisplay* display_;
    TimeService* timeService_;

    int pointsLeft_;
    int pointsRight_;
    int setsLeft_;
    int setsRight_;
    bool teamLeftServes_;
    char scrollingText_[100];
    float scrollX_;

    unsigned long timeoutStart_;
    bool timeoutOn_;
};

#endif // SCOREBOARD_MODE_H
