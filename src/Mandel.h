#define PxMATRIX_double_buffer true
#define double_buffer

#include <PxMatrix.h>

class Mandel
{
private:
    /* data */
public:
    float zoomMandelbrot;
    PxMATRIX *display;
    Mandel(PxMATRIX *display);
    ~Mandel();
    int value(double real, double imag);
    void draw_deep(double x_start, double x_fin, double y_start, double y_fin);
    void mandelbrot();
};
