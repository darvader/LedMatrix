#define PxMATRIX_double_buffer true
#define double_buffer

#include <Mandel.h>
#include <PxMatrix.h>
#include <complex>

#ifdef ESP8266
Mandel::Mandel(PxMATRIX *display)
#endif
#ifdef ESP32
Mandel::Mandel(VirtualMatrixPanel *display)
#endif
{
    this->display = display;
}


Mandel::~Mandel()
{
}

int Mandel::value(double real, double imag)
{
    int limit = 100;
    double zReal = real;
    double zImag = imag;

    for (int i = 0; i < limit; ++i)
    {
      double r2 = zReal * zReal;
      double i2 = zImag * zImag;

      if (r2 + i2 > 4.0)
        return i;

      zImag = 2.0 * zReal * zImag + imag;
      zReal = r2 - i2 + real;
    }
    return limit;
}

void Mandel::draw_deep(double x_start, double x_fin, double y_start, double y_fin) {
  clear();
  zoomMandelbrot *= 0.99;

  int width = 64; //number of characters fitting horizontally on my screen 
	//~ int width = 300; //number of characters fitting horizontally on my screen 
	int height = 32; //number of characters fitting vertically on my screen
	//~ int height = 70; //number of characters fitting vertically on my screen

  double dx = (x_fin - x_start)/(width-1);
	double dy = (y_fin - y_start)/(height-1);

  for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
      yield();
			
			double x = x_start + j*dx; // current real value
			double y = y_fin - i*dy; // current imaginary value

      int c = value(x,y);

      display->drawPixelRGB888(j, i, c, 0, 0);
    }
  }
  showBuffer();
}


void Mandel::mandelbrot() {
	
	//~ double center_x = -0.7746806106269039;
	//~ double center_y = -0.1374168856037867;
	//~ double iter = 31;
	//~ int color_threshold = 25;
	
	static double center_x = -1.04082816210546;
	static double center_y = 0.346341718848392;
	static int iter = 83;
	
	static double factor = 1.0;
	
	static double x_start = 0.0;
	static double x_fin = 0.0;
	static double y_start = 0.0;
	static double y_fin = 0.0;
	static int i = 0;

  i++;
  if (i > iter) {
    i = 0;
    factor = 1.0;
  }

  factor = factor / 1.3;
  x_start = center_x - 1.5*factor;
  x_fin = center_x + 1.5*factor;
  y_start = center_y - factor;
  y_fin = center_y + factor;
	draw_deep(x_start, x_fin, y_start, y_fin);
	
} 
