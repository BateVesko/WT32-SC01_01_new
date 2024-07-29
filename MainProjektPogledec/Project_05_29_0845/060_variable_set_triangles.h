// variable_set_triangles.h
#ifndef VARIABLE_SET_TRIANGLES_H
#define VARIABLE_SET_TRIANGLES_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>
#include <080_PWM_generator.h>
#include "030_touch_utils.h"
//#include <button_rect.h>

// Дефиниране на пиновете и обектите
// #define ALT_SDA 18
// #define ALT_SCL 19
// #define TFT_RST -1
// #define TFT_INT 39
#define LED_PIN 25  // Използваме GPIO25 за светодиод

// FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);
//TFT_eSPI tft = TFT_eSPI();

//extern TFT_eSPI tft;

uint16_t posLeft = 198;
uint16_t posTop = 53;
uint8_t textSize = 3;

//extern bool startBug;
//extern unsigned long currentTime;
//extern unsigned long lastBugTime;
//extern uint8_t countBug;

extern bool rotateGo;
extern uint16_t pwmPeriod;

uint8_t widthTriangle = 4 * 7 * textSize;
uint8_t heightTriangle = widthTriangle;
uint8_t distTriangle = 15 * textSize;
uint16_t bgColor = tft.color565(0, 255, 255);
uint16_t txtColor = TFT_BLUE;

//uint8_t touches;
int8_t minPwm = 0;
int8_t maxPwm = 100;
int myPwm = minPwm;
int8_t pwmStep = 10;
String myPwmString = String(myPwm) + "%";
bool fanOn = false;


// void initializeScreen() {  // функциията не се ползва
//   tft.fillScreen(bgColor);
//   if (bgColor != TFT_WHITE) {  // ако фона не е бял прави буквите жълти. Иначе остават черни.
//     txtColor = TFT_YELLOW;
//   }
//   tft.setTextColor(txtColor);  // цвят на текста
//   tft.setTextDatum(TC_DATUM);  // текст в позиция горе/средата спрямо зададените координати
//   tft.setTextSize(textSize);  // размер на текста
// }

void drawTriangles() {
  tft.fillTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, TFT_GREEN);
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, txtColor);
  tft.fillTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_RED);
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, txtColor);
}

void updatePwmDisplay() {
  tft.setTextDatum(TC_DATUM);                                                                           // текст в позиция горе/средата спрямо зададените координати
  tft.setTextSize(textSize);                                                                            // размер на текста
  tft.setTextColor(txtColor);                                                                          // Задаваме сини букви на син фон
  tft.fillRect(posLeft, posTop + heightTriangle + 4 * textSize, widthTriangle, 7 * textSize, bgColor);  // зачиства старото показание
  tft.drawString(myPwmString, posLeft + widthTriangle / 2, posTop + heightTriangle + 4 * textSize);     // изписва новото показание
}

void deleteTriangles() {
  tft.fillRect(posLeft, posTop, widthTriangle + 1, 2 * heightTriangle + distTriangle + 1, bgColor);  // зачиства старото показание
}

void handleTouchTriangles(uint16_t newX, uint16_t newY) {
  Serial.println("Triangle");                                                                                 // обработка на тъча
  if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + heightTriangle) {  // ако е горна стрелка
    myPwm += pwmStep;                                                                                         // увеличава PWM със стъпката
    myPwm = constrain(myPwm, minPwm, maxPwm);                                                                 // държи стойността между двете граници
    pwmPeriod = map(myPwm, 10, 100, 300, 30);                                                                 // мащабира myPwm към ppwmPeriod
    rotateGo = true;                                                                                          // разрешава въртенето
    myPwmString = String(myPwm) + "%";                                                                        // добавя дименсия % след числото
    updatePwmDisplay();
    setPWM(LED_PIN, myPwm);
    Serial.println("Up");  // -> по-горе
    // countBug = 0;              // нулиране на брояча за бъг
    // displayBugTime(countBug);  // визуализиране брояча на бъг
    // startBug = false;  //  спира брояча
  } else if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop + heightTriangle + distTriangle && newY < posTop + 2 * heightTriangle + distTriangle) {
    myPwm -= pwmStep;                          // намалява PWM със стъпката
    myPwm = constrain(myPwm, minPwm, maxPwm);  // държи стойността между двете граници
    pwmPeriod = map(myPwm, 10, 100, 300, 30);  // мащабира myPwm към ppwmPeriod
    if (myPwm == 0) rotateGo = false;
    myPwmString = String(myPwm) + "%";  // добавя дименсия % след числото
    updatePwmDisplay();                 // -> по-горе
    setPWM(LED_PIN, myPwm);             // зарежда текущ PWM към вентилатора
    Serial.println("Down");
    // countBug = 0;              // нулиране на брояча за бъг
    // displayBugTime(countBug);  // визуализиране брояча на бъг
    // startBug = false;  //  спира брояча
  }
  // else if (newX > 50 && newX < 50 + widthTriangle && newY > posTop + 2 * heightTriangle + distTriangle + 4 * textSize && newY < posTop + 2 * heightTriangle + distTriangle + 4 * textSize + 3 * 7 * textSize) {
  //   tft.fillRect(50, posTop + 2 * heightTriangle + distTriangle + 4 * textSize, widthTriangle, 3 * 7 * textSize, bgColor);  // Изчистване на зоната на бутона
  //   drawButton_01(fanOn);                                                                                                   // -> button_rect.h
  //   fanOn = !fanOn;
  //   countBug = 0;              // нулиране на брояча за бъг
  //   displayBugTime(countBug);  // визуализиране брояча на бъг
  //   startBug = true;  //  включва брояча
  //   lastBugTime = currentTime;
  // }

  // if (!fanOn) {  // вкл/изкл PWM
  //   setPWM(LED_PIN, 0);
  // } else {
  //   setPWM(LED_PIN, myPwm);
  // }
}

#endif  // VARIABLE_SET_TRIANGLES_H
