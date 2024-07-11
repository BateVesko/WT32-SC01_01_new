#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <driver/ledc.h>  // Включване на заглавния файл за LEDC функции
#include <PWM_generator.h>

#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39
#define LED_PIN 25  // Използваме GPIO25 за светодиод

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);
TFT_eSPI tft = TFT_eSPI();

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
unsigned long lastMeasurementTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(ALT_SDA, ALT_SCL);

  //pinMode(LED_PIN, OUTPUT);  // Настройка на пина за светодиода като изход

  initializePWM(LED_PIN);
  setPWM(LED_PIN, minPwm);

  ts.begin();
  Serial.println("FT6336U инициализиран");

  tft.begin();
  tft.setRotation(3);       // Ротация на екрана в позиция 3
  tft.fillScreen(bgColor);  // Почистване на екрана, задаване на черен цвят
  if (bgColor != TFT_WHITE) {
    txtColor = TFT_YELLOW;
  }
  tft.setTextColor(txtColor);  // Цвят на текста
  tft.setTextDatum(TC_DATUM);  // Middle Center Datum - Центрира текста спрямо зададените координати по хоризонтала
  tft.setTextSize(textSize);   // Рармер на шрифта

  tft.fillTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, TFT_GREEN);  // Чертае стрелка нагоре
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop, posLeft, posTop + heightTriangle, posLeft + widthTriangle, posTop + heightTriangle, TFT_BLACK);
  tft.fillTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_RED);    // Чертае стрелка надолу                                                                 // Чертае стрелка нагоре
  tft.drawTriangle(posLeft + widthTriangle / 2, posTop + 2 * heightTriangle + distTriangle, posLeft, posTop + heightTriangle + distTriangle, posLeft + widthTriangle, posTop + heightTriangle + distTriangle, TFT_BLACK);  // Чертае стрелка надолу

  tft.drawString(myPwmString, posLeft + widthTriangle / 2, posTop + heightTriangle + 4 * textSize);  // Визуализиране на текста

  //delay(5000);
}

void loop() {
  //unsigned long currentTime = millis();

  // if (currentTime - lastMeasurementTime >= 1000) {
  //   lastMeasurementTime = currentTime;
  //   digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  // }


  touches = ts.read_touch_number();
  if (touches > 0) {
    Serial.print("Touches: ");
    Serial.println(touches);
    touches = 0;
    uint16_t x = ts.read_touch1_x();
    uint16_t y = ts.read_touch1_y();

    uint16_t newX = 480 - y;  // Корекция на координатите за ротация 1
    uint16_t newY = x;

    Serial.print("X: ");
    Serial.print(newX);
    Serial.print(" Y: ");
    Serial.println(newY);

    if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop && newY < posTop + heightTriangle) {  // Проверка за увеличаване
      myPwm += 10;
      myPwm = constrain(myPwm, minPwm, maxPwm);
      myPwmString = String(myPwm) + "%";                                                                    // Конвертиране на int в string
      tft.fillRect(posLeft, posTop + heightTriangle + 4 * textSize, widthTriangle, 7 * textSize, bgColor);  // Изчистване на зоната под триъгълника
      tft.drawString(myPwmString, posLeft + widthTriangle / 2, posTop + heightTriangle + 4 * textSize);     // Визуализиране на текста
    }

    if (newX > posLeft && newX < posLeft + widthTriangle && newY > posTop + heightTriangle + distTriangle && newY < posTop + 2 * heightTriangle + distTriangle) {  // Проверка за намаляване
      myPwm -= 10;
      myPwm = constrain(myPwm, minPwm, maxPwm);
      myPwmString = String(myPwm) + "%";                                                                    // Конвертиране на int в string
      tft.fillRect(posLeft, posTop + heightTriangle + 4 * textSize, widthTriangle, 7 * textSize, bgColor);  // Изчистване на зоната под триъгълника
      tft.drawString(myPwmString, posLeft + widthTriangle / 2, posTop + heightTriangle + 4 * textSize);     // Визуализиране на текста
    }

    setPWM(LED_PIN, myPwm);

    delay(250);  // Закъснение за предотвратяване на многократно отчитане на едно натискане
  }
  //delay(10);
}
