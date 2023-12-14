#ifdef ESP8266
// Creates a second buffer for background drawing (doubles the required RAM)
#define PxMATRIX_double_buffer true
#define double_buffer
#include <PxMatrix.h>
#endif

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <cstring>
#include <WifiUpdate.h>
#include <NTPClient.h>
// #include <FFTLed.h>
#include <scoreboard.h>
#include <Globals.h>
#include <TimeSample.h>
#include <Mandel.h>
#include <Timer.h>
#include <fauxmoESP.h>


// Pins for LED MATRIX
#ifdef ESP32
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
  #include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
  #include "WiFi.h"
 // #include "AsyncTCP.h"
  #include "Udp.h"
// Change these to whatever suits
  #define R1 27
  #define G1 10
  #define B1 25
  #define R2 22
  #define G2 13
  #define B2 21
  #define A 17
  #define B 5
  #define C 16
  #define D 23
  #define E -1 // required for 1/32 scan panels, like 64x64px. Any available pin would do, i.e. IO32
  #define LAT 19
  #define OE 32
  #define CLK 15

  #define NUM_ROWS 2 // Number of rows of chained INDIVIDUAL PANELS
  #define NUM_COLS 2 // Number of INDIVIDUAL PANELS per ROW
  #define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another
  #define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_RIGHT_UP
  

  // placeholder for the matrix object
    MatrixPanel_I2S_DMA *dma_display = nullptr;

    // placeholder for the virtual display object
    VirtualMatrixPanel  *display = nullptr;
    const uint8_t replyPacket[] = "LedMatrix";

#endif

uint8_t display_draw_time=40; //30-70 is usually fine
#ifdef ESP8266

#include <ESP8266WiFi.h>
#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display-> If too large the ESP will crash

// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX *display = new PxMATRIX(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);
const char replyPacket[] = "LedMatrix";

#endif
using namespace std;

inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Some standard colors
const uint16_t myRED = color565(255, 0, 0);
const uint16_t myGREEN = color565(0, 255, 0);
const uint16_t myBLUE = color565(0, 0, 255);
const uint16_t myWHITE = color565(255, 255, 255);
const uint16_t myYELLOW = color565(255, 255, 0);
const uint16_t myOrange = color565(255, 127, 0);
const uint16_t myCYAN = color565(0, 255, 255);
const uint16_t myMAGENTA = color565(255, 0, 255);
const uint16_t myBLACK = color565(0, 0, 0);
const uint16_t timeoutColor = color565(0, 50, 50);

const uint16_t myCOLORS[9] = {myBLACK,myRED,myGREEN,myBLUE,myOrange,myYELLOW,myCYAN,myMAGENTA,myWHITE};


float zoomMandelbrot = 1.0;

boolean off = false;

// US Eastern Time Zone (New York, Detroit)
const TimeChangeRule myDST PROGMEM = {"EDT", Second, Sun, Mar, 2, 120};    // Daylight time = UTC - 4 hours
const TimeChangeRule mySTD PROGMEM = {"EST", First, Sun, Nov, 2, 60};     // Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

// If TimeChangeRules are already stored in EEPROM, comment out the three
// lines above and uncomment the line below.
//Timezone myTZ(100);       // assumes rules stored at EEPROM address 100

TimeChangeRule *tcr;        // pointer to the time change rule, use to get TZ abbrev

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display->display(display_draw_time*0.7);
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

}


const int ESP_BUILTIN_LED = 2;
// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
char scrollingText[100] = "VSV Jena 90 e.V. : Gastmannschaft";


WiFiUDP Udp;
WiFiUDP UdpNtp;

const long utcOffsetInSeconds = 60*60*0;
NTPClient *timeClient = new NTPClient(UdpNtp, "pool.ntp.org", utcOffsetInSeconds);
unsigned int localUdpPort = 4210;
char incomingPacket[255];
//char incomingPacket[64*32*3];
IPAddress masterIp;
int mode = 8;
Scoreboard *scoreboard;
TimeSample *timeSample;
Mandel *mandel;
Timer *timer;
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
  fauxmo.addDevice("Plasma");
  fauxmo.addDevice("Uhr");

  fauxmo.setPort(80); // required for gen3 devices
  fauxmo.enable(false);

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

inline void showBuffer() {
  #ifdef ESP8266
    display->showBuffer();
  #else
    display->flipDMABuffer();
  #endif
}

inline void clear() {
  #ifdef ESP8266
    display->clearDisplay();
  #endif
  #ifdef ESP32
    display->clearScreen();
  #endif
}

void logMemory() {
#ifdef ESP32
  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  log_d("Min Free heap: %d", ESP.getMinFreeHeap());
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free SketchSpace: %d", ESP.getFreeSketchSpace());
#else
  Serial.printf("Free heap: %d", ESP.getFreeHeap());
  Serial.printf("Free SketchSpace: %d", ESP.getFreeSketchSpace());
#endif
}

