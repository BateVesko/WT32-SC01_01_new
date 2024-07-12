#ifndef BUTTON_RECT_H
#define BUTTON_RECT_H

extern uint16_t posLeft;
extern uint16_t posTop;
extern uint8_t textSize;

uint16_t curLeft;
uint16_t curTop;
uint16_t curWidth;
uint16_t curHeight;

uint32_t color;
String buttonText;

void drawButton_01(bool isOn) {
  if (isOn) {
    color = TFT_GREEN;
    buttonText = "On";
  } else {
    color = TFT_RED;
    buttonText = "Off";
  }
  curLeft = 50;
  curTop = posTop + 2 * 4 * 7 * textSize + 15 * textSize + 4 * textSize;
  //curTop = posTop + 225;
  curWidth = 4 * 7 * textSize;
  curHeight = 3 * 7 * textSize;
  tft.fillRoundRect(curLeft + 3, curTop + 3, curWidth, curHeight, 3 * textSize, 0x4208);  // Задаваме тъмносиво за сянка на бутона
  tft.fillRoundRect(curLeft, curTop, curWidth, curHeight, 3 * textSize, color);  // Задаваме цвят на бутона
  tft.setTextDatum(MC_DATUM);  // текст в позиция средата/средата спрямо зададените координати
  tft.setTextSize(3);
  tft.setTextColor(0x4208);  // Задаваме тъмносиво за сянка на буквите
  tft.drawString(buttonText, curLeft + curWidth / 2 + 2, curTop + curHeight / 2 + 2);
  tft.setTextColor(TFT_WHITE);  // Задаваме бели букви
  tft.drawString(buttonText, curLeft + curWidth / 2, curTop + curHeight / 2);

  //tft.fillTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_RED);
  //tft.drawTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_BLACK);
}

void displayBugTime(uint8_t bugTime) {
  tft.setCursor(curLeft + curWidth + 20, curTop + 7 * textSize);
  tft.fillRect(curLeft + curWidth + 20, curTop + 7 * textSize, 100, 7 * textSize, TFT_BLUE);  // Изчистване на предишното показание
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW); 
  tft.print(bugTime);
}

#endif  // BUTTON_RECT_H