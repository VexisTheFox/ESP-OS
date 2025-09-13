# Simple OLED GUI Example for ESP8266

This project uses the Adafruit SSD1306 library to display a simple message on a 128x64 OLED display (SSD1306) connected to your ESP8266.

## Wiring
- SDA -> D2 (GPIO4)
- SCL -> D1 (GPIO5)
- VCC -> 3.3V
- GND -> GND

## PlatformIO Library Installation
Add these lines to your `platformio.ini`:

```
library_dependencies =
    adafruit/Adafruit SSD1306@^2.5.7
    adafruit/Adafruit GFX Library@^1.11.9
```

## Example Code
See `src/main.cpp` for a simple test GUI.
