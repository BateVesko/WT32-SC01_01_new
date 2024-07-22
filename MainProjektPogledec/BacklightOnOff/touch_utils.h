//Project_03
#ifndef TOUCH_UTILS_H
#define TOUCH_UTILS_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>
//#include <variable_set_triangles.h>
//#include "frame_generator.h"
//#include "ColorUtils.h"

// Дефиниране на пиновете и обектите
#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39
#define LED_PIN 25  // Използваме GPIO25 за светодиод

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);

extern TFT_eSPI tft;  // Деклариране на външен обект за дисплея.

// extern char direction_fan;
// extern int counter;
// extern uint16_t posLeft;
// extern uint16_t posTop;
// extern uint8_t widthTriangle;
// extern uint8_t heightTriangle;
// extern uint8_t distTriangle;

//extern void reloadIcon(int numIcon);
//extern int currentIconIndex_fan;

uint8_t touches;
uint16_t newX = 0;
uint16_t newY = 0;
bool backlightOn = false;
bool isTouched = false;
//bool settingMode = false;
//bool wiFiMode = false;
//bool fanMode = false;

void visualizeTouch(){
  Serial.print("X: ");
  Serial.print(newX);
  Serial.print(" Y: ");
  Serial.println(newY);
  
  tft.setCursor(10, 60);
  tft.print("X: ");
  tft.fillRect(46,60,36,14,TFT_BLUE);
  tft.print(newX);

  tft.setCursor(90, 60);
  tft.print(" Y: ");
  tft.fillRect(138,60,36,17,TFT_BLUE);
  tft.println(newY);
}

void handleTouch() {                 // Примерна функция за обработка на тъч събития
  touches = ts.read_touch_number();  // проверка за докосване на тъча
  if (touches > 0) {  
    isTouched = true;
    // //touches = 0;
    uint16_t x = ts.read_touch1_x();
    uint16_t y = ts.read_touch1_y();
    newX = 480 - y;
    newY = x;
    visualizeTouch();
    if(!backlightOn){// Има докосване и дисплея свети. Обработва се докосването
      backlightOn = true;
      isTouched = true;
      digitalWrite(TFT_BL, HIGH);  // дисплея светва
    }

    // //handleTouch(newX, newY);  // -> variable_set_triangles.h
    // delay(200);  // интервал между две възможни докосвания на тъча

    // // Проверка на различни зони на екрана и извършване на действия
    // if (newX > 0 && newX < 80 && newY > 0 && newY < 80 && (settingMode == wiFiMode)) {
    //   Serial.println("Touched earth icon");
    //   if (wiFiMode) {
    //     settingMode = !settingMode;
    //     wiFiMode = !wiFiMode;
    //     reloadIcon(8);  // -> .ino
    //     reloadIcon(0);  // -> .ino
    //     tft.fillRect(80, 165, 213, 126, mainBackgrountColor);
    //     displayTextInfo();  // -> display_units
    //     tft.drawLine(80, 0, 80, 320, mainBorderColor);
    //     tft.drawLine(400, 0, 400, 320, mainBorderColor);
    //   } else {
    //     settingMode = !settingMode;
    //     wiFiMode = !wiFiMode;
    //     buttonFrameGenerator(0, 0, 80, 80, TFT_RED, 4);
    //     tft.fillRect(80, 30, 320, 261, mainBackgrountColor);  // зачиства разпределнието
    //     tft.drawLine(80, 0, 80, 320, mainBorderColor);
    //   }
    //   counter = 0;
    // } else if (newX > 0 && newX < 80 && newY > 80 && newY < 160 && (settingMode == fanMode)) {
    //   Serial.println("Touched fan icon");
    //   if (settingMode) {
    //     settingMode = !settingMode;
    //     fanMode = !fanMode;
    //     deleteTriangles();  // -> variable_set_triangles
    //     //buttonFrameDeleter(0, 80);
    //     //currentIconIndex_fan = 0;
    //     reloadIcon(1);  // -> .ino
    //     reloadIcon(8);  // -> .ino
    //     tft.fillRect(80, 165, 213, 126, mainBackgrountColor);
    //     displayTextInfo();  // -> display_units
    //     tft.drawLine(80, 0, 80, 320, mainBorderColor);
    //     tft.drawLine(400, 0, 400, 320, mainBorderColor);
    //   } else {
    //     settingMode = !settingMode;
    //     fanMode = !fanMode;
    //     tft.fillRect(80, 30, 320, 261, mainBackgrountColor);  // зачиства разпределнието
    //     drawTriangles();                                      // -> variable_set_triangles
    //     updatePwmDisplay();                                   // -> variable_set_triangles
    //     handleTouchTriangles(newX, newY);                     // -> variable_set_triangles
    //     //currentIconIndex_fan = 0;
    //     //reloadIcon(1);
    //     buttonFrameGenerator(0, 81, 80, 80, TFT_RED, 4);
    //     tft.drawLine(80, 0, 80, 320, mainBorderColor);
    //   }
    // } else if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + 2 * heightTriangle + distTriangle && settingMode) {
    //   handleTouchTriangles(newX, newY);
    // }
    delay(150);  // Малка пауза за избягване на многократни регистрации на докосване
  } 
}

#endif  // TOUCH_UTILS_H
