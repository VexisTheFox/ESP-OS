#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Simple 2x2 Bayer matrix for dithering
const uint8_t bayer[2][2] = {
  {0, 2},
  {3, 1}
};

void drawDitheredRect(int x, int y, int w, int h, uint8_t brightness) {
  // brightness: 0 (off) to 3 (full on)
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      uint8_t threshold = bayer[i % 2][j % 2];
      if (brightness > threshold) {
        display.drawPixel(x + i, y + j, SSD1306_WHITE);
      } else {
        display.drawPixel(x + i, y + j, SSD1306_BLACK);
      }
    }
  }
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  // Draw rectangles with different simulated brightness
  drawDitheredRect(10, 10, 30, 30, 1); // dim
  drawDitheredRect(50, 10, 30, 30, 2); // medium
  drawDitheredRect(90, 10, 30, 30, 3); // bright
  display.display();
}

void loop() {
  // nothing
}
