//Project_03
#ifndef TOUCH_UTILS_H
#define TOUCH_UTILS_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>
#include "020_display_utils.h"
#include "050_frame_generator.h"
#include "060_variable_set_triangles.h"
#include "090_ColorUtils.h"

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

extern void reloadIcon(int numIcon);

extern int currentIconIndex_fan;
extern int pwmLightUp;
extern int pwmLightDown;
extern void LightUp();
extern bool dimmingLightUp;
extern bool dimmingLightDown;

uint8_t touches;
uint16_t newX = 0;
uint16_t newY = 0;
bool settingMode = false;
bool wiFiMode = false;
bool fanMode = false;
bool backlightOn = false;
bool lampIsOn = false;
bool isTouched = false;

unsigned long lastTouchTime = 0;
const unsigned long touchInterval = 150;  // Интервал между две възможни докосвания на тъча

void handleTouch() {
  unsigned long currentTime = millis();

  touches = ts.read_touch_number();  // проверка за докосване на тъча
  if (touches > 0) {
    if (currentTime - lastTouchTime >= touchInterval) {
      isTouched = true;
      uint16_t x = ts.read_touch1_x();
      uint16_t y = ts.read_touch1_y();
      newX = 480 - y;
      newY = x;
      Serial.print("X: ");
      Serial.print(newX);
      Serial.print(" Y: ");
      Serial.println(newY);

      if (dimmingLightDown) {  
        dimmingLightDown = false;
        backlightOn = true;
        isTouched = true;
        pwmLightUp = pwmLightDown;
        dimmingLightUp = true;  // Разрешава включване на подсветката.
        return;
      }

      if (!backlightOn && !lampIsOn) {  // Има докосване и дисплея свети. Обработва се докосването
        backlightOn = true;
        isTouched = true;
        pwmLightUp = 0;
        dimmingLightUp = true;  // Разрешава включване на подсветката.
      }

      // Проверка на различни зони на екрана и извършване на действия
      if (newX > 0 && newX < 80 && newY > 0 && newY < 80 && (settingMode == wiFiMode)) {
        Serial.println("Touched earth icon");
        if (wiFiMode) {
          settingMode = !settingMode;
          wiFiMode = !wiFiMode;
          tft.fillRect(80, 25, 320, 270, mainBackgrountColor);  // зачиства изобразените мрежи
          reloadIcon(8);  // -> .ino визуализира разпределението
          reloadIcon(0);  // -> .ino визуализира земята
          displayTextInfo();  // -> display_units
          visualizeMainLines();  // -> ColorUtils
        } else {
          settingMode = !settingMode;
          wiFiMode = !wiFiMode;
          buttonFrameGenerator(0, 0, 80, 80, TFT_RED, 4);  // Изобразява червена рамка за "Активно" около earthIcon 
          tft.fillRect(80, 25, 320, 270, mainBackgrountColor);  // зачиства разпределнието
          visualizeMainLines();  // -> ColorUtils
          displayNetworks();
          displayContReconections();
        }
        counter = 0;
      } else if (newX > 0 && newX < 80 && newY > 80 && newY < 160 && (settingMode == fanMode)) {
        Serial.println("Touched fan icon");
        if (settingMode) {
          settingMode = !settingMode;
          fanMode = !fanMode;
          deleteTriangles();  // -> variable_set_triangles
          reloadIcon(1);      // -> .ino
          reloadIcon(8);      // -> .ino
          //tft.fillRect(80, 165, 213, 126, mainBackgrountColor);  // Изобразяване на правоъгълник върху бялото поле на чертежа.
          displayTextInfo();  // -> display_units
          visualizeMainLines();  // -> ColorUtils
          // tft.drawLine(80, 0, 80, 320, mainBorderColor);
          // tft.drawLine(400, 0, 400, 320, mainBorderColor);
          // tft.drawLine(80, 24, 400, 24, mainBorderColor);
          // tft.drawLine(80, 271, 400, 271, mainBorderColor);
        } else {
          settingMode = !settingMode;
          fanMode = !fanMode;
          tft.fillRect(80, 25, 320, 270, mainBackgrountColor);  // зачиства разпределнието
          drawTriangles();                                      // -> variable_set_triangles
          updatePwmDisplay();                                   // -> variable_set_triangles
          handleTouchTriangles(newX, newY);                     // -> variable_set_triangles
          buttonFrameGenerator(0, 81, 80, 80, TFT_RED, 4);
          visualizeMainLines();  // -> ColorUtils
          // tft.drawLine(80, 0, 80, 320, mainBorderColor);
          // tft.drawLine(80, 24, 400, 24, mainBorderColor);
          // tft.drawLine(80, 271, 400, 271, mainBorderColor);
        }
      } else if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + 2 * heightTriangle + distTriangle && settingMode) {
        handleTouchTriangles(newX, newY);
      } else if (newX > 0 && newX < 80 && newY > 240 && newY < 320 && !settingMode && !fanMode) {
        Serial.println("Touched lamp icon");
        lampIsOn = !lampIsOn;
        if (!lampIsOn) {
          reloadIcon(3);
        } else {
          reloadIcon(11);
        }
        Serial.println(lampIsOn);
      }

      lastTouchTime = currentTime;  // Актуализиране на времето на последното докосване
    }
  }
  return;
}


#endif  // TOUCH_UTILS_H
