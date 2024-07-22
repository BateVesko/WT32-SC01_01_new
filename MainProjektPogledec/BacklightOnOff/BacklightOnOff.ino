#include <TFT_eSPI.h>
#include <Wire.h>
#include "soft_backlight.h"
#include "touch_utils.h"

TFT_eSPI tft = TFT_eSPI();  // Инициализация на дисплея

extern bool backlightOn;
extern bool isTouched;

uint16_t backlightTime = 10000;
unsigned long lastUpdateTime_Backlight;
unsigned long lastUpdateTime_ToOff;
uint8_t countToOff = 0;

void setup() {
  Serial.begin(115200);
  tft.begin();
  //tft.init();
  tft.fillScreen(TFT_BLUE);      // Запълване на екрана със цвят.
  tft.setTextColor(TFT_YELLOW);  // Жълти букви.
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Touch to toggle backlight");

  Wire.begin(ALT_SDA, ALT_SCL);
  //initTouch();  // Инициализация на тъчскрийна
  initBacklight();  // Инициализация на подсветката
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime_ToOff >= 1000) {
    lastUpdateTime_ToOff = currentTime;
    tft.setCursor(320, 10);
    tft.fillRect(320, 10, 36, 17, TFT_BLUE);
    tft.println(countToOff--);
  }

  if (backlightOn) {
    if (currentTime - lastUpdateTime_Backlight >= backlightTime && !isTouched) {
      lastUpdateTime_Backlight = currentTime;
      digitalWrite(TFT_BL, LOW);  // Дисплея изгасва.
      backlightOn = false;
    }
    if (isTouched) {
      lastUpdateTime_Backlight = currentTime;
      lastUpdateTime_ToOff = currentTime;
      countToOff = backlightTime / 1000;
      tft.setCursor(320, 10);
      tft.fillRect(320, 10, 36, 17, TFT_BLUE);
      tft.println(countToOff--);
      isTouched = false;
    }
    //delay(50);  // Малка пауза за избягване на многократни регистрации на докосване
  }



  handleTouch();
  delay(10);  // малко закъснение за стабилизация. Работи неизвестно как, но ако го няма се появява
}
