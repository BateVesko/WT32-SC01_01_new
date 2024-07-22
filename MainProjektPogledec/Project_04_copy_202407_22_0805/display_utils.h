//Project_03
#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <TFT_eSPI.h>
#include "memory_utils.h"
#include "ColorUtils.h"

extern TFT_eSPI tft; // Деклариране на външен обект за дисплея
extern int myPwm;  // % въздухообмен спрямо обема на помещението за час

// Глобална променлива за предишната стойност на свободната памет
uint32_t prevFreeHeap = 0;

// Глобални променливи за температурите
int temp1 = 0;
int temp2 = 0;
int temp3 = 0;

// Флаг за показване дали текстът за паметта е вече изписан
bool memoryTextDisplayed = false;

int randomTemperature() {
  //return (rand() % 45) - 9; // Генерира случайно число между -9 и 35
  return (rand() % 5) + 18; // Генерира случайно число между 18 и 23
}

// Функция за изобразяване на IP адреса
void displayIPAddress() {
  //uint16_t colorBackIP = tft.color565(247, 247, 0); // жълт цвят
  tft.fillRect(80, 0, 320, 24, secondBackgrountColor);
  tft.fillRoundRect(84, 3, 30, 20, 5, mainBackgrountColor);
  tft.drawRoundRect(84, 3, 30, 20, 5, mainBorderColor); 
  tft.fillRoundRect(119, 3, 178, 20, 5, mainBackgrountColor);
  tft.drawRoundRect(119, 3, 178, 20, 5, mainBorderColor); 
  tft.drawLine(80, 24, 400, 24, mainBorderColor);
  tft.setCursor(87, 6);
  tft.setTextColor(mainBorderColor);
  tft.setTextSize(2);
  tft.printf("IP %s", WiFi.localIP().toString().c_str());
}

void dispRoundRect(int pL, int pT, int rW, int rH, int rR, uint16_t rC, uint16_t bC, uint16_t tC, int temp) {
  tft.fillRoundRect(pL, pT, rW, rH, rR, rC); // Запълнен правоъгълник с радиус на закръгляне 5 пиксела
  tft.drawRoundRect(pL, pT, rW, rH, rR, bC); // Граница на правоъгълник с радиус на закръгляне 5 пиксела
  tft.setCursor(pL+8, pT+5);
  tft.setTextColor(tC);
  tft.setTextSize(2);
  tft.printf("%2d", temp);
  // tft.print((char)247); // Unicode за градуса символ (°)
  // tft.print("C"); // Добавете 'C' за Целзий
  drawGradusCircle(tft, 2);  // Размерът на шрифта е 8 пиксела по размер на шрифта
}

// Функция за изобразяване на информация за паметта и брояча
void displayMemoryInfo(int counter) {
  uint16_t colorBack = tft.color565(0, 247, 247);  // син цят
  uint32_t freeHeap = ESP.getFreeHeap();
  //freeHeap += 10 * counter * counter;

  // Генериране на случайни температури
  temp1 = randomTemperature();
  temp2 = randomTemperature();
  temp3 = randomTemperature();

  // Изобразяване на температурите с заоблено поле
  uint16_t circleColor = TFT_BLUE;
  uint16_t borderColor = TFT_RED;
  uint16_t numberColor = tft.color565(255, 255, 0); // жълт цвят

// Изобразяване на температурите
  dispRoundRect(100, 95, 60, 25, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp1);
  dispRoundRect(336, 65, 60, 25, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp2);
  dispRoundRect(324, 193, 60, 25, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp3);

  // Визуализация на паметта и брояча най-долу
  if (!memoryTextDisplayed) {
    tft.fillRect(80, tft.height() - 24, 320, 27, secondBackgrountColor);
    tft.drawLine(80, tft.height() - 25, 400, tft.height() - 25, mainBorderColor);
    tft.setCursor(82, tft.height() - 19);
    tft.setTextColor(mainTextColor);
    tft.setTextSize(2);
    //tft.printf("Free heap:%6u Counter:%1d", freeHeap, counter);
    tft.printf("Free heap:%6u Count:%1d", freeHeap, counter);
    memoryTextDisplayed = true;
    prevFreeHeap = freeHeap;
  } 
  else 
  {
    if (freeHeap != prevFreeHeap) {
      tft.fillRoundRect(201, tft.height() - 22, 80, 20, 5, mainBackgrountColor);
      tft.drawRoundRect(201, tft.height() - 22, 80, 20, 5, mainBorderColor);
      tft.setCursor(205, tft.height() - 19);
      tft.setTextColor(mainTextColor);
      tft.setTextSize(2);
      tft.printf("%6u", freeHeap);
      prevFreeHeap = freeHeap;
    }

    tft.fillRoundRect(358, tft.height() - 22, 40, 20, 5, mainBackgrountColor);
    tft.drawRoundRect(358, tft.height() - 22, 40, 20, 5, mainBorderColor);
    tft.setCursor(361, tft.height() - 19);
    tft.setTextColor(mainTextColor);
    tft.setTextSize(2);
    tft.printf("%3d", counter);
  }
}

// Изобразява пояснителен текст на два ред
void displayTextInfo(){  // Air exchange of 30% of the room's volume per hour.
  uint16_t colorBack = secondBackgrountColor;  //tft.color565(0, 240, 240);  //  #f4d58d  ; uint16_t lightBlue = 0x7D7C;
  uint16_t borderColor = TFT_BLUE;  //tft.color565(141, 8, 1);  // #8d0801
  uint16_t textColor = TFT_BLUE;  //tft.color565(141, 8, 1);  // #8d0801
  tft.fillRoundRect(85, 174, 204, 51, 5, colorBack); // Запълнен правоъгълник с радиус на закръгляне 10 пиксела
  tft.drawRoundRect(85, 174, 204, 51, 5, borderColor); // Граница на правоъгълник с радиус на закръгляне 10 пиксела
  tft.setCursor(98, 185);
  tft.setTextColor(textColor);
  tft.setTextSize(2);
  tft.printf("Ventilated:");
  tft.fillRoundRect(226, 180, 54, 25, 5, mainBackgrountColor); // Запълнен правоъгълник с радиус на закръгляне 5 пиксела
  tft.drawRoundRect(226, 180, 54, 25, 5, TFT_BLUE); // Граница на правоъгълник с радиус на закръгляне 5 пиксела
  tft.setTextColor(TFT_BLUE);
  tft.printf("%3d%%\n", myPwm);
  tft.setCursor(94, 209);
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.print("/of the room's volume per hour/");
}

#endif // DISPLAY_UTILS_H
