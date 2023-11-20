
#include <BaseLedMatrix.h>
#define PxMATRIX_double_buffer true
#define double_buffer

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
