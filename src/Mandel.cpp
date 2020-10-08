#define PxMATRIX_double_buffer true
#define double_buffer

#include <Mandel.h>
#include <PxMatrix.h>
#include <complex>

Mandel::Mandel(PxMATRIX *display)
{
    this->display = display;
}

Mandel::~Mandel()
{
}

int Mandel::value (int x, int y, float zoom)  {
    // complex<float> point((float)x/30-1.5, (float)y/30-0.5);
    // point *= zoom;
    // complex<float> z(0, 0);
    // int nb_iter = 0;
    // while (abs (z) < 2 && nb_iter <= 20) {
    //     z = z * z + point;
    //     nb_iter++;
    // }
    // if (nb_iter < 20)
    //    return (255*nb_iter)/20;
    // else 
    //    return 0;
}

void Mandel::mandelbrot() {
  display->clearDisplay();
  zoomMandelbrot *= 0.99;
  for (int x = 0; x<64; x++) {
    for (int y = 0; y <32; y++) {
      yield();
      int c = value(x, y, zoomMandelbrot);
      display->drawPixelRGB888(x, y, c, 0, 0);
    }
  }
  display->showBuffer();
}

