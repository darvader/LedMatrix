
#ifndef _MANDEL
#define _MANDEL

#include <BaseLedMatrix.h>

class Mandel: public BaseLedMatrix
{
private:
    /* data */
public:
    float zoomMandelbrot;
#ifdef ESP8266
    Mandel(PxMATRIX *display);
#endif
#ifdef ESP32
    Mandel(VirtualMatrixPanel *display);
#endif
    ~Mandel();
    void mandelbrot();
};
#endif
