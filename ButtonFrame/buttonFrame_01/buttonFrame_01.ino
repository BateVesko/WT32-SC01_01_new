#include <TFT_eSPI.h>
#include "fan.h"
#include "touch_utils.h"
#include "frame_generator.h"

TFT_eSPI tft = TFT_eSPI();  // Инициализация на дисплея

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  displayIcon(fanIcon);
}

void loop() {
  handleTouch();
}
