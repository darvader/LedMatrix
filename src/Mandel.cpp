#define PxMATRIX_double_buffer true
#define double_buffer

#include <Mandel.h>
#include <PxMatrix.h>
#include <complex>
#include <Globals.h>

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

void Mandel::mandelbrot() {
	static float zoom = 1.0;
  static float targetX = -0.65;  // Center on part of the Mandelbrot set
  static float targetY = 0.0;
  static unsigned long zoomStartTime = millis();
  static float zoomDuration = 60000;  // 1 minute
  static float targetZoom = 10.0;  // Target zoom level after 1 minute

  float zoomProgress = (millis() - zoomStartTime) / zoomDuration;
  if ((millis() - zoomStartTime)>zoomDuration) zoomStartTime = millis();
  if (zoomProgress > 1.0) zoomProgress = 1.0;
  zoom = 1.0 + (targetZoom - 1.0) * zoomProgress;

  float xmin = targetX - 2.5 / zoom;
  float xmax = targetX + 1.5 / zoom;
  float ymin = targetY - 2.0 / zoom;
  float ymax = targetY + 2.0 / zoom;

	float x, y, xx;
  int hx, hy, hxmax = matrix_width, hymax = matrix_height;
  int iteration, max_iteration = 1000;
  uint16_t color;

  float xr = (xmax - xmin) / ((hxmax - 1) * zoom);
  float yr = (ymax - ymin) / ((hymax - 1) * zoom);

  for (hx = 0; hx < hxmax; hx++) {
    for (hy = 0; hy < hymax; hy++) {
      float cr = xmin + (hx * xr);
      float ci = ymin + (hy * yr);

      x = 0.0;
      y = 0.0;
      iteration = 0;
      
      while ((x*x + y*y <= 4) && (iteration < max_iteration)) {
        xx = x*x - y*y + cr;
        y = 2.0*x*y + ci;
        x = xx;
        iteration++;
      } 
      
      if (iteration >= max_iteration) {
        // Inside the Mandelbrot set, color it black
        color = myBLACK;
      } else {
        // Outside the Mandelbrot set, color it with a gradient based on the number of iterations
        color = display->color565(iteration % 256, iteration % 128, iteration % 64); 
      }
      
      display->drawPixel(hx, hy, color);
    }
  }
  showBuffer();
} 
