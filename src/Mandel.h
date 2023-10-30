
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
    int value(double real, double imag);
    void draw_deep(double x_start, double x_fin, double y_start, double y_fin);
    void mandelbrot();
};