void setupDisplay() {
  #ifdef ESP8266
  display->begin(16);
  display->setFastUpdate(true);
    // Set driver chip type

  clear();
  display->setTextColor(myCYAN);
  display->setCursor(2,0);
  display->print("Pixel");
  display->setTextColor(myMAGENTA);
  display->setCursor(2,8);
  display->print("Time");
  display_update_enable(true);
  display->setDriverChip(FM6124);
  showBuffer();
  #endif
  #ifdef ESP32
// Define your display layout here, e.g. 1/8 step, and optional SPI pins begin(row_pattern, CLK, MOSI, MISO, SS)
  //Another way of creating config structure
  //Custom pin mapping for all pins
  HUB75_I2S_CFG::i2s_pins _pins={R1, G1, B1, R2, G2, B2, A, B, C, D, E, LAT, OE, CLK};
  HUB75_I2S_CFG mxconfig(
                          64,   // width
                          32,   // height
                            4,   // chain length
                        _pins,   // pin mapping
    HUB75_I2S_CFG::FM6126A      // driver chip
  );
  mxconfig.double_buff = true;
  // mxconfig.setPixelColorDepthBits(6);
  // mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;
  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // let's adjust default brightness to about 75%

  // Allocate memory and start DMA display
  if( not dma_display->begin() )
      Serial.println(F("****** !KABOOM! I2S memory allocation failed ***********"));

  // create VirtualDisplay object based on our newly created dma_display object
  display = new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, 64, 32, VIRTUAL_MATRIX_CHAIN_TYPE);

  // So far so good, so continue
  display->fillScreen(display->color444(0, 0, 0));
  display->drawDisplayTest(); // draw text numbering on each screen to check connectivity
  dma_display->setBrightness8(100);    // range is 0-255, 0 - 0%, 255 - 100%
  display->flipDMABuffer();

  Serial.println(F("Chain of 4x 64x32 panels for this example:"));
  Serial.println(F("+---------+---------+"));
  Serial.println(F("|    4    |    3    |"));
  Serial.println(F("|         |         |"));
  Serial.println(F("+---------+---------+"));
  Serial.println(F("|    1    |    2    |"));
  Serial.println(F("| (ESP32) |         |"));
  Serial.println(F("+---------+---------+"));
  // delay(3000);

   // draw blue text
   // display->setFont(&FreeSansBold12pt7b);
   display->setTextColor(display->color565(0, 0, 255));
   display->setTextSize(3); 
   display->setCursor(0, display->height()/2-10);    
   display->print("ABCDEFGHI");
   
   // Red text inside red rect (2 pix in from edge)
   display->drawRect(1,1, display->width()-2, display->height()-2, display->color565(255,0,0));

   // White line from top left to bottom right
   display->drawLine(0,0, display->width()-1, display->height()-1, display->color565(255,255,255));
   display->flipDMABuffer();
   //display->drawDisplayTest(); // re draw text numbering on each screen to check connectivity
  #endif

}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Booting"));

  setupDisplay();
  Serial.println(F("finished"));
  display->setTextWrap(false);

  setupWifiUpdate();
  setupFauxmo();
  setupUdp();
  timeClient->begin();
  scoreboard = new Scoreboard(timeClient, display);
  timeSample = new TimeSample(display, timeClient);
  mandel = new Mandel(display);
  timer = new Timer(display);

  logMemory();

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
  Udp.write(replyPacket, sizeof(replyPacket));
  Udp.endPacket();
  Serial.println(F("Detect received."));
  clear();
  showBuffer();
  clear();
  for(int i = 0; i < matrix_width; i+=2) {
    display->drawFastVLine(i,0,matrix_height-1,myRED);
    display->setCursor(10,10);
    display->setTextColor(myGREEN);
    display->setTextSize(1);
    display->print("Detect");
    delay(10);
    showBuffer();
  }
  clear();
  showBuffer();
  clear();
}

void displayOff() {
  off = true;
  clear();
  showBuffer();
  clear();
  showBuffer();
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
  showBuffer();
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
      #ifdef ESP32
        dma_display->setBrightness8(display_draw_time*2.55);
      #endif
      return;
    }
    if (strstr(incomingPacket,"timer=") != NULL) {
      mode = 30;
      uint16_t timeToSet = incomingPacket[6] << 8 | incomingPacket[7];
      Serial.printf("Timer: %d", timeToSet);
      timer->setTimer(timeToSet);

      return;
    }
    if (std::strcmp(incomingPacket,"timerStart") == 0) {
      timer->start();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatch") == 0) {
      mode = 30;
      timer->stopWatch();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatchStart") == 0) {
      timer->stopWatchStart();
      return;
    }
    if (std::strcmp(incomingPacket,"stopWatchStop") == 0) {
      timer->stopWatchStop();
      return;
    }
    if (std::strcmp(incomingPacket,"timerPause") == 0) {
      timer->pause();
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
      timeSample->initializedSnow = false;
      mode = 6;
      return;
    }
    if (std::strcmp(incomingPacket,"timePlasma") == 0) {
      mode = 7;
      return;
    }
    if (std::strcmp(incomingPacket,"timeColoredSnow") == 0) {
      timeSample->initializedSnow = false;
      mode = 8;
      return;
    }
    if (std::strcmp(incomingPacket,"timeGameOfLife") == 0) {
      timeSample->initializedGOL = false;
      mode = 9;
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
      // ledFFT(display); 

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
    yield();
    fauxmo.handle();
    ArduinoOTA.handle();
    receiveUdp();
  }
}

void loop() {
  if (!off) {
    switch (mode)
    {
    case 1:
      scoreboard->show();
      myDelay(2);
      break;
    case 2:
      timeSample->timeSample1();
      myDelay(30);
      break;
    case 3:
      timeSample->timeSample2();
      myDelay(1);
      break;
    case 4:
      timeSample->timeSample3();
      myDelay(1);
      break;
    case 5:
      timeSample->timeSample4();
      myDelay(1);
      break;
    case 6:
      timeSample->timeSnow(false);
      myDelay(20);
      break;
    case 7:
      timeSample->timePlasma();
      myDelay(1);
      break;
    case 8:
      timeSample->timeSnow(true);
      myDelay(20);
      break;
    case 9:
      timeSample->timeGameOfLife();
      myDelay(30);
      break;
    case 60:
      mandel->mandelbrot();
      myDelay(1);
      break;
    case 30:
      timer->show();
      myDelay(1);
      break;
    default:
      myDelay(1);

      break;
    }
  } else {
    myDelay(1);
  }
}
