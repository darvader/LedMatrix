@echo off
REM LED Matrix Release Build Script
REM This script builds firmware for both ESP8266 and ESP32 and copies to release folder

echo Building LED Matrix firmware...

REM Clean previous builds
echo Cleaning previous builds...
pio run --target clean

REM Build ESP32
echo Building for ESP32...
pio run --environment nodemcu-32s
if %errorlevel% neq 0 (
    echo ESP32 build failed!
    exit /b 1
)

REM Build ESP8266
echo Building for ESP8266...
pio run --environment nodemcuv2
if %errorlevel% neq 0 (
    echo ESP8266 build failed!
    exit /b 1
)

REM Copy firmware files
echo Copying firmware files...
copy .pio\build\nodemcu-32s\firmware.bin release\ESP32\
copy .pio\build\nodemcuv2\firmware.bin release\ESP8266\

REM Create version info
for /f "tokens=2 delims==" %%i in ('wmic os get localdatetime /value') do set datetime=%%i
set VERSION=%datetime:~0,8%_%datetime:~8,6%

echo Firmware built successfully!
echo ESP32 firmware: release\ESP32\firmware.bin
echo ESP8266 firmware: release\ESP8266\firmware.bin
echo Version: %VERSION%

echo.
echo To create a release archive:
echo mkdir release_%VERSION%
echo xcopy release\* release_%VERSION% /E /I /H /Y
echo tar -czf ledmatrix_%VERSION%.tar.gz release_%VERSION%

pause