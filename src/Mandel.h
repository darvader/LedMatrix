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
    int value (int x, int y, float zoom);
    void mandelbrot();
};
