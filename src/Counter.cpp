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
    freeAllResources();
}

void Counter::freeRunners() {
    if (runners != nullptr) {
        delete[] (runner_t*)runners;
        runners = nullptr;
        initializedRunners = false;
    }
}

void Counter::freeAllResources() {
    freeRunners();
}

void Counter::initializeRunners() {
    const int numRunners = 400;
    runners = new runner_t[numRunners];
    runner_t* runnersArray = (runner_t*)runners;
    
    for (int i = 0; i < numRunners; i++) {
        createRunner(&runnersArray[i]);
    }
    
    initializedRunners = true;
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

void Counter::createRunner(runner_t *runner)
{
    float speed = random(100) * 0.01f + 0.2f;
    runner->speed = speed;
    runner->r = random(255) * speed;
    runner->g = random(255) * speed;
    runner->b = random(255) * speed;
    runner->distance = 0.0f;
    runner->position = 0.0f;
    runner->radius = 0.5f + 0.006f*random(100);
}


void Counter::drawRunners() {
    const int width = 128;
    const int height = 64;
    const int a = 60; // semi-major axis
    const int b = 30; // semi-minor axis
    const int centerX = width / 2;
    const int centerY = height / 2;
    const int numRunners = 400; // number of dots around the ellipse

    if (!initializedRunners) {
        initializeRunners();
    }

    runner_t* runnersArray = (runner_t*)runners;

    for (int i = 0; i < numRunners; i++) {
        runner_t *runner = &runnersArray[i];
        runner->position += (M_PI/1000.0f)*runner->speed;

        float theta = runner->position;
        int x = centerX + a * cos(theta)*runner->radius;
        int y = centerY + b * sin(theta)*runner->radius;
        display->drawPixelRGB888(x, y, runner->r, runner->g, runner->b);

        // after three rounds, create a new runner
        if (runner->position > M_TWOPI * 3) {
            createRunner(runner);
        }
    }
}

int countDigits(int num) {
    if (num == 0) return 0;
    return 1 + countDigits(num / 10);
}

void Counter::show() {
    clear();
    drawRunners();
    display->setTextSize(TEXTSIZE);
    display->setTextColor(myGREEN);
    display->setCursor(13,4);
    display->printf("%04u", this->counter);
    drawTime();
    showBuffer();
}
