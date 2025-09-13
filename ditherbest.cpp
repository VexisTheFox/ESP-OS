#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 4x4 Bayer matrix for more advanced dithering
const uint8_t bayer4x4[4][4] = {
  { 0,  8,  2, 10},
  {12,  4, 14,  6},
  { 3, 11,  1,  9},
  {15,  7, 13,  5}
};

// brightness: 0 (off) to 16 (full on)
void drawDitheredRect4x4(int x, int y, int w, int h, uint8_t brightness) {
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      uint8_t threshold = bayer4x4[i % 4][j % 4];
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
  drawDitheredRect4x4(5, 10, 28, 44, 4);   // dim
  drawDitheredRect4x4(37, 10, 28, 44, 8);  // medium
  drawDitheredRect4x4(69, 10, 28, 44, 12); // bright
  drawDitheredRect4x4(101, 10, 22, 44, 16); // full
  display.display();
}

void loop() {
  // nothing
}
