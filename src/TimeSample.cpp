#define PxMATRIX_double_buffer true
#define double_buffer

#include <TimeSample.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>
#include <Time.h>

TimeSample::TimeSample(PxMATRIX *display, NTPClient *timeClient)
{
    this->display = display;
    this->timeClient = timeClient;
  setupTime();
}

TimeSample::~TimeSample()
{
}

struct line_t {
  uint16_t color;
  int16_t x1, x2, y1, y2;
  double arc;
  int degree;
};

enum Snow { small, medium, large };
struct snow_t {
  Snow type;
  float x, y;
  float speed;
};

void TimeSample::drawTimeWithBackground() {
  display->fillRect(9, 10, 49, 9, myBLACK);
  drawTime();
}

void inline TimeSample::drawTime() {
  timeClient->update();
  time_t utc = timeClient->getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);


  display->setTextSize(1);
  display->setCursor(10,11);
  display->setTextColor(myMAGENTA);
  display->printf("%02d:%02d:%02d", hour(local), minute(local), second(local));
}

void TimeSample::timeSample1() {
  static int x = 0;
  static int y = 0;
  static int stepX = 1;
  static int stepY = 1;

  if (x == 63) stepX = -1;
  if (x == 0) stepX = 1;
  if (y == 31) stepY = -1;
  if (y == 0) stepY = 1;

  display->clearDisplay();
  

  display->drawLine(x, 0, 63-x, 31, myRED);
  display->drawLine(63-x, 0, x, 31, myBLUE);
  display->drawLine(0, y, 63, 31-y, myGREEN);
  display->drawLine(0, 31-y, 63, y, myYELLOW);

  drawTimeWithBackground();

  x += stepX;
  y += stepY;
  display->showBuffer();

  // delay(30);
}

void TimeSample::timeSample2() {
  static const int size = 10; 
  static line_t lines[size];
  static boolean initialized = false;
  static byte radius = 35;

  if (!initialized) {
    for (int i = 0; i < size; i++) {
      lines[i].x1 = sin(i * PI/size) * radius + 32;
      lines[i].y1 = cos(i * PI/size) * radius + 16;
      lines[i].x2 = sin(i * PI/size + PI) * radius + 32;
      lines[i].y2 = cos(i * PI/size + PI) * radius + 16;

      lines[i].color = myCOLORS[i % 7];
      lines[i].arc = i * PI/size;
    }
    initialized = true;
  }

  display->clearDisplay();

  for (int i = 0; i<size; i++) {
    display->drawLine(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    lines[i].arc -= PI * 2/180; // 2 degree more

    if (lines[i].arc < 2 * PI) lines[i].arc += 2 * PI;

    double arc = lines[i].arc;
    lines[i].x1 = sin(arc) * radius + 32;
    lines[i].y1 = cos(arc) * radius + 16;
    lines[i].x2 = sin(arc + PI) * radius + 32;
    lines[i].y2 = cos(arc + PI) * radius + 16;
  }
  drawTimeWithBackground();

  display->showBuffer();
}

void TimeSample::timeSample3() {
  static const int size = 20; 
  static line_t lines[size];
  static boolean initialized = false;
  static byte radius = 37;
  static const int step = 360/size; 
  static int xVals[360];
  static int yVals[360];

  if (!initialized) {
    for (int i = 0; i < 360; i++) {
      xVals[i] = sin(i/180.0 * PI) * radius + 32;
      yVals[i] = cos(i/180.0 * PI) * radius + 16;
    }

    for (int i = 0; i < size; i++) {
      lines[i].x1 = xVals[i * step];
      lines[i].y1 = yVals[i * step];
      int step2 = (i + 1) * step;
      if (step2 >= 360 ) step2 -=360; 
      lines[i].x2 = xVals[(i + 1) * step];
      lines[i].y2 = yVals[(i + 1) * step];

      lines[i].color = myCOLORS[i % 7];
      lines[i].degree = i * step;
    }

    initialized = true;
  }

  display->clearDisplay();

  for (int i = 0; i<size; i++) {
    lines[i].degree -= 2; // 2 degree more
    if (lines[i].degree < 0) lines[i].degree += 360;

    display->fillTriangle(32, 16, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    int degree = lines[i].degree;
    lines[i].x1 = xVals[degree];
    lines[i].y1 = yVals[degree];
    int step2 = degree + step;
    if (step2 >= 360) step2 -= 360;
    lines[i].x2 = xVals[step2];
    lines[i].y2 = yVals[step2];
  }
  drawTimeWithBackground();

  display->showBuffer();
}

void TimeSample::timeSample4() {
  timeClient->update();
  static const int size = 30; 
  static line_t lines[size];
  static boolean initialized = false;
  static byte radius = 37;
  static const int step = 360/size; 
  static int xVals[360];
  static int yVals[360];

  if (!initialized) {
    for (int i = 0; i < 360; i++) {
      xVals[i] = sin(i/180.0 * PI) * radius + 32;
      yVals[i] = cos(i/180.0 * PI) * radius + 16;
    }

    for (int i = 0; i < size; i++) {
      lines[i].x1 = xVals[i * step];
      lines[i].y1 = yVals[i * step];
      int step2 = (i + 1) * step;
      if (step2 >= 360 ) step2 -=360; 
      lines[i].x2 = xVals[(i + 1) * step];
      lines[i].y2 = yVals[(i + 1) * step];
      uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536L/size * i));
      lines[i].color = display->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
      lines[i].degree = i * step;
    }

    initialized = true;
  }

  display->clearDisplay();

  for (int i = 0; i<size; i++) {
    lines[i].degree -= 2; // 2 degree more
    if (lines[i].degree < 0) lines[i].degree += 360;

    display->fillTriangle(32, 16, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    int degree = lines[i].degree;
    lines[i].x1 = xVals[degree];
    lines[i].y1 = yVals[degree];
    int step2 = degree + step;
    if (step2 >= 360) step2 -= 360;
    lines[i].x2 = xVals[step2];
    lines[i].y2 = yVals[step2];
  }
  drawTimeWithBackground();

  display->showBuffer();
}

uint16_t static mediumSnow[] = {0x0000, 0xffff, 0x0000,
                                0xffff, 0xffff, 0xffff,
                                0x0000, 0xffff, 0x0000};



void TimeSample::timeSnow() {
  static boolean initialized = false;
  static const int numSmallSnows = 60; 
  static const int numMediumSnows = 10; 
  static const int numLargeSnows = 10; 

  static snow_t smallSnows[numSmallSnows];
  static snow_t mediumSnows[numMediumSnows];
  static snow_t largeSnows[numLargeSnows];
  static snow_t snow;

  if (!initialized) {
    for (int i = 0; i < numSmallSnows; i++) {
      smallSnows[i].type = small;
      smallSnows[i].x = random(63);
      smallSnows[i].y = random(31);
      smallSnows[i].speed = random(3, 10)/10.0f;
    }    

    initialized = true;
  }

  timeClient->update();
  display->clearDisplay();

  for (int i = 0; i < numSmallSnows; i++) {
    display->drawPixelRGB888(smallSnows[i].x, smallSnows[i].y, 0xff*smallSnows[i].speed, 0xff*smallSnows[i].speed, 0xff*smallSnows[i].speed);
    smallSnows[i].y += smallSnows[i].speed;
    if (smallSnows[i].y > 31) {
      smallSnows[i].x = random(63); 
      smallSnows[i].y = 0;
      smallSnows[i].speed = random(3,10)/10.0f;
    }
  }  

  drawTime();
  display->showBuffer();
}
