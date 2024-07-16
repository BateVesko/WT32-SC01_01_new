//Project_03
#ifndef TOUCH_UTILS_H
#define TOUCH_UTILS_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>
#include <variable_set_triangles.h>
//#include <PWM_generator.h>
//#include <button_rect.h>

// Дефиниране на пиновете и обектите
#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39
#define LED_PIN 25  // Използваме GPIO25 за светодиод

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);

extern char direction_fan;
extern int counter;
extern uint16_t posLeft;
extern uint16_t posTop;
extern uint8_t widthTriangle;
extern uint8_t heightTriangle;
extern uint8_t distTriangle;

uint8_t touches;
uint16_t newX = 0;
uint16_t newY = 0;
bool settingMode = false;


void handleTouch() {                 // Примерна функция за обработка на тъч събития
  touches = ts.read_touch_number();  // проверка за докосване на тъча
  if (touches > 0) {
    //touches = 0;
    uint16_t x = ts.read_touch1_x();
    uint16_t y = ts.read_touch1_y();
    newX = 480 - y;
    newY = x;
    Serial.print("X: ");
    Serial.print(newX);
    Serial.print(" Y: ");
    Serial.println(newY);
    //handleTouch(newX, newY);  // -> variable_set_triangles.h
    delay(200);  // интервал между две възможни докосвания на тъча

    // Проверка на различни зони на екрана и извършване на действия
    if (newX > 0 && newX < 80 && newY > 0 && newY < 80) {
      Serial.println("Touched earth icon");
      counter = 0;
    } else if (newX > 0 && newX < 80 && newY > 80 && newY < 160) {
      Serial.println("Touched fan icon");
      if (settingMode) {
        deleteTriangles();
        settingMode = !settingMode;
      } else {
        drawTriangles();
        updatePwmDisplay();
        handleTouchTriangles(newX, newY);
        settingMode = !settingMode;
      }
    } else if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + 2 * heightTriangle + distTriangle && settingMode){
      handleTouchTriangles(newX, newY);
    }
  }
}

#endif  // TOUCH_UTILS_H
