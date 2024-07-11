#include "variable_set_triangles.h"

void setup() {
  Serial.begin(115200);
  Wire.begin(ALT_SDA, ALT_SCL);

  initializePWM(LED_PIN);
  setPWM(LED_PIN, minPwm);

  ts.begin();
  Serial.println("FT6336U инициализиран");

  tft.begin();
  tft.setRotation(3);
  initializeScreen();
  drawTriangles();
  updatePwmDisplay();
}

void loop() {
  touches = ts.read_touch_number();
  if (touches > 0) {
    touches = 0;
    uint16_t x = ts.read_touch1_x();
    uint16_t y = ts.read_touch1_y();
    uint16_t newX = 480 - y;
    uint16_t newY = x;
    handleTouch(newX, newY);
    delay(250);
  }
}
