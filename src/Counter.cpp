#include <Counter.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>

#ifdef ESP8266
Counter::Counter(PxMATRIX *display)
#define TEXTSIZE 2
#endif
#ifdef ESP32
#define TEXTSIZE 4
Counter::Counter(VirtualMatrixPanel *display)
#endif
{
    this->display = display;
}

Counter::~Counter()
{
}

void Counter::setCounter(u_int16_t counter) {
    this->counter = counter;
}

void Counter::start() {
    this->counter = 0;
}

int countDigits(int num) {
    if (num == 0) return 0;
    return 1 + countDigits(num / 10);
}

void Counter::show() {
    clear();
    display->setTextSize(TEXTSIZE);
    display->setTextColor(myGREEN);
    display->setCursor(13,14);
    display->printf("%04u", this->counter);
    showBuffer();
}
