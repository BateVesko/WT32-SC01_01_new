//Project_03
#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <TFT_eSPI.h>
#include "100_memory_utils.h"
#include "090_ColorUtils.h"

extern TFT_eSPI tft;  // Деклариране на външен обект за дисплея
extern int myPwm;     // % въздухообмен спрямо обема на помещението за час
extern String networks[20];
extern uint16_t countReconections;

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
  return (rand() % 5) + 18;  // Генерира случайно число между 18 и 23
}

// Функция за изобразяване на IP адреса
void displayIPAddress() {
  //uint16_t colorBackIP = tft.color565(247, 247, 0); // жълт цвят
  tft.fillRect(80, 0, 320, 25, secondBackgrountColor);
  tft.fillRoundRect(84, 3, 30, 20, 5, mainBackgrountColor);    // Заоблен правоъгълник за "IP"
  tft.drawRoundRect(84, 3, 30, 20, 5, mainBorderColor);        // Заоблен контур на правоъгълник за "IP"
  tft.fillRoundRect(119, 3, 178, 20, 5, mainBackgrountColor);  // Заоблен правоъгълник за IPадреса
  tft.drawRoundRect(119, 3, 178, 20, 5, mainBorderColor);      // Заоблен контур на правоъгълник за IPадреса
  visualizeMainLines();                                        // -> ColorUtils
  // tft.drawLine(80, 24, 400, 24, mainBorderColor);
  // tft.drawLine(80, 271, 400, 271, mainBorderColor);
  tft.setCursor(87, 6);  // Позиционира курсора
  tft.setTextColor(mainBorderColor);
  tft.setTextSize(2);
  tft.printf("IP %s", WiFi.localIP().toString().c_str());  // Изписва "IP <адреса>"
}

// Функция за изобразяване на заоблен правоъгълник с температура и знак за градус Целзий
void dispRoundRect(int pL, int pT, int rW, int rH, int rR, uint16_t rC, uint16_t bC, uint16_t tC, int temp) {
  tft.fillRoundRect(pL, pT, rW, rH, rR, rC);  // Запълнен правоъгълник с радиус на закръгляне rR пиксела
  tft.drawRoundRect(pL, pT, rW, rH, rR, bC);  // Граница на правоъгълник с радиус на закръгляне rR пиксела
  //tft.setCursor(pL+8, pT+5);
  tft.setCursor(pL + 8, pT + 4);
  tft.setTextColor(tC);
  tft.setTextSize(2);
  tft.printf("%2d", temp);
  // tft.print((char)247); // Unicode за градуса символ (°)
  // tft.print("C"); // Добавете 'C' за Целзий
  drawGradusCircle(tft, 2);  // -> ColorUtils.h - Диаметърът на кръгчето за градус е 8*(размер на шрифта-1) пиксела с дебелина (размер на шрифта-1)пиксела
}

// Функция за изобразяване на информация за паметта, брояча и темепературите
void displayMemoryInfo(int counter) {
  uint32_t freeHeap = ESP.getFreeHeap();

  // Генериране на случайни температури
  temp1 = randomTemperature();  // -> Abowe
  temp2 = randomTemperature();  // -> Abowe
  temp3 = randomTemperature();  // -> Abowe

  // Изобразяване на температурите
  dispRoundRect(118, 92, 58, 22, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp1);   // -> Abowe
  dispRoundRect(339, 64, 58, 22, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp2);   // -> Abowe
  dispRoundRect(325, 193, 58, 22, 5, secondBackgrountColor, mainBorderColor, mainTextColor, temp3);  // -> Abowe

  // Визуализация на паметта и брояча най-долу
  if (!memoryTextDisplayed) {  // Визуализира текста към стойностите само в началото. После пропуска за да не трепти екрана
    tft.fillRect(80, tft.height() - 25, 320, 27, secondBackgrountColor);
    tft.setCursor(82, tft.height() - 19);
    tft.setTextColor(mainTextColor);
    tft.setTextSize(2);
    tft.printf("Free heap:%6u Count:%1d", freeHeap, counter);
    visualizeMainLines();  // -> ColorUtils
    memoryTextDisplayed = true;
    prevFreeHeap = freeHeap;
  } else {                           // Визуализира само стойностите на променливите
    if (freeHeap != prevFreeHeap) {  // Визуализира само ако има промяна в стойността
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
void displayTextInfo() {                                // Air exchange of 30% of the room's volume per hour.
  uint16_t colorBack = secondBackgrountColor;           //tft.color565(0, 240, 240);  //  #f4d58d  ; uint16_t lightBlue = 0x7D7C;
  uint16_t borderColor = TFT_BLUE;                      //tft.color565(141, 8, 1);  // #8d0801
  uint16_t textColor = TFT_BLUE;                        //tft.color565(141, 8, 1);  // #8d0801
  tft.fillRoundRect(85, 174, 204, 51, 5, colorBack);    // Запълнен правоъгълник с радиус на закръгляне 10 пиксела
  tft.drawRoundRect(85, 174, 204, 51, 5, borderColor);  // Граница на правоъгълник с радиус на закръгляне 10 пиксела
  tft.setCursor(98, 185);
  tft.setTextColor(textColor);
  tft.setTextSize(2);
  tft.printf("Ventilated:");
  tft.fillRoundRect(226, 180, 54, 25, 5, mainBackgrountColor);  // Запълнен правоъгълник с радиус на закръгляне 5 пиксела
  tft.drawRoundRect(226, 180, 54, 25, 5, TFT_BLUE);             // Граница на правоъгълник с радиус на закръгляне 5 пиксела
  tft.setTextColor(TFT_BLUE);
  tft.printf("%3d%%\n", myPwm);
  tft.setCursor(94, 209);
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.print("/of the room's volume per hour/");
}

void displayNetworks() {
  uint16_t colorBack = secondBackgrountColor;  //tft.color565(0, 240, 240);  //  #f4d58d  ; uint16_t lightBlue = 0x7D7C;
  uint16_t borderColor = TFT_BLUE;             //tft.color565(141, 8, 1);  // #8d0801
  uint16_t textColor = TFT_BLUE;               //tft.color565(141, 8, 1);  // #8d0801
  tft.setTextColor(textColor);
  tft.setTextSize(2);
  int y = 40;  // Начална позиция по y
  for (int i = 0; i < 20 && networks[i] != ""; ++i) {
    tft.fillRoundRect(90, y - 3, 80 + networks[i].length() * 12, 20, 5, colorBack);    // Запълнен правоъгълник с радиус на закръгляне 10 пиксела
    tft.drawRoundRect(90, y - 3, 80 + networks[i].length() * 12, 20, 5, borderColor);  // Граница на правоъгълник с радиус на закръгляне 10 пиксела
    tft.setCursor(100, y);                                                             // Задава курсора на (20, y)
    tft.printf("Net%d:%s", i + 1, networks[i].c_str());                                // Принтира името на мрежата
    y += 25;                                                                           // Преместване на следващия ред надолу
  }
}

void displayContReconections() {
  uint16_t colorBack = secondBackgrountColor;  //tft.color565(0, 240, 240);  //  #f4d58d  ; uint16_t lightBlue = 0x7D7C;
  uint16_t borderColor = TFT_BLUE;             //tft.color565(141, 8, 1);  // #8d0801
  uint16_t textColor = TFT_BLUE;               //tft.color565(141, 8, 1);  // #8d0801
  tft.setTextColor(textColor);
  tft.setTextSize(2);
  tft.setCursor(100, 260);
  tft.printf("Cont reconections: %d", countReconections);
}

#endif  // DISPLAY_UTILS_H
