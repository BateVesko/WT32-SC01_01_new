#include <Wire.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>

#define ALT_SDA 18
#define ALT_SCL 19
#define TFT_RST -1
#define TFT_INT 39

FT6336U ts(ALT_SDA, ALT_SCL, TFT_RST, TFT_INT);
TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    Wire.begin(ALT_SDA, ALT_SCL);

    ts.begin();
    Serial.println("FT6336U инициализиран");

    tft.begin();
    tft.setRotation(3);  // Ротация на екрана в позиция 1
    tft.fillScreen(TFT_BLACK);  // Почистване на екрана, задаване на черен цвят
    tft.fillRect(0, 0, 10, 10, TFT_RED);
}

void loop() {
    uint8_t touches = ts.read_touch_number();
    if (touches > 0) {
        uint16_t x = ts.read_touch1_x();
        uint16_t y = ts.read_touch1_y();

        // Корекция на координатите за ротация 1
        uint16_t newX = 480 - y;  // Примерно за екран 320x240, коригирайте според вашия екран
        uint16_t newY = x;

        Serial.print("X: ");
        Serial.print(newX);
        Serial.print(" Y: ");
        Serial.println(newY);

        // Рисуване на бяла точка с размер 3x3 пиксела
        tft.fillRect(newX, newY, 10, 10, TFT_BLUE);
    }
    //delay(1);
}
