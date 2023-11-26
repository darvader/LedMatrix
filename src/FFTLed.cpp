#include <Arduino.h>
//#include <arduinoFFT.h>
#include <Globals.h>
#include <FFTLed.h>
#include <PxMatrix.h>

#define SAMPLES 1              //Must be a power of 2
#define SAMPLING_FREQUENCY 20000 //Hz, must be 10000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT.
#define amplitude 2
unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;
float scale = 1.0f;


// arduinoFFT FFT = arduinoFFT();

#ifdef ESP32
void ledFFT(VirtualMatrixPanel *display) {
#else
void ledFFT(PxMATRIX *display) {
#endif
  int j = 0;
  Serial.printf("Start LED");
  int fadeDown = 10;
  int threshold = 10;
#ifdef ESP32
  display->clearScreen();
#else
  display->clearDisplay();
#endif

  for (int i = 2; i < 128; i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    int real = (int) vReal[i];
    if (real<threshold) {
      real = 0;
    }
    // Serial.printf("Color: %d for j: %d & i: %d ", real, j, i);
    if (i <= (63))   {
      //red part (basses)
      display->drawPixel(i, 32 - real * 32/255, myRED);
    } else {
      display->drawPixel(i-64, 32 - real * 32/255, myBLUE);
    } 
    yield();
  }
  // display->showBuffer();
}

// int led;

// void fftRow() {
//   calcFFT();
//   ledFFTRow();
// }

// boolean drawColumn(int color) {
//   for (int i = 0; i < 10; i++) {
//     // pixelsStrip.setPixelColor(led++, color);
//     // if (led >= numPixelStrip) return true;
//   }
//   return false;
// }

// int oldFreq[7] = {0};

// void ledFFTRow() {
//   int freq[7] = {0};

//   for (int i = 2; i < SAMPLES/2; i++) {
//     int real = (int) vReal[i];
//     if (real < 10) continue;
//     if (i<=5 )  {if (real>freq[0]) freq[0] = real;} else// 125Hz
//     if (i >5   && i<=12 ) {if (real>freq[1]) freq[1] = real;} else // 250Hz
//     if (i >12  && i<=32 ) {if (real>freq[2]) freq[2] = real;} else // 500Hz
//     if (i >32  && i<=62 ) {if (real>freq[3]) freq[3] = real;} else // 1000Hz
//     if (i >62  && i<=105 ) {if (real>freq[4]) freq[4] = real;} else // 2000Hz
//     if (i >105 && i<=120 ) {if (real>freq[5]) freq[5] = real;} else // 4000Hz
//     if (i >120 && i<=146 ) {if (real>freq[6]) freq[6] = real;}  // 8000Hz
//   }

//   for (int i = 0; i < 7; i++)
//   {
//     if (oldFreq[i]<freq[i]) {
//       oldFreq[i] = freq[i];
//     } else {
//       freq[i] = oldFreq[i];
//       if (oldFreq[i] < 10) oldFreq[i] = 0; else  oldFreq[i] -= 10;
//     }
//   }
  
//   led = 0;
//   // while (led < numPixelStrip) {
//     // if (drawColumn(pixelsStrip.Color(freq[0], 0, 0))) break;
//     // if (drawColumn(pixelsStrip.Color(0, freq[1], 0))) break;
//     // if (drawColumn(pixelsStrip.Color(0, 0, freq[2]))) break;
//     // if (drawColumn(pixelsStrip.Color(freq[3],freq[3], 0))) break;
//     // if (drawColumn(pixelsStrip.Color(freq[4], 0, freq[4]))) break;
//     // if (drawColumn(pixelsStrip.Color(0, freq[5], freq[5]))) break;
//     // if (drawColumn(pixelsStrip.Color(freq[6], freq[6], freq[6]))) break;
//   // }
//   // pixelsStrip.show();

// }

// void calcFFT() {
//   // Serial.println(analogRead(A0));
//   double max = 0;
//   double min = 9999999;
//   // double diff = 0;
//   double analog = 0; 
//   for (int i = 0; i < SAMPLES; i++) {
//     newTime = micros()-oldTime;
//     oldTime = newTime;
//     analog = analogRead(A0);

//     if (max < analog) max = analog;
//     if (min > analog) min = analog;
//     vReal[i] = analog; // A conversion takes about 1mS on an ESP8266
//     // vImag[i] = 0;
//     while (micros() < (newTime + sampling_period_us)) { /* do nothing to wait */ }
//   }
//   // diff = max - min;
//   // WebSerial.println(String("min=")+min+String(" max=")+max+String(" diff="));
//   // Serial.printf("min: %f, max: %f, diff=%f", min, max, diff);
//   // Serial.println();

//   FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
//   FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
//   FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

//   max = 0.0;
//   for (int i = 20; i < SAMPLES/2; i++)
//   {
//     double r = vReal[i];
//     if (r > max) max = r;
//   }

//   if (max * scale > 255) scale = 255/max;
//   else if (max * scale < 200) scale *= 1.001f;

//   for (int i = 2; i < SAMPLES/2; i++)
//   {
//     vReal[i] *= scale;
//   }
// }

// void fft() {
//   calcFFT();
//   ledFFT();
// }


