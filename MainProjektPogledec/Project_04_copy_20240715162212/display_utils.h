//Project_03
#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <TFT_eSPI.h>
#include "memory_utils.h"

extern TFT_eSPI tft; // Деклариране на външен обект за дисплея

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
  uint16_t colorBackIP = tft.color565(247, 247, 0); // жълт цвят
  tft.fillRect(80, 0, 204, 22, colorBackIP);
  tft.setCursor(82, 4);
  tft.setTextColor(TFT_BLUE, colorBackIP);
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
  tft.print((char)247); // Unicode за градуса символ (°)
  tft.print("C"); // Добавете 'C' за Целзий
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
  dispRoundRect(100, 95, 60, 25, 5, circleColor, borderColor, numberColor, temp1);
  dispRoundRect(336, 65, 60, 25, 5, circleColor, borderColor, numberColor, temp2);
  dispRoundRect(324, 193, 60, 25, 5, circleColor, borderColor, numberColor, temp3);

  // Визуализация на паметта и брояча най-долу
  if (!memoryTextDisplayed) {
    tft.fillRect(80, tft.height() - 20, 320, 20, colorBack);
    tft.setCursor(86, tft.height() - 17);
    tft.setTextColor(TFT_RED, colorBack);
    tft.setTextSize(2);
    tft.printf("Free heap:%6u Counter:%1d", freeHeap, counter);
    memoryTextDisplayed = true;
    prevFreeHeap = freeHeap;
  } 
  else 
  {
    if (freeHeap != prevFreeHeap) {
      tft.fillRect(203, tft.height() - 20, 78, 20, TFT_BLUE);
      tft.setCursor(207, tft.height() - 17);
      tft.setTextColor(TFT_YELLOW, TFT_BLUE);
      tft.setTextSize(2);
      tft.printf("%6u", freeHeap);
      prevFreeHeap = freeHeap;
    }

    tft.fillRect(383, tft.height() - 20, 30, 20, TFT_BLUE);
    tft.setCursor(387, tft.height() - 17);
    tft.setTextColor(TFT_YELLOW, TFT_BLUE);
    tft.setTextSize(2);
    tft.printf("%2d", counter);
  }
}



#endif // DISPLAY_UTILS_H
