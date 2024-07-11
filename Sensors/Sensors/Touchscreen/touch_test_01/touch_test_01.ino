#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>

#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);
TFT_eSPI tft = TFT_eSPI();

uint16_t posLeft = 20;
uint16_t posTop = 20;
uint8_t textSize = 3;
uint8_t widthTriangle = 4 * 7 * textSize;
uint8_t heightTriangle = 4 * widthTriangle;
uint8_t distTriangle = 2 * textSize * 7;

//tft.fillTriangle(posLEft+widthTriangle/2, posTop, posLeft, posTop+heightTriangel, posLEft+widthTriangle, posTop+heightTriangel, TFT_GREEN);  // Чертае стрелка нагоре
//tft.fillTriangle(posLEft + widthTriangle / 2, posTop+heightTriangel+distTriangle, posLeft, posTop+heightTriangel+distTriangle, posLEft+widthTriangle, posTop+heightTriangel+distTriangle, TFT_RED);  // Чертае стрелка надолу

uint8_t touches;
int myPwm = 0;
int8_t minPwm = 0;
int8_t maxPwm = 8;
String myPwmString = String(myPwm);

void setup() {
  Serial.begin(115200);
  Wire.begin(ALT_SDA, ALT_SCL);

  ts.begin();
  Serial.println("FT6336U инициализиран");

  tft.begin();
  tft.setRotation(3);           // Ротация на екрана в позиция 3
  tft.fillScreen(TFT_BLUE);     // Почистване на екрана, задаване на черен цвят
  tft.setTextColor(TFT_WHITE);  // Цвят на текста
  tft.setTextDatum(MC_DATUM);   // Middle Center Datum - Центрира текста спрямо зададените координати
  tft.setTextSize(3);           // Рармер на шрифта

  tft.fillTriangle(70, 20, 20, 120, 120, 120, TFT_GREEN);  // Чертае стрелка нагоре
  tft.fillTriangle(70, 288, 20, 188, 120, 188, TFT_RED);   // Чертае стрелка надолу

  tft.drawString(myPwmString, 70, 156);  // Визуализиране на текста

  //delay(5000);
}

void loop() {
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

    if (newX > 20 && newX < 120 && newY > 20 && newY < 120) {  // Проверка за увеличаване
      myPwm++;
      myPwm = constrain(myPwm, minPwm, maxPwm);
      myPwmString = String(myPwm);               // Конвертиране на int в string
      tft.fillRect(20, 140, 100, 28, TFT_BLUE);  // Изчистване на зоната под триъгълника
      tft.drawString(myPwmString, 70, 156);      // Визуализиране на текста
    }

    if (newX > 20 && newX < 120 && newY > 188 && newY < 288) {  // Проверка за намаляване
      myPwm--;
      myPwm = constrain(myPwm, minPwm, maxPwm);
      myPwmString = String(myPwm);               // Конвертиране на int в string
      tft.fillRect(20, 140, 100, 28, TFT_BLUE);  // Изчистване на зоната под триъгълника
      tft.drawString(myPwmString, 70, 156);      // Визуализиране на текста
    }

    delay(250);  // Закъснение за предотвратяване на многократно отчитане на едно натискане
  }
  //delay(10);
}
