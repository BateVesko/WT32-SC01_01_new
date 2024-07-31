// ColorUtils.h
#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


extern TFT_eSPI tft; // Деклариране на външен обект за дисплея

// Функция за конвертиране на RGB888 към RGB565
uint16_t rgb888ToRgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Функция за конвертиране на шестнадесетичен цвят към RGB565
uint16_t hexToRgb565(const char* hex) {
  uint8_t r = 0, g = 0, b = 0;
  if (hex[0] == '#') {
    hex++;
  }
  if (strlen(hex) == 6) {
    r = strtol(hex, NULL, 16) >> 16;
    g = (strtol(hex, NULL, 16) & 0x00FF00) >> 8;
    b = (strtol(hex, NULL, 16) & 0x0000FF);
  }
  return rgb888ToRgb565(r, g, b);
}

// Функция за рисуване на символ за градус като малко кръгче, последвано от 'C'
// Диаметърът на кръгчето за градус е 8*(размер на шрифта-1) пиксела с дебелина (размер на шрифта-1)пиксела
void drawGradusCircle(TFT_eSPI& tft, int fontSize) {
  int x = tft.getCursorX();
  int y = tft.getCursorY();
  int circleRadius = fontSize + 1;  // Радиусът на кръгчето е половината от размера на шрифта
  int offsetY = fontSize / 2;       // Вертикално изместване на кръгчето спрямо основната линия

  // Вземане на текущия цвят на текста
  uint16_t color = tft.textcolor;

  // Рисуване на кръгчето
  //tft.drawCircle(x + circleRadius, y - offsetY, circleRadius, color);
  for (int i = 0; i <= circleRadius - 3; i++) {
    tft.drawCircle(x + circleRadius, y + circleRadius - 1, circleRadius + i, color);
    //tft.drawCircle(x + fontSize/2, y, circleRadius + i, color);
  }

  // Рисуване на 'C' след кръгчето
  tft.setCursor(x + circleRadius * 3, y);
  tft.print("C");
}


uint16_t mainBackgrountColor = rgb888ToRgb565(0, 255, 255);
uint16_t secondBackgrountColor = rgb888ToRgb565(173, 216, 230);
uint16_t mainBorderColor = rgb888ToRgb565(0, 0, 255);
uint16_t mainTextColor = rgb888ToRgb565(0, 0, 255);

void visualizeMainLines() {
  tft.drawLine(80, 0, 80, 320, mainBorderColor);
  tft.drawLine(tft.width()-80, 0, tft.width()-80, 320, mainBorderColor);
  tft.drawLine(80, 25, tft.width()-80, 25, mainBorderColor);
  tft.drawLine(80, tft.height() - 25, tft.width()-80, tft.height() - 25, mainBorderColor);
}



#endif  // COLOR_UTILS_H
