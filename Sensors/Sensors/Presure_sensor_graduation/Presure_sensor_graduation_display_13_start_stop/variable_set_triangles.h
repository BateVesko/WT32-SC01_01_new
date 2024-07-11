// variable_set_triangles.h
#ifndef VARIABLE_SET_TRIANGLES_H
#define VARIABLE_SET_TRIANGLES_H

#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>
#include <PWM_generator.h>
#include <button_rect.h>

// Дефиниране на пиновете и обектите
#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39
#define LED_PIN 25  // Използваме GPIO25 за светодиод

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);
//TFT_eSPI tft = TFT_eSPI();

uint16_t posLeft = 378;
uint16_t posTop = 18;
uint8_t textSize = 3;


uint8_t widthTriangle = 4 * 7 * textSize;
uint8_t heightTriangle = widthTriangle;
uint8_t distTriangle = 15 * textSize;
uint32_t bgColor = TFT_BLUE;
uint32_t txtColor;

uint8_t touches;
int8_t minPwm = 0;
int8_t maxPwm = 100;
int myPwm = minPwm;
int8_t pwmStep = 10;
String myPwmString = String(myPwm) + "%";
bool fanOn = false;

void initializeScreen() {
  tft.fillScreen(bgColor);
  if (bgColor != TFT_WHITE) {
    txtColor = TFT_YELLOW;
  }
  tft.setTextColor(txtColor);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(textSize);
}

void drawTriangles() {
  tft.fillTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, TFT_GREEN);
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, TFT_BLACK);
  tft.fillTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_RED);
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_BLACK);
}

void updatePwmDisplay() {
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(textSize);
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);  // Задаваме жълти букви на син фон
  tft.fillRect(posLeft, posTop + heightTriangle + 4 * textSize, widthTriangle, 7 * textSize, bgColor);
  tft.drawString(myPwmString, posLeft + widthTriangle / 2, posTop + heightTriangle + 4 * textSize);
}

void handleTouch(uint16_t newX, uint16_t newY) {
  if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + heightTriangle) {
    myPwm += pwmStep;
    myPwm = constrain(myPwm, minPwm, maxPwm);
    myPwmString = String(myPwm) + "%";
    updatePwmDisplay();  // -> по-горе
  } else if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop + heightTriangle + distTriangle && newY < posTop + 2 * heightTriangle + distTriangle) {
    myPwm -= pwmStep;
    myPwm = constrain(myPwm, minPwm, maxPwm);
    myPwmString = String(myPwm) + "%";
    updatePwmDisplay();  // -> по-горе
  } else if (newX > 50 && newX < 50 + widthTriangle && newY > posTop + 2 * heightTriangle + distTriangle + 4 * textSize && newY < posTop + 2 * heightTriangle + distTriangle + 4 * textSize + 3 * 7 * textSize) {
    drawButton_01(fanOn);  // -> button_rect.h
    fanOn = !fanOn;
    //drawButton_01(fanOn);
  }

  if (!fanOn) {
    setPWM(LED_PIN, 0);
  } else {
    setPWM(LED_PIN, myPwm);
  }
}

#endif  // VARIABLE_SET_TRIANGLES_H
