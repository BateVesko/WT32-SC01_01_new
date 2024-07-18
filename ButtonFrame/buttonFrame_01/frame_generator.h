#ifndef FRAME_GENERATOR_H
#define FRAME_GENERATOR_H

#include <TFT_eSPI.h>
#include "fan.h"

extern TFT_eSPI tft;

void buttonFrameGenerator(int x, int y, int ic_w, int ic_h, uint16_t frame_color, int frame_thickness, bool isTouched) {
  if (isTouched) {
    for (int i = 0; i < frame_thickness; i++) {
      tft.drawRect(x + i, y + i, ic_w - 2 * i, ic_h - 2 * i, frame_color);
    }
  } else {
    tft.fillRect(x, y, ic_w, ic_h, TFT_BLACK); // Премахване на рамката
    displayIcon(fanIcon); // Преначертаване на иконата
  }
}

#endif
