#ifndef BUTTON_RECT_H
#define BUTTON_RECT_H

extern uint16_t posLeft;
extern uint16_t posTop;
extern uint8_t textSize;

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
  uint16_t curLeft = 50;
  uint16_t curTop = posTop + 2 * 4 * 7 * textSize + 15 * textSize + 4 * textSize;
  uint16_t curWidth = 4 * 7 * textSize;
  uint16_t curHeight = 3 * 7 * textSize;
  tft.fillRoundRect(curLeft + 3, curTop + 3, curWidth, curHeight, 3 * textSize, 0x4208);
  tft.fillRoundRect(curLeft, curTop, curWidth, curHeight, 3 * textSize, color);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(0x4208);  // Задаваме тъмносиво за сянка на буквите
  tft.drawString(buttonText, curLeft + curWidth / 2 + 2, curTop + curHeight / 2 + 2);
  tft.setTextColor(TFT_WHITE);  // Задаваме бели букви
  tft.drawString(buttonText, curLeft + curWidth / 2, curTop + curHeight / 2);
  
  //tft.fillTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_RED);
  //tft.drawTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_BLACK);
}



#endif  // BUTTON_RECT_H