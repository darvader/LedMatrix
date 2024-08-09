#include <Counter.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>
#include <TimeLedMatrix.h>

#ifdef ESP8266
Counter::Counter(PxMATRIX *display)
#define TEXTSIZE 2
#endif
#ifdef ESP32
#define TEXTSIZE 4
Counter::Counter(VirtualMatrixPanel *display, NTPClient *timeClient)
#endif
{
    this->display = display;
    this->timeClient = timeClient;
    setupTime();
}

Counter::~Counter()
{
}

void inline Counter::drawTime() {
  timeClient->update();
  time_t utc = timeClient->getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);

#ifdef ESP32
  display->setTextSize(1);
  display->setCursor(40, 50);
#else
  display->setTextSize(1);
  display->setCursor(10,11);
#endif
  display->setTextColor(myMAGENTA);
  display->printf("%02d:%02d:%02d", hour(local), minute(local), second(local));
}


void Counter::setCounter(u_int16_t counter) {
    this->counter = counter;
}

void Counter::start() {
    this->counter = 0;
}

void Counter::runningDots() {
    const int width = 128;
    const int height = 64;
    const int a = 60; // semi-major axis
    const int b = 30; // semi-minor axis
    const int centerX = width / 2;
    const int centerY = height / 2;
    const int numDots = 20; // number of dots around the ellipse
    const float step = M_TWOPI / numDots; // step size for each dot
    static float position = 0.0f;

    if (position > M_TWOPI) position = 0.0f;
    position += M_PI/1000.0f;
    for (int i = 0; i < numDots; ++i) {
        float theta = i * step + position;
        int x = centerX + a * cos(theta);
        int y = centerY + b * sin(theta);
        display->drawPixel(x, y, myOrange);
    }

}

int countDigits(int num) {
    if (num == 0) return 0;
    return 1 + countDigits(num / 10);
}

void Counter::show() {
    clear();
    runningDots();
    display->setTextSize(TEXTSIZE);
    display->setTextColor(myGREEN);
    display->setCursor(13,4);
    display->printf("%04u", this->counter);
    drawTime();
    showBuffer();
}
