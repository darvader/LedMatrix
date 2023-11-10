#define PxMATRIX_double_buffer true
#define double_buffer

#include <Globals.h>
#include <scoreboard.h>
#include <Fonts/Picopixel.h>
#include <TimeLedMatrix.h>

void Scoreboard::showScrollingText() {
  static int textX = 0;
  display->setTextSize(1);
  display->setTextColor(myGREEN);
  display->setCursor(matrix_width + 1 - textX++, matrix_height-7);
  int size = display->print(scrollingText)*6 + matrix_width;
  if (textX > size) {
    textX = 0;
  }
}

void Scoreboard::showScore() {
#ifdef ESP32
  display->setTextSize(4);
#elif
  display->setTextSize(2);
#endif

  display->setCursor(3,0);
  display->setTextColor(myWHITE);
  display->printf("%02d:%02d", pointsLeft, pointsRight);

  display->setTextColor(myRED);
#ifdef ESP32
  display->setCursor(2, 30);
  display->setTextSize(2);
  display->print(setsLeft);
  display->setCursor(116, 30);
  display->print(setsRight);
#elif
  display->setCursor(0, 15);
  display->setTextSize(1);
  display->print(setsLeft);
  display->setCursor(120, 30);
  display->print(setsRight);
#endif

#ifdef ESP32
  if (teamLeftServes) {
    display->fillCircle(17, 37, 4, myYELLOW); 
  } else {
    display->fillCircle(110, 37, 4, myYELLOW);
  }
#elif
  if (teamLeftServes) {
    display->fillCircle(11, 18, 3, myYELLOW); 
  } else {
    display->fillCircle(51, 18, 3, myYELLOW); 
  }
#endif

}

void Scoreboard::showTime() {
  timeClient->update();
  time_t utc = timeClient->getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);

  display->setTextSize(1);
  display->setTextColor(myMAGENTA);
#ifdef ESP32
  display->setCursor(40, 32);
#elif
  display->setCursor(18, 20);
  display->setFont(&Picopixel);
#endif

  display->printf("%02d:%02d:%02d", hour(local), minute(local), second(local));
  display->setFont();
}

void Scoreboard::timeOut() {
  int secs = millis() - timeoutStart;
  if (secs > 30000) {
    timeoutOn = false;
  }
  int width = secs/30000.0 * matrix_width;
  display->fillRect(0, matrix_height - 8, width,8, timeoutColor);

  display->setTextSize(1);
  display->setTextColor(myOrange);
#ifdef ESP32
  display->setCursor(40, matrix_height - 7);
#elif
  display->setCursor(4, matrix_height - 7);
#endif

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
  clear();
  showScore();
  if (timeoutOn) 
    timeOut();
  else
    showScrollingText();
  showTime();
  showBuffer();
}
