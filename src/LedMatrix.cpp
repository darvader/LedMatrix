#define PxMATRIX_double_buffer true
#define double_buffer

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <cstring>
#include <WifiUpdate.h>
#include <NTPClient.h>
#include <complex>
#include <Adafruit_NeoPixel.h>
#include <FFTLed.h>
#include <scoreboard.h>
#include <Globals.h>
#include <PxMatrix.h>

using namespace std;
// Creates a second buffer for backround drawing (doubles the required RAM)

// Pins for LED MATRIX
#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif

#define matrix_width 64
#define matrix_height 32

// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display-> If too large the ESP will crash
uint8_t display_draw_time=50; //30-70 is usually fine

// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX displayP = PxMATRIX(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
PxMATRIX *display = &displayP;
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

// Some standard colors
uint16_t myRED = display->color565(255, 0, 0);
uint16_t myGREEN = display->color565(0, 255, 0);
uint16_t myBLUE = display->color565(0, 0, 255);
uint16_t myWHITE = display->color565(255, 255, 255);
uint16_t myYELLOW = display->color565(255, 255, 0);
uint16_t myOrange = display->color565(255, 127, 0);
uint16_t myCYAN = display->color565(0, 255, 255);
uint16_t myMAGENTA = display->color565(255, 0, 255);
uint16_t myBLACK = display->color565(0, 0, 0);
uint16_t timeoutColor = display->color565(0, 50, 50);

uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myOrange,myYELLOW,myCYAN,myMAGENTA,myBLACK};


float zoomMandelbrot = 1.0;

boolean off = false;

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display->display(display_draw_time);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display->display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display->display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


void display_update_enable(bool is_enable)
{

#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
#endif

#ifdef ESP32
  if (is_enable)
  {
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  }
  else
  {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
#endif
}


const int ESP_BUILTIN_LED = 2;
// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
char scrollingText[512] = "VSV Jena 90 e.V. : Gastmannschaft";


WiFiUDP Udp;
WiFiUDP UdpNtp;

const long utcOffsetInSeconds = 60*60*2;
NTPClient *timeClient = new NTPClient(UdpNtp, "pool.ntp.org", utcOffsetInSeconds);
unsigned int localUdpPort = 4210;
char incomingPacket[1024];
char replyPacket[] = "LedMatrix";
IPAddress masterIp;
int mode = 1;
Scoreboard *scoreboard;


void setupUdp();
void receiveUdp();

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

// Define your display layout here, e.g. 1/8 step, and optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)
  display->begin(16);
  display->setFastUpdate(true);
    // Set driver chip type

  display->clearDisplay();
  display->setTextColor(myCYAN);
  display->setTextWrap(false);
  display->setCursor(2,0);
  display->print("Pixel");
  display->setTextColor(myMAGENTA);
  display->setCursor(2,8);
  display->print("Time");
  display_update_enable(true);
  display->setDriverChip(FM6124);
  display->showBuffer();

  setupWifiUpdate();
  setupUdp();
  timeClient->begin();
  scoreboard = new Scoreboard(timeClient, display);
  Serial.printf("Display pointer: %d \n", display);
}


void setupUdp() {
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  // WebSerial.println("Udp configured.");
}


unsigned long last_draw=0;
void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text, uint8_t colorR, uint8_t colorG, uint8_t colorB)
{
    uint16_t text_length = text.length();
    display->setTextWrap(false);  // we don't wrap text so it scrolls nicely
    display->setTextSize(1);
    display->setRotation(0);
    display->setTextColor(display->color565(colorR,colorG,colorB));
      // display->setTextColor(display->color565(colorR,colorG,colorB));

    // Asuming 5 pixel average character width
    for (int xpos=matrix_width; xpos>-(matrix_width+text_length*5); xpos--)
    {
      display->clearDisplay();
      display->setCursor(xpos,ypos);
      display->println(text);
      delay(scroll_delay);
      yield();

      // This might smooth the transition a bit if we go slow
      // display->setTextColor(display->color565(colorR/4,colorG/4,colorB/4));
      // display->setCursor(xpos-1,ypos);
      // display->println(text);

      delay(scroll_delay/5);
      yield();

    }
}

void detect() {
  // send back a reply, to the IP address and port we got the packet from
  masterIp = Udp.remoteIP();
  Udp.beginPacket(masterIp, 4445);
  Udp.write(replyPacket);
  Udp.endPacket();
  Serial.println("Detect received.");
}

void displayOff() {
  off = true;
  display->clearDisplay();
  display->showBuffer();
}

