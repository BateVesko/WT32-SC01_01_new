#ifndef DATA_VISUALIZE_H
#define DATA_VISUALIZE_H

#include <TFT_eSPI.h>  // Assuming you use this library for visualization

TFT_eSPI tft = TFT_eSPI();  // Създаване на обект за TFT дисплея
extern uint8_t textSize;
//uint8_t sizeText = textSize;
uint8_t sizeText = 2;

uint8_t indentText = 10 * sizeText;
uint8_t lineSpacing = 7 * sizeText + 6 * sizeText;
uint8_t heightText = 7 * sizeText;
uint8_t lengthText = 6 * sizeText;

void initializeDisplay() {

  tft.begin();
  tft.setRotation(3);  // Ротация на екрана в позиция 3
  tft.fillScreen(TFT_BLUE);  // Задаваме син фон
  //tft.setTextColor(TFT_YELLOW, TFT_BLUE);  // Задаваме жълти букви на син фон
  tft.setTextSize(sizeText);  // Задаваме размера на текста
}

void display_on(int finalVccAverage, int finalAdcAverage_OU1, int delta, int ADCoffset, int pressureDifference, float presureReal, float airSpeed, float debit) {
    int numberSize = 7;
    char buffer[numberSize];
    tft.setTextColor(TFT_YELLOW, TFT_BLUE);  // Задаваме жълти букви на син фон
    tft.setTextSize(sizeText);  // Задаваме размера на текста

    // Визуализиране на стойностите на дисплея
    tft.setCursor(indentText, 10); // Ред 1
    tft.print("Vcc: ");
    tft.fillRect(indentText + 5 * 6 * sizeText, 10, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 5 * 6 * sizeText, 10);
    tft.print(finalVccAverage);

    tft.setCursor(indentText, 10 + 1 * lineSpacing); // Ред 2
    tft.print("Vsensor: ");
    tft.fillRect(indentText + 9 * 6 * sizeText, 10 + 1 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 9 * 6 * sizeText, 10 + 1 * lineSpacing);
    tft.print(finalAdcAverage_OU1);

    tft.setCursor(indentText, 10 + 2 * lineSpacing); // Ред 3
    tft.print("Diff: ");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 2 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 2 * lineSpacing);
    tft.print(delta);

    tft.setCursor(indentText, 10 + 3 * lineSpacing); // Ред 4
    tft.print("ADCoffset: ");
    tft.fillRect(indentText + 11 * 6 * sizeText, 10 + 3 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 11 * 6 * sizeText, 10 + 3 * lineSpacing);
    tft.print(ADCoffset);

    tft.setCursor(indentText, 10 + 4 * lineSpacing); // Ред 5
    tft.print("ADC: ");
    tft.fillRect(indentText + 5 * 6 * sizeText, 10 + 4 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 5 * 6 * sizeText, 10 + 4 * lineSpacing);
    tft.print(pressureDifference);

    tft.setCursor(indentText, 10 + 5 * lineSpacing); // Ред 6
    tft.print("Presure =");
    tft.fillRect(indentText + 9 * 6 * sizeText, 10 + 5 * lineSpacing, numberSize * lengthText, heightText, TFT_RED);  // Изчистване на предишното показание
    tft.setCursor(indentText + 9 * 6 * sizeText, 10 + 5 * lineSpacing);
    sprintf(buffer, "%6.2f", presureReal); // Форматиране на числото
    tft.print(buffer);
    tft.print(" mmH2O");

    tft.setCursor(indentText, 10 + 6 * lineSpacing); // Ред 7
    tft.print("Vair =");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 6 * lineSpacing, 6 * lengthText, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 6 * lineSpacing);
    sprintf(buffer, "%6.1f", airSpeed); // Форматиране на числото с точност до един знак след десетичната точка
    tft.print(buffer);
    tft.print(" m/s");

    tft.setCursor(indentText, 10 + 7 * lineSpacing); // Ред 8
    tft.print("Wair =");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 7 * lineSpacing, 6 * lengthText, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 7 * lineSpacing);
    sprintf(buffer, "%6d", (int)debit); // Форматиране на числото като цяло число
    tft.print(buffer);
    tft.print(" m3/h");
}

// void displayTemperature(float temp) {
//     tft.setCursor(0, 0);
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(2);
//     tft.printf("Temperature: %.2f C", temp);
// }

// void displayPressure(float pressure) {
//     tft.setCursor(0, 30);
//     tft.setTextColor(TFT_WHITE, TFT_BLACK);
//     tft.setTextSize(2);
//     tft.printf("Pressure: %.2f Pa", pressure);
// }

#endif
