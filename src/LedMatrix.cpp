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
#include <FFTLed.h>
#include <scoreboard.h>
#include <Globals.h>
#include <PxMatrix.h>
#include <TimeSample.h>
#include <Mandel.h>
#include <Timer.h>
#include <fauxmoESP.h>

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
uint8_t display_draw_time=10; //30-70 is usually fine

// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX *display = new PxMATRIX(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
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
char incomingPacket[64*32*3];
char replyPacket[] = "LedMatrix";
IPAddress masterIp;
int mode = 6;
Scoreboard *scoreboard;
TimeSample timeSample(display, timeClient);
Mandel mandel(display);
Timer timer(display);
fauxmoESP fauxmo;

void setupUdp();
void receiveUdp();
void displayOff();
void myDelay(ulong millisecs);

void setupFauxmo() {
  fauxmo.addDevice("Uhr 1");
  fauxmo.addDevice("Uhr 2");
  fauxmo.addDevice("Uhr 3");
  fauxmo.addDevice("Uhr 4");
  fauxmo.addDevice("Schnee");
  fauxmo.addDevice("Uhr");

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
      Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
      if (std::strcmp(device_name,"Uhr") == 0) {
        if (!state) {
          displayOff();
        }
        display_draw_time = value/255.0 * 60 ;
        return;
      } else off = false;
      if (std::strcmp(device_name,"Uhr 1") == 0) {
        mode = 2;

        return;
      }
      if (std::strcmp(device_name,"Uhr 2") == 0) {
        mode = 3;
        return;
      }
      if (std::strcmp(device_name,"Uhr 3") == 0) {
        mode = 4;
        return;
      }
      if (std::strcmp(device_name,"Uhr 4") == 0) {
        mode = 5;
        return;
      }
      if (std::strcmp(device_name,"Schnee") == 0) {
        mode = 6;
        return;
      }

  });
}

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

  setupFauxmo();
  setupWifiUpdate();
  setupUdp();
  timeClient->begin();
  scoreboard = new Scoreboard(timeClient, display);
}


void setupUdp() {
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  // WebSerial.println("Udp configured.");
}

void detect() {
  // send back a reply, to the IP address and port we got the packet from
  masterIp = Udp.remoteIP();
  Udp.beginPacket(masterIp, 4445);
  Udp.write(replyPacket);
  Udp.endPacket();
  Serial.println("Detect received.");
  display->clearDisplay();
  display->showBuffer();
  display->clearDisplay();
  for(int i = 0; i < matrix_width; i++) {
    display->drawFastVLine(i,0,31,myRED);
    display->setCursor(10,10);
    display->setTextColor(myGREEN);
    display->setTextSize(1);
    display->print("Detect");
    myDelay(20);
    display->showBuffer();
  }
}

void displayOff() {
  off = true;
  display->clearDisplay();
  display->showBuffer();
  display->clearDisplay();
  display->showBuffer();
}

void displayPicture() {
  int i = 0;
  for (int x = 0; x < matrix_width; x++) {
    yield();
    for (int y = 0; y < matrix_height; y++)
    {
      display->drawPixelRGB888(x,y, incomingPacket[i++], incomingPacket[i++], incomingPacket[i++]);
    }
    
  }
  display->showBuffer();
}

void receiveUdp() {
  int packetSize = Udp.parsePacket();

  if (packetSize)
  {
    int len = Udp.read(incomingPacket, 6144);
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
    if (std::strcmp(incomingPacket,"picture") == 0) {
      mode = 20;
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
    if (strstr(incomingPacket,"timer=") != NULL) {
      mode = 30;
      uint16_t timeToSet = incomingPacket[6] << 8 | incomingPacket[7];
      Serial.printf("Timer: %d", timeToSet);
      timer.setTimer(timeToSet);

      return;
    }
    if (std::strcmp(incomingPacket,"timerStart") == 0) {
      timer.start();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatch") == 0) {
      mode = 30;
      timer.stopWatch();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatchStart") == 0) {
      timer.stopWatchStart();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatchStop") == 0) {
      timer.stopWatchStop();
      return;
    }
    if (std::strcmp(incomingPacket,"timerPause") == 0) {
      timer.pause();
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
    if (std::strcmp(incomingPacket,"timeSnow") == 0) {
      mode = 6;
      return;
    }
    if (std::strcmp(incomingPacket,"scoreboard") == 0) {
      mode = 1;
      return;
    }
    if (std::strcmp(incomingPacket,"mandelbrot") == 0) {
      mode = 60;
      zoomMandelbrot = 1.0;
      return;
    }
    if (mode == 20) {
      displayPicture(); 

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

// do something usefull instead of just waiting
void myDelay(ulong millisecs) {
  long time = millis();
  while (millis() - time < millisecs) {
    fauxmo.handle();
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
      timeSample.timeSample1();
      myDelay(30);
      break;
    case 3:
      timeSample.timeSample2();
      myDelay(30);
      break;
    case 4:
      timeSample.timeSample3();
      myDelay(1);
      break;
    case 5:
      timeSample.timeSample4();
      myDelay(1);
      break;
    case 6:
      timeSample.timeSnow();
      myDelay(30);
      break;
    case 60:
      mandel.mandelbrot();
      myDelay(1);
      break;
    case 30:
      timer.show();
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
