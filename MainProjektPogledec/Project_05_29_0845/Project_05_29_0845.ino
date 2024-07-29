// A1_6531 -> 48575443BD107EA3
// TP-Link_1AEA -> 83590566

#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <PNGdec.h>
#include "110_images.h"
#include "020_display_utils.h"
#include "040_rotate_icon.h"
#include "070_HTTPServer.h"
#include "030_touch_utils.h"
#include "080_PWM_generator.h"
#include "090_ColorUtils.h"

// Създаване на инстанция на библиотеката TFT_eSPI
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// Създаване на инстанция на PNG декодера
PNG png;  // PNG decoder instance

#define MAX_IMAGE_WIDTH 320  // Adjust for your images

// Структура за съхранение на информация за иконките
struct Icon {
  const uint8_t* data;
  uint32_t size;
  int16_t width;
  int16_t height;
  int16_t xpos;
  int16_t ypos;
};

// Масив от иконки
Icon icons[] = {
  { earth, sizeof(earth), 80, 80, 0, 0 },                          // 0
  { fan_0_new, sizeof(fan_0_new), 80, 80, 0, 81 },                 // 1
  { heater, sizeof(heater), 80, 80, 0, 162 },                      // 2
  { glamp, sizeof(glamp), 80, 72, 0, 243 },                        // 3
  { http, sizeof(http), 80, 80, 400, 0 },                          // 4
  { dust, sizeof(dust), 80, 80, 400, 81 },                         // 5
  { bell, sizeof(bell), 80, 74, 400, 162 },                        // 6
  { lock, sizeof(lock), 80, 78, 400, 237 },                        // 7
  { redutaSmall101, sizeof(redutaSmall101), 320, 270, 81, 33 },      // 8
  { color_palette80, sizeof(color_palette80), 5, 100, 400, 315 },  // 9
  { color_palette80, sizeof(color_palette80), 5, 100, 0, 315 },    // 10
  { lamp, sizeof(lamp), 80, 72, 0, 243 },                          // 11
  // Добавете още иконки тук
};

const int holdDownLevel = 40;                     // Ниво за задържане яркостта при LightDown(), 0-250
const unsigned long intervalMillisLightDown = 1;  // Интервал за стъпкова промяна на яркостта на екрана, mcs
const int16_t holdDownTime = 400;                 // Брой стъпки за задържане яркостта на екрана при LightDown().

extern bool backlightOn;
extern bool isTouched;
extern bool lampIsOn;

int16_t currentIconXpos;
int16_t currentIconYpos;

unsigned long lastUpdateTime_Count = 0;
unsigned long lastUpdateTime_RotIcon_fan = 0;
unsigned long lastUpdateTime_Backlight;
uint16_t backlightTime = 10000;
int currentIconIndex_fan = 0;
int Ix_fan = 0;
int Iy_fan = 81;
char direction_fan = 'r';
bool rotateGo = false;
uint16_t pwmPeriod = 0;
int count_time = 2000;

int counter = 0;  // Брояч

//****************************
// Променливи за LightUp метод
unsigned long previousMillisLightUp = 0;
const unsigned long intervalMillisLightUp = 10;
int pwmLightUp = 0;
bool dimmingLightUp = false;

// Променливи за LightDown метод
unsigned long previousMillisLightDown = 0;

int16_t holdTime = holdDownTime;  // Брой цикли на intervalMillisLightDown за задържана на изгасването.
int pwmLightDown = 255;
bool dimmingLightDown = false;
//***************************


String ssidList;
WebServer httpServer(80);

String ssid = "";
String password = "";
String knownNetwork = "TP-Link_1AEA";  // име на "позната" мрежа



void LightUp();
void LightDown();

void setup() {
  analogWrite(TFT_BL, 0);  // Изключване на подсветката в началото
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);  // 0xf7be - онова мръсно бяло

  initIcons();              //  -> Below
  pinMode(TFT_BL, OUTPUT);  // Предполага се, че подсветката е свързана към пин TFT_BL

  lampIsOn = false;
  backlightOn = true;
  isTouched = false;

  initWiFiAndServer();     //  -> Below
  initializePWM(LED_PIN);  // -> PWM_generator
  //initBacklight();  // Инициализация на подсветката
  Wire.begin(ALT_SDA, ALT_SCL);
  rotate_icon_new(tft, png, Ix_fan, Iy_fan, currentIconIndex_fan, direction_fan);
  setPWM(LED_PIN, 0);
  displayTextInfo();     // -> display_utils.h
  displayIPAddress();    // -> display_utils.h
  visualizeMainLines();  // -> ColorUtils
  // tft.drawLine(80, 0, 80, 320, mainBorderColor);
  // tft.drawLine(400, 0, 400, 320, mainBorderColor);


  //LightUp();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime_Count >= count_time && !settingMode && !wiFiMode) {
    lastUpdateTime_Count = currentTime;
    displayMemoryInfo(counter);     // Показва брояча
    counter = (counter + 1) % 999;  // Увеличаване на брояча ************
  }

  if (currentTime - lastUpdateTime_RotIcon_fan >= pwmPeriod && rotateGo && !settingMode) {  //
    lastUpdateTime_RotIcon_fan = currentTime;
    rotate_icon_new(tft, png, Ix_fan, Iy_fan, currentIconIndex_fan, direction_fan);
    currentIconIndex_fan = (currentIconIndex_fan + 1) % 4;
  }

  LightUp();
  LightDown();
  if (backlightOn) {
    if (currentTime - lastUpdateTime_Backlight >= backlightTime && !isTouched && !lampIsOn) {
      pwmLightDown = 255;
      holdTime = holdDownTime;
      dimmingLightDown = true;
      backlightOn = false;
    }
    if (isTouched) {
      lastUpdateTime_Backlight = currentTime;
      isTouched = false;
    }
  }

  httpServer.handleClient();

  handleTouch();
  //delay(10);  // малко закъснение за стабилизация. Работи неизвестно как, но ако го няма се появява
}

