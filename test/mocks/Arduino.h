// Minimal Arduino.h stub for native testing
#ifndef ARDUINO_H_STUB
#define ARDUINO_H_STUB

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>

typedef uint8_t byte;
typedef bool boolean;

// Minimal stubs
inline unsigned long millis() { return 0; }
inline void yield() {}
inline long random(long max) { return rand() % max; }
inline long random(long min, long max) { return min + (rand() % (max - min)); }
inline void delay(unsigned long ms) {}

// Serial stub
class SerialStub {
public:
    void begin(unsigned long) {}
    void println(const char*) {}
    void print(const char*) {}
    int printf(const char* fmt, ...) { return 0; }
    int printf_P(const char* fmt, ...) { return 0; }
};
static SerialStub Serial;

// PROGMEM stub
#define PROGMEM
#define F(x) x
#define PSTR(x) x

// Math
#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef M_PI
#define M_PI PI
#endif
#ifndef M_TWOPI
#define M_TWOPI (2.0 * M_PI)
#endif

#endif // ARDUINO_H_STUB
