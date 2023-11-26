#ifndef __globals_led_h__
#define __globals_led_h__
#include<Timezone.h>

extern double vReal[];
extern char incomingPacket[];
extern char scrollingText[];
extern const uint16_t myCOLORS[];

extern const uint16_t myRED;
extern const uint16_t myGREEN;
extern const uint16_t myBLUE;
extern const uint16_t myWHITE;
extern const uint16_t myYELLOW;
extern const uint16_t myOrange;
extern const uint16_t myCYAN;
extern const uint16_t myMAGENTA;
extern const uint16_t myBLACK;
extern const uint16_t timeoutColor;

// US Eastern Time Zone (New York, Detroit)
const extern TimeChangeRule myDST;    // Daylight time = UTC - 4 hours
const extern TimeChangeRule mySTD;     // Standard time = UTC - 5 hours
extern Timezone myTZ;

// If TimeChangeRules are already stored in EEPROM, comment out the three
// lines above and uncomment the line below.
//Timezone myTZ(100);       // assumes rules stored at EEPROM address 100

extern TimeChangeRule *tcr;        // pointer to the time change rule, use to get TZ abbrev

#ifdef ESP8266
    #define matrix_width 64
    #define matrix_height 32
#else
    #define matrix_width 128 // Number of pixels wide of each INDIVIDUAL panel module. 
    #define matrix_height 64 // Number of pixels tall of each INDIVIDUAL panel module.
#endif

#endif