void receiveUdp() {
  int packetSize = Udp.parsePacket();

  if (packetSize)
  {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }

    if (std::strcmp(incomingPacket,"Detect") == 0) {
      detect();
      return;
    }
    if (strstr(incomingPacket,"off") != NULL) {
      displayOff();
      return;
    } else off = false;
    if (std::strcmp(incomingPacket,"remoteFFT") == 0) {
      mode = 10;
      return;
    } else
    if (strstr(incomingPacket,"pointsLeft=") != NULL) {
      scoreboard->pointsLeft = incomingPacket[11];
      return;
    }
    if (strstr(incomingPacket,"pointsRight=") != NULL) {
      scoreboard->pointsRight = incomingPacket[12];
      return;
    }
    if (strstr(incomingPacket,"teamLeftServes=") != NULL) {
      scoreboard->teamLeftServes = incomingPacket[15];
      return;
    }
    if (strstr(incomingPacket,"setsLeft=") != NULL) {
      scoreboard->setsLeft = incomingPacket[9];
      return;
    }
    if (strstr(incomingPacket,"setsRight=") != NULL) {
      scoreboard->setsRight = incomingPacket[10];
      return;
    }
    if (strstr(incomingPacket,"updateScore=") != NULL) {
      scoreboard->updateScore();
      return;
    } 
    if (strstr(incomingPacket,"scrollText=") != NULL) {
      scoreboard->updateScrollingText();
      return;
    }
    if (strstr(incomingPacket,"brightness=") != NULL) {
      display_draw_time = incomingPacket[11];
      return;
    }
    if (std::strcmp(incomingPacket,"timeout") == 0) {
      scoreboard->timeoutStart = millis();
      scoreboard->timeoutOn = true;
      return;
    }
    if (std::strcmp(incomingPacket,"time1") == 0) {
      mode = 2;
      return;
    }
    if (std::strcmp(incomingPacket,"time2") == 0) {
      mode = 3;
      return;
    }
    if (std::strcmp(incomingPacket,"time3") == 0) {
      mode = 4;
      return;
    }
    if (std::strcmp(incomingPacket,"time4") == 0) {
      mode = 5;
      return;
    }
    if (std::strcmp(incomingPacket,"scoreboard") == 0) {
      mode = 1;
      return;
    }
    if (std::strcmp(incomingPacket,"mandelbrot") == 0) {
      mode = 6;
      zoomMandelbrot = 1.0;
      return;
    }
    if (mode == 10) {
      for (int i = 0; i <= 128; i++) {
        vReal[i] = (double)incomingPacket[i];
      }
      ledFFT(display); 

      return;
    } 


    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

struct line_t {
  uint16_t color;
  int16_t x1, x2, y1, y2;
  double arc;
  int degree;
};

void drawTimeWithBackground() {
  display->fillRect(9, 10, 49, 9, myBLACK);
  display->setTextSize(1);
  display->setCursor(10,11);
  display->setTextColor(myMAGENTA);
  display->printf("%02d:%02d:%02d", timeClient->getHours(), timeClient->getMinutes(), timeClient->getSeconds());
}

void timeSample1() {
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

void timeSample2() {
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

void timeSample3() {
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

void timeSample4() {
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

int value (int x, int y, float zoom)  {
    complex<float> point((float)x/30-1.5, (float)y/30-0.5);
    point *= zoom;
    complex<float> z(0, 0);
    int nb_iter = 0;
    while (abs (z) < 2 && nb_iter <= 20) {
        z = z * z + point;
        nb_iter++;
    }
    if (nb_iter < 20)
       return (255*nb_iter)/20;
    else 
       return 0;
}

void mandelbrot() {
  display->clearDisplay();
  zoomMandelbrot *= 0.99;
  for (int x = 0; x<matrix_width; x++) {
    for (int y = 0; y <matrix_height; y++) {
      yield();
      int c = value(x, y, zoomMandelbrot);
      display->drawPixelRGB888(x, y, c, 0, 0);
    }
  }
  display->showBuffer();
}

// do something usefull instead of just waiting
void myDelay(ulong millisecs) {
  long time = millis();
  while (millis() - time < millisecs) {
    ArduinoOTA.handle();
    receiveUdp();
    yield();
  }
}

void loop() {
  if (!off) {
    switch (mode)
    {
    case 1:
      scoreboard->show();
      myDelay(30);
      break;
    case 2:
      timeSample1();
      myDelay(30);
      break;
    case 3:
      timeSample2();
      myDelay(30);
      break;
    case 4:
      timeSample3();
      myDelay(1);
      break;
    case 5:
      timeSample4();
      myDelay(1);
      break;
    case 6:
      mandelbrot();
      myDelay(1);
      break;
    
    default:
      myDelay(1);

      break;
    }
  } else {
    myDelay(30);
  }


}
