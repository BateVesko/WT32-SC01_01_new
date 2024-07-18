#ifndef TOUCH_UTILS_H
#define TOUCH_UTILS_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include "frame_generator.h"
#include "fan.h"

#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);

extern TFT_eSPI tft;

bool isTouched = false;
int x = 100;
int y = 100;
int ic_w = 80;
int ic_h = 80;
int frame_thickness = 2;
uint16_t frame_color = tft.color565(255, 0, 0); // Червен цвят

uint8_t touches;
uint16_t newX = 0;
uint16_t newY = 0;

void handleTouch() {
  touches = ts.read_touch_number();
  if (touches > 0) {
    uint16_t x_touch = ts.read_touch1_x();
    uint16_t y_touch = ts.read_touch1_y();
    newX = 480 - y_touch;
    newY = x_touch;

    if (newX > x && newX < (x + ic_w) && newY > y && newY < (y + ic_h)) {
      isTouched = !isTouched;
      buttonFrameGenerator(x, y, ic_w, ic_h, frame_color, frame_thickness, isTouched);
    }
    delay(200);  // интервал между две възможни докосвания на тъча
  }
}

#endif
