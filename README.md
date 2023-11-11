# LedMatrix
This projects runs a HUB75 RGB LED matrix panel with either an ESP8266 or ESP32

# License
[see license](LICENSE.txt)

# build
You need to use PlatformIO to build and upload to the ESP

# Wifi
You need to establish a connection to your WIFI. There are plenty of documentation out there how to do it ;).

# Used Libraries
## LedMatrix
I used two different kind of libraries for the different chips. For the older ESP8266 I used the lib [PxMatrix](https://github.com/2dom/PxMatrix) and for the ESP32 the more powerful [ESP32-HUB75-MatrixPanel-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA). I recommend to use the later one, because it enables you to use a 2x2 matrix with four 64x32 displays which is amazing for creating larger display. 

# 


