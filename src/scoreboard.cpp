#define PxMATRIX_double_buffer true
#define double_buffer

#include <Globals.h>
#include <scoreboard.h>
#include <Fonts/Picopixel.h>
#include <Time.h>

void Scoreboard::showScrollingText() {
  static int textX = 0;
  display->setTextSize(1);
  display->setTextColor(myGREEN);
  display->setCursor(65 - textX++, 25);
  int size = display->print(scrollingText)*6 + 64;
  if (textX > size) {
    textX = 0;
  }
}

void Scoreboard::showScore() {
  display->setTextSize(2);
  display->setCursor(3,0);
  display->setTextColor(myWHITE);
  display->printf("%02d:%02d", pointsLeft, pointsRight);

  display->setCursor(0, 15);
  display->setTextSize(1);
  display->setTextColor(myRED);
  display->print(setsLeft);
  display->setCursor(59, 15);
  display->print(setsRight);

  if (teamLeftServes) {
    display->fillCircle(11, 18, 3, myYELLOW); 
  } else {
    display->fillCircle(51, 18, 3, myYELLOW); 
  }
}

void Scoreboard::showTime() {
  timeClient->update();
  time_t utc = timeClient->getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);

  display->setTextSize(1);
  display->setFont(&Picopixel);
  display->setTextColor(myMAGENTA);
  display->setCursor(18, 20);
  display->printf("%02d:%02d:%02d", hour(local), minute(local), second(local));
  display->setFont();
}

void Scoreboard::timeOut() {
  int secs = millis() - timeoutStart;
  if (secs > 30000) {
    timeoutOn = false;
  }
  int width = secs/30000.0 * 64;
  display->fillRect(0, 24, width,8, timeoutColor);

  display->setTextColor(myOrange);
  display->setCursor(4, 25);
  display->printf("T-Out:%ds", 30-(secs/1000));
}

void Scoreboard::updateScore() {
  pointsLeft = incomingPacket[12];
  pointsRight = incomingPacket[13];
  setsLeft = incomingPacket[14];
  setsRight = incomingPacket[15];
  teamLeftServes = incomingPacket[16];
}

void Scoreboard::updateScrollingText() {
  int j = 0;
  for (int i = 11; incomingPacket[i] != 0; i++) {
    scrollingText[j++] = incomingPacket[i];
  }
  scrollingText[j] = 0;
}

void Scoreboard::show() {
  display->clearDisplay();
  showScore();
  if (timeoutOn) 
    timeOut();
  else
    showScrollingText();
  showTime();
  display->showBuffer();
}
