#ifdef ESP32
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
void ledFFT(VirtualMatrixPanel *display);
#else
#include <PxMatrix.h>
void ledFFT(PxMATRIX *display);
#endif
void fft();
void fftRow();
void ledFFTRow();
void calcFFT();
