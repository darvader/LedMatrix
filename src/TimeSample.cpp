#include <TimeSample.h>
#include <Globals.h>
#include <Adafruit_NeoPixel.h>
#include <TimeLedMatrix.h>
#include <FastLED.h>
#include <Counter.h>

#ifdef ESP8266
TimeSample::TimeSample(PxMATRIX *display, NTPClient *timeClient, Counter *counter)
{
#endif
#ifdef ESP32
TimeSample::TimeSample(VirtualMatrixPanel *display, NTPClient *timeClient, Counter *counter)
{
#endif
    this->display = display;
    this->timeClient = timeClient;
    this->counter = counter;
    setupTime();
}

TimeSample::~TimeSample()
{
}

struct line_t {
  uint16_t color;
  int16_t x1, x2, y1, y2;
  double arc;
  double degree;
};

enum Snow { small, medium, large };
struct snow_t {
  Snow type;
  float x, y;
  float speed;
  uint8_t r, g, b;
};

struct star_t {
  float x, y;     // position
  float z;        // depth (distance from viewer)
  float vz;       // velocity in z direction
  uint8_t brightness;
};

void TimeSample::drawTimeWithBackground() {
  #ifdef ESP8266
  static int x = 1;
  #else
  static int x = 4;
  #endif
  static int width = display->width();
  static int height = display->height();
#ifdef ESP32
  display->fillRect(39, height/2-5, 49, 9, myBLACK);
#else
  display->fillRect(9, 10, 49, 9, myBLACK);
#endif
  drawTime();
}

void inline TimeSample::drawTime() {
  timeClient->update();
  time_t utc = timeClient->getEpochTime();
  time_t local = myTZ.toLocal(utc, &tcr);

#ifdef ESP32
  display->setTextSize(1);
  display->setCursor(40, (display->height()/2)-4);
#else
  display->setTextSize(1);
  display->setCursor(10,11);
#endif
  display->setTextColor(myMAGENTA);
  display->printf("%02d:%02d:%02d", hour(local), minute(local), second(local));
}

void TimeSample::timeSample1() {
  static int x = 0;
  static int y = 0;
  static int stepX = 1;
  static int stepY = 1;
  static int width = display->width()-1;
  static int height = display->height()-1;

  if (x == width) stepX = -1;
  if (x == 0) stepX = 1;
  if (y == height) stepY = -1;
  if (y == 0) stepY = 1;

  clear();
  

  display->drawLine(x, 0, width-x, height, myRED);
  display->drawLine(width-x, 0, x, height, myBLUE);
  display->drawLine(0, y, width, height-y, myGREEN);
  display->drawLine(0, height-y, width, y, myYELLOW);

  drawTimeWithBackground();

  x += stepX;
  y += stepY;
  showBuffer();

  // delay(30);
}

void TimeSample::timeSample2() {
  static const int size = 10; 
  static line_t lines[size];
  static boolean initialized = false;
  static const byte width = matrix_width/2; 
  static const byte height = matrix_height/2; 
  static byte radius = width+5;

  if (!initialized) {
    for (int i = 0; i < size; i++) {
      lines[i].x1 = sin(i * PI/size) * radius + width;
      lines[i].y1 = cos(i * PI/size) * radius + height;
      lines[i].x2 = sin(i * PI/size + PI) * radius + width;
      lines[i].y2 = cos(i * PI/size + PI) * radius + height;

      lines[i].color = myCOLORS[i % 7 + 1];
      lines[i].arc = i * PI/size;
    }
    initialized = true;
  }

  clear();

  for (int i = 0; i<size; i++) {
    display->drawLine(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    lines[i].arc -= PI * 0.2/180; // 2 degree more

    if (lines[i].arc < 2 * PI) lines[i].arc += 2 * PI;

    double arc = lines[i].arc;
    lines[i].x1 = sin(arc) * radius + width;
    lines[i].y1 = cos(arc) * radius + height;
    lines[i].x2 = sin(arc + PI) * radius + width;
    lines[i].y2 = cos(arc + PI) * radius + height;
  }
  drawTimeWithBackground();

  showBuffer();
}

void TimeSample::timeSample3() {
  static const int size = 20; 
  static line_t lines[size];
  static boolean initialized = false;
  static byte radius = matrix_width/2+5;
  static const int step = 360/size; 

  if (!initialized) {
    for (int i = 0; i < 360; i++) {
      xVals[i] = sin(i/180.0 * PI) * radius + matrix_width/2;
      yVals[i] = cos(i/180.0 * PI) * radius + matrix_height/2;
    }

    for (int i = 0; i < size; i++) {
      lines[i].x1 = xVals[i * step];
      lines[i].y1 = yVals[i * step];
      int step2 = (i + 1) * step;
      if (step2 >= 360 ) step2 -=360; 
      lines[i].x2 = xVals[(i + 1) * step];
      lines[i].y2 = yVals[(i + 1) * step];

      lines[i].color = myCOLORS[i % 7];
      lines[i].degree = i * step;
    }

    initialized = true;
  }

  clear();

  for (int i = 0; i<size; i++) {
    lines[i].degree -= 0.2; // 2 degree more
    if (lines[i].degree < 0) lines[i].degree += 360;

    display->fillTriangle(matrix_width/2, matrix_height/2, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    int degree = lines[i].degree;
    lines[i].x1 = xVals[degree];
    lines[i].y1 = yVals[degree];
    int step2 = degree + step;
    if (step2 >= 360) step2 -= 360;
    lines[i].x2 = xVals[step2];
    lines[i].y2 = yVals[step2];
  }
  drawTimeWithBackground();

  showBuffer();
}

void TimeSample::timeSample4() {
  timeClient->update();
  static const int size = 60; 
  static line_t lines[size];
  static boolean initialized = false;
  static const int step = 360/size; 
  static int width = display->width();
  static int height = display->height();
  static byte radius = height + 10;

  if (!initialized) {
    for (int i = 0; i < 360; i++) {
      xVals[i] = sin(i/180.0 * PI) * radius + width/2;
      yVals[i] = cos(i/180.0 * PI) * radius + height/2;
    }

    for (int i = 0; i < size; i++) {
      lines[i].x1 = xVals[i * step];
      lines[i].y1 = yVals[i * step];
      int step2 = (i + 1) * step;
      if (step2 >= 360 ) step2 -=360; 
      lines[i].x2 = xVals[(i + 1) * step];
      lines[i].y2 = yVals[(i + 1) * step];
      uint32_t c32 = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(65536L/size * i));
      lines[i].color = display->color565((c32 & 0xff0000) >> 16, (c32 & 0xff00) >> 8, c32 & 0xff);
      lines[i].degree = i * step;
    }

    initialized = true;
  }

  clear();

  for (int i = 0; i<size; i++) {
    lines[i].degree -= 2; // 2 degree more
    if (lines[i].degree < 0) lines[i].degree += 360;

    display->fillTriangle(width/2, height/2, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, lines[i].color);

    int degree = lines[i].degree;
    lines[i].x1 = xVals[degree];
    lines[i].y1 = yVals[degree];
    int step2 = degree + step;
    if (step2 >= 360) step2 -= 360;
    lines[i].x2 = xVals[step2];
    lines[i].y2 = yVals[step2];
  }
  drawTimeWithBackground();

  showBuffer();
}



  void TimeSample::timeEllipse() {
    clear();
    counter->drawRunners();
    drawTime();

    showBuffer();
  }

  void TimeSample::timePlasma()
  {
    static int PANE_WIDTH = display->width();
    static int PANE_HEIGHT = display->height();
    static uint16_t time_counter = 0, cycles = 0, fps = 0;
    static unsigned long fps_timer;
    static CRGB currentColor;
    static CRGBPalette16 palettes[] = {HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p};
    static CRGBPalette16 currentPalette = palettes[0];

    timeClient->update();

    // clear();


    for (int x = 0; x < PANE_WIDTH; x++)
    {
      #ifdef ESP8266
        if (x % 4 == 0) yield(); // let update the display, otherwise it flickers
      #endif
      for (int y = 0; y < PANE_HEIGHT; y++)
      {
        int16_t v = 0;
        uint8_t wibble = sin8(time_counter);
        v += sin16(x * wibble * 3 + time_counter);
        v += cos16(y * (128 - wibble) + time_counter);
        v += sin16(y * x * cos8(-time_counter) / 8);

        currentColor = ColorFromPalette(currentPalette, (v >> 8) + 127); //, brightness, currentBlendType);
        display->drawPixelRGB888(x, y, currentColor.r, currentColor.g, currentColor.b);
      }
    }

    ++time_counter;
    ++cycles;
    ++fps;

    if (cycles >= 1024)
    {
      time_counter = 0;
      cycles = 0;
      currentPalette = palettes[random(0, sizeof(palettes) / sizeof(palettes[0]))];
    }

    // print FPS rate every 5 seconds
    // Note: this is NOT a matrix refresh rate, it's the number of data frames being drawn to the DMA buffer per second
    if (fps_timer + 5000 < millis())
    {
      Serial.printf_P(PSTR("Effect fps: %d\n"), fps / 5);
      fps_timer = millis();
      fps = 0;
    }
    drawTimeWithBackground();

    showBuffer();
  }


void TimeSample::timeSnow(bool colored) {
  #ifdef ESP8266
  static const int size = 1;
  #else
  static const int size = 4;
  #endif
  static const int numSmallSnows = 60*size; 
  static const int width = display->width()-1;
  static const int height = display->height()-1; 

  static snow_t smallSnows[numSmallSnows];

  if (!initializedSnow) {
    for (int i = 0; i < numSmallSnows; i++) {
      snow_t* snow = &smallSnows[i];
      snow->type = small;
      snow->x = random(width);
      snow->y = random(height);
      snow->speed = random(1, 10)/10.0f;
      if (colored) {
        snow->r = random(255);
        snow->b = random(255);
        snow->g = random(255);
      }
    }    

    initializedSnow = true;
  }

  timeClient->update();
  clear();

  for (int i = 0; i < numSmallSnows; i++) {
      snow_t* sn = &smallSnows[i];
    if (colored) {
      uint8_t r = sn->r * sn->speed;
      uint8_t g = sn->g * sn->speed;
      uint8_t b = sn->b * sn->speed;
      display->drawPixelRGB888(sn->x, sn->y, r, g, b);
    } else {
      display->drawPixelRGB888(sn->x, sn->y, 0xff*sn->speed, 0xff*sn->speed, 0xff*sn->speed);
    }
    sn->y += sn->speed;
    if (sn->y > height) {
      sn->x = random(width); 
      sn->y = 0;
      sn->speed = random(3,10)/10.0f;
    }
  }

  drawTime();
  showBuffer();
}

int TimeSample::countAliveNeighbors(uint8_t  x, uint8_t  y) {
  uint8_t  count = 0;
  for (int8_t   i = -1; i <= 1; i++) {
    for (int8_t   j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue; // Skip the current cell
      // Wrap around the grid
      uint8_t  nx = (x + i + GRIDX) % GRIDX;
      uint8_t  ny = (y + j + GRIDY) % GRIDY;
      count += get(grid, nx, ny) ? 1 : 0;
    }
  }
  return count;
}

void TimeSample::set(uint8_t grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y, bool value) {
    uint8_t  byteIndex = x / 8;  // Find the byte in the array
    uint8_t  bitIndex = x % 8;   // Find the bit in the byte

    if (value) {
        grid[byteIndex][y] |= (1 << bitIndex);  // Set the bit using OR
    } else {
        grid[byteIndex][y] &= ~(1 << bitIndex);  // Clear the bit using AND and NOT
    }
}

bool TimeSample::get(byte grid[GRIDX_BYTE][GRIDY], uint8_t  x, uint8_t  y) {
  uint8_t  xByte = x / 8;
  uint8_t  xBit = x % 8;
  return (grid[xByte][y] & (1 << xBit)) != 0;
}

void TimeSample::timeGameOfLife() {
  static int golType = -1;
  static int startTime = millis();

  if (!initializedGOL) {
    for (int y = 0; y < GRIDY; y++) {
      for (int x = 0; x < GRIDX; x++) {
        set(grid, x, y, (random(2) == 0));
      }
    }
    golType++;
    if (golType>2) golType = 0;
    startTime = millis();
    initializedGOL = true;
  }

  clear();
  
  int changes = 0;
  for (int y = 0; y < GRIDY; y++) {
    #ifdef ESP8266 
      yield(); 
    #endif
    for (int x = 0; x < GRIDX; x++) {
      int aliveNeighbors = countAliveNeighbors(x, y);
      if (get(grid, x, y)) { // survive part
        switch (golType)
        {
        case 0: // original
          set(newGrid, x, y, aliveNeighbors == 2 || aliveNeighbors == 3);
          break;
        case 1: // high life
          set(newGrid, x, y, aliveNeighbors == 2 || aliveNeighbors == 3);
          break;
        case 2: // 2x2
          set(newGrid, x, y, aliveNeighbors >= 1 || aliveNeighbors <= 6);
          break;
        case 3: // 34 Life
          set(newGrid, x, y, aliveNeighbors == 3);
          break;
        case 4: // Long life
          set(newGrid, x, y, aliveNeighbors == 5);
          break;
        case 5: // day & night
          set(newGrid, x, y, aliveNeighbors >= 3 && aliveNeighbors <= 6);
          break;
        
        default:
          set(newGrid, x, y, aliveNeighbors == 2 || aliveNeighbors == 3);
          break;
        }
      } else { // born part
        switch (golType)
        {
        case 0: // original
          set(newGrid, x, y, aliveNeighbors == 3);
          break;
        case 1: // high life
          set(newGrid, x, y, aliveNeighbors == 3 || aliveNeighbors == 6);
          break;
        case 2: // 2x2
          set(newGrid, x, y, aliveNeighbors == 3);
          break;
        case 3: // 34 Life
          set(newGrid, x, y, aliveNeighbors == 3 || aliveNeighbors == 4);
          break;
        case 4: // Long Life
          set(newGrid, x, y, aliveNeighbors == 3);
          break;
        case 5: // day & night
          set(newGrid, x, y, aliveNeighbors >= 3 && aliveNeighbors <= 8);
          break;
        
        default:
          set(newGrid, x, y, aliveNeighbors == 2 || aliveNeighbors == 3);
          break;
        }
        
      }
      if (get(oldGrid, x, y) != get(newGrid, x, y)) {
      // if (grid[x][y] != newGrid[x][y]) {
        changes++;
      }
    }
  }

  // re-initialize if nothing much changes or at least after 2 mins
#ifdef ESP32
  if ((changes < (GRIDX * GRIDY) / 4000) || 
#else
  if ((changes < (GRIDX * GRIDY) / 1000) || 
#endif
    (millis() - startTime > 240000)) {
    initializedGOL = false;
  }

// Swap grids
  for (int y = 0; y < GRIDY; y++) {
    #ifdef ESP8266 
      yield(); 
    #endif
    for (int x = 0; x < GRIDX_BYTE; x++) {
      oldGrid[x][y] = grid[x][y];
      grid[x][y] = newGrid[x][y];
    }
  }

  // Draw the grid
  for (int y = 0; y < GRIDY; y++) {
    #ifdef ESP8266 
      yield(); 
    #endif
    for (int x = 0; x < GRIDX; x++) {
      display->drawPixel(x, y, get(grid,x,y) ? myCOLORS[countAliveNeighbors(x, y)] : myBLACK);
    }
  }

  drawTimeWithBackground();

  showBuffer();
}

void TimeSample::timeStarWars() {
  static const int numStars = 200;
  static star_t stars[numStars];
  static boolean initialized = false;
  static int width = matrix_width;
  static int height = matrix_height;
  static float centerX = width / 2.0;
  static float centerY = height / 2.0;

  if (!initialized) {
    // Initialize stars at random positions
    for (int i = 0; i < numStars; i++) {
      stars[i].x = random(-width, width);
      stars[i].y = random(-height, height);
      stars[i].z = random(1, width);  // depth
      stars[i].vz = random(5, 15) / 10.0;  // speed varies
      stars[i].brightness = random(128, 255);
    }
    initialized = true;
  }

  clear();

  // Update and draw stars
  for (int i = 0; i < numStars; i++) {
    #ifdef ESP8266 
      if (i % 20 == 0) yield();
    #endif
    
    // Move star closer (increase z)
    stars[i].z -= stars[i].vz;
    
    // If star passes the viewer, reset it to the back
    if (stars[i].z <= 0) {
      stars[i].x = random(-width, width);
      stars[i].y = random(-height, height);
      stars[i].z = width;
      stars[i].vz = random(5, 15) / 10.0;
      stars[i].brightness = random(128, 255);
    }
    
    // Project 3D position to 2D screen
    float k = width / stars[i].z;  // perspective factor
    int sx = (int)(stars[i].x * k + centerX);
    int sy = (int)(stars[i].y * k + centerY);
    
    // Calculate previous position for streak effect
    float prevZ = stars[i].z + stars[i].vz;
    float prevK = width / prevZ;
    int px = (int)(stars[i].x * prevK + centerX);
    int py = (int)(stars[i].y * prevK + centerY);
    
    // Draw star only if on screen
    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
      // Calculate brightness based on depth (closer = brighter)
      uint8_t brightness = (uint8_t)(stars[i].brightness * (1.0 - stars[i].z / width));
      
      // Star color: white/blue tint
      uint8_t r = brightness;
      uint8_t g = brightness;
      uint8_t b = min(255, brightness + 30);  // slight blue tint
      
      uint16_t color = display->color565(r, g, b);
      
      // Draw motion streak for faster stars
      if (stars[i].vz > 0.8 && px >= 0 && px < width && py >= 0 && py < height) {
        display->drawLine(px, py, sx, sy, color);
      } else {
        // Draw single pixel for slower stars
        display->drawPixel(sx, sy, color);
        
        // Draw larger stars when they're closer
        if (stars[i].z < width / 4) {
          if (sx + 1 < width) display->drawPixel(sx + 1, sy, color);
          if (sy + 1 < height) display->drawPixel(sx, sy + 1, color);
        }
      }
    }
  }

  drawTimeWithBackground();
  showBuffer();
}

