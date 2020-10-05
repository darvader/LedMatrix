#include <NTPClient.h>
#include <Globals.h>

class Scoreboard
{
private:
    /* data */
public:
    NTPClient *timeClient;
    PxMATRIX *display;
    int pointsLeft = 0;
    int pointsRight = 0;
    int setsLeft = 0;
    int setsRight = 0;
    boolean teamLeftServes = false;
    char timeout = 0;  // 0 no time-out; 1 time-out left; 2 time-out right
    int timeoutStart = 0;
    boolean timeoutOn = false;
    Scoreboard(NTPClient *timeClient, PxMATRIX *display) {
        this->timeClient=timeClient;
        this->display = display;
    } 

    void showScrollingText();
    void show();
    void showScore();
    void showTime();
    void timeOut();
    void updateScore();
    void updateScrollingText();
};