void displayIcon(const Icon& icon) {
  int16_t rc = png.openFLASH((uint8_t*)icon.data, icon.size, pngDraw);
  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
}

void pngDraw(PNGDRAW* pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(currentIconXpos, currentIconYpos + pDraw->y, pDraw->iWidth, 1, lineBuffer);  // Използвайте текущите позиции и pDraw->y
}

void initIcons() {
  for (int i = 0; i < sizeof(icons) / sizeof(icons[0]) - 1; i++) {
    currentIconXpos = icons[i].xpos;
    currentIconYpos = icons[i].ypos;
    displayIcon(icons[i]);
  }
}

void reloadIcon(int numIcon) {  // изобразява отново икона по номера в Icon icons[]
  currentIconXpos = icons[numIcon].xpos;
  currentIconYpos = icons[numIcon].ypos;
  displayIcon(icons[numIcon]);
}

void initWiFiAndServer() {
  WiFi.disconnect(true);  // Принудително изключване на автоматичното свързване

  // Инициализация на Access Point
  WiFi.mode(WIFI_AP_STA);                                 // Уверете се, че платката е в режим Access Point и станция
  WiFi.softAP("BatevotoVeskovo_ESP32-AP_1", "12345678");  // Така ще се вижда точката за достъп към платката в режим AP

  // Сканиране на наличните WiFi мрежи
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String currentSSID = WiFi.SSID(i);

    // Добавяне на SSID към списъка с налични мрежи
    ssidList += "<option value=\"" + currentSSID + "\"";
    if (currentSSID == knownNetwork) {
      ssidList += " selected";  // Маркиране на позната мрежа като избрана
    }
    ssidList += ">" + currentSSID + "</option>";
  }

  // Инициализиране на HTTP сървъра
  httpServer.on("/", handleRoot);
  httpServer.on("/connect", HTTP_POST, handleConnect);
  httpServer.on("/welcome", handleWelcome);  // Добавяне на маршрута за /welcome
  httpServer.begin();
  Serial.println("HTTP server started");

  // Принтиране на IP адреса на AP
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void LightUp() {
  if (dimmingLightUp) {
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillisLightUp) >= intervalMillisLightUp) {
      previousMillisLightUp = currentMillis;
      uint8_t step = 1;
      analogWrite(TFT_BL, pwmLightUp);
      Serial.print("PwmLightUp: ");
      Serial.println(pwmLightUp);
      if (pwmLightUp > 30) {
        step = 2;
      }
      if (pwmLightUp > 80) {
        step = 5;
      }
      pwmLightUp += step;
      if (pwmLightUp >= 255) {
        analogWrite(TFT_BL, 255);
        dimmingLightUp = false;
      }
    }
  }
}

void LightDown() {
  if (dimmingLightDown) {
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillisLightDown) >= intervalMillisLightDown) {
      previousMillisLightDown = currentMillis;
      uint8_t step = 5;
      analogWrite(TFT_BL, pwmLightDown);
      Serial.print("PwmLightDown: ");
      Serial.println(pwmLightDown);
      if (pwmLightDown == holdDownLevel) {
        holdTime--;
        Serial.print("HoldTime: ");
        Serial.println(holdTime);
        if (holdTime <= 0) {
          pwmLightDown = holdDownLevel - 1;
          return;
        }
        return;
      }
      if (pwmLightDown <= holdDownLevel - 1) {
        step = 1;
      }
      pwmLightDown -= step;
      if (pwmLightDown <= 0) {
        analogWrite(TFT_BL, pwmLightDown);
        delay(10);  // Малко закъснение за да се уверим, че командата се изпълнява
        Serial.print("LastPwmLightDown: ");
        Serial.println(pwmLightDown);
        dimmingLightDown = false;
        analogWrite(TFT_BL, 0);
      }
    }
  }
}