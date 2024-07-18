#ifndef FRAME_GENERATOR_H
#define FRAME_GENERATOR_H

#include <TFT_eSPI.h>
//#include "fan.h"

extern TFT_eSPI tft;
extern bool settingMode;
//extern PNG png;

void buttonFrameGenerator(int x, int y, int ic_w, int ic_h, uint16_t frame_color, int frame_thickness) {
  for (int i = 0; i < frame_thickness; i++) {
    tft.drawRoundRect(x + i, y + i, ic_w - 2 * i, ic_h - 2 * i, 6 - i, frame_color);
  }
}

#endif
