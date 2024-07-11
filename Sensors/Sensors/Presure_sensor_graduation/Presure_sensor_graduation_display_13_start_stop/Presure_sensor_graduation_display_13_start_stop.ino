#include <Arduino.h>
//#include <TFT_eSPI.h>
#include <driver/ledc.h>  // Включване на заглавния файл за LEDC функции
#include "data_visualize.h"
#include "PWM_generator.h"
#include <variable_set_triangles.h>
#include <button_rect.h>

#define ANALOG_PIN_OU2 2   // Използваме GPIO2 за аналогов вход (изход ОУ2)
#define ANALOG_PIN_OU1 26  // Използваме GPIO26 за аналогов вход (изход ОУ1)
#define VCC_PIN 35         // Използваме GPIO35 за измерване на Vcc чрез делител на напрежение
#define LED_PIN 25         // Използваме GPIO25 за светодиод

//#include "Core_0.h"

// Глобална променлива
float presureReal = 0.0;

const float D = 0.110;
const float d = 0.070;
const int numMeasurements = 5;  // Брой измервания за осредняване
const int numAverages = 5;      // Брой усреднени стойности за допълнително осредняване

int adcReadings_OU2[numMeasurements];  // Масив за съхранение на измерванията на ОУ2
int adcReadings_OU1[numMeasurements];  // Масив за съхранение на измерванията на ОУ1
int vccReadings[numMeasurements];      // Масив за съхранение на измерванията на Vcc

int adcAverages_OU2[numAverages];  // Масив за съхранение на усреднените стойности на ОУ2
int adcAverages_OU1[numAverages];  // Масив за съхранение на усреднените стойности на ОУ1
int vccAverages[numAverages];      // Масив за съхранение на усреднените стойности на Vcc

int adcIndex = 0;  // Индекс за текущото измерване
int avgIndex = 0;  // Индекс за текущата усреднена стойност

unsigned long lastMeasurementTime = 0;
unsigned long lastPrintTime = 0;

int ADCoffset = 0;  // Първоначална стойност на налягането

float AirSpeed(float pressureReal, float D, float d) {
  const float g = 9.81;  // Ускорение на свободното падане в м/с²
  // Изчисляване на скоростта в по-малката тръба
  if (pressureReal < 0) return 0;  // Защита от отрицателни налягания
  float velocity = sqrt((2 * g * pressureReal) / (1 - pow(d, 4) / pow(D, 4)));
  return velocity;
}

void setup() {
  Serial.begin(9600);              // Започваме сериен монитор на 9600 бауда
  analogSetAttenuation(ADC_11db);  // Настройка на затихването за обхват 0-3.3V

  delay(5000);  // Пауза за уравновесяване на нивото на стенда

  initializeDisplay();  // Инициализация на дисплея
  //initializeScreen();
  drawTriangles();
  drawButton_01(true);
  updatePwmDisplay();
  initializePWM(LED_PIN);  // *****************
  Wire.begin(ALT_SDA, ALT_SCL);

  pinMode(LED_PIN, OUTPUT);  // Настройка на пина за светодиода като изход

  // Забавяне за стабилизиране на показанията
  delay(1000);

  // Първоначално измерване за ADCoffset
  int numInitialMeasurements = 20;  // Увеличаване на броя първоначални измервания
  int adcTotal_OU2 = 0;
  for (int i = 0; i < numInitialMeasurements; i++) {
    adcTotal_OU2 += analogRead(ANALOG_PIN_OU2);
    delay(10);  // Малко забавяне между измерванията за стабилизиране
  }
  ADCoffset = adcTotal_OU2 / numInitialMeasurements;

  Serial.println("PWM инициализация завършена");

  setPWM(LED_PIN, 0);  // Настройка на PWM на 0
}

void loop() {
  unsigned long currentTime = millis();

  // Осредняване на 10 измервания през 30 милисекунди
  if (currentTime - lastMeasurementTime >= 30) {
    lastMeasurementTime = currentTime;

    adcReadings_OU2[adcIndex] = analogRead(ANALOG_PIN_OU2);
    adcReadings_OU1[adcIndex] = analogRead(ANALOG_PIN_OU1);
    vccReadings[adcIndex] = analogRead(VCC_PIN);
    adcIndex = (adcIndex + 1) % numMeasurements;

    if (adcIndex == 0) {
      // Изчисляване на средната стойност на 10 измервания
      int adcTotal_OU2 = 0;
      int adcTotal_OU1 = 0;
      int vccTotal = 0;
      for (int i = 0; i < numMeasurements; i++) {
        adcTotal_OU2 += adcReadings_OU2[i];
        adcTotal_OU1 += adcReadings_OU1[i];
        vccTotal += vccReadings[i];
      }
      int adcAverage_OU2 = adcTotal_OU2 / numMeasurements;
      int adcAverage_OU1 = adcTotal_OU1 / numMeasurements;
      int vccAverage = vccTotal / numMeasurements;

      // Съхраняване на усреднената стойност в масива с усреднени стойности
      adcAverages_OU2[avgIndex] = adcAverage_OU2;
      adcAverages_OU1[avgIndex] = adcAverage_OU1;
      vccAverages[avgIndex] = vccAverage;
      avgIndex = (avgIndex + 1) % numAverages;
    }
  }

  // Принтиране на осредненото измерване на всеки 1000 милисекунди
  if (currentTime - lastPrintTime >= 1000) {
    lastPrintTime = currentTime;

    // Осредняване на последните 10 усреднени стойности
    int adcAvgTotal_OU2 = 0;
    int adcAvgTotal_OU1 = 0;
    int vccAvgTotal = 0;
    for (int i = 0; i < numAverages; i++) {
      adcAvgTotal_OU2 += adcAverages_OU2[i];
      adcAvgTotal_OU1 += adcAverages_OU1[i];
      vccAvgTotal += vccAverages[i];
    }
    int finalAdcAverage_OU2 = adcAvgTotal_OU2 / numAverages;
    int finalAdcAverage_OU1 = adcAvgTotal_OU1 / numAverages;
    int finalVccAverage = vccAvgTotal / numAverages;
    int delta = finalVccAverage - finalAdcAverage_OU1;

    // Изчисляване на скоростта на въздушния поток
    int pressureDifference = -finalAdcAverage_OU2 + ADCoffset;
    presureReal = (3.0 / 873) * pressureDifference;  // Линейна зависимост
    float airSpeed = AirSpeed(presureReal, D, d);    // Примерни стойности за D и d
    float debit = 3.14159 * pow((D / 2), 2) * airSpeed * 3600;

    display_on(finalVccAverage, finalAdcAverage_OU1, delta, ADCoffset, pressureDifference, presureReal, airSpeed, debit);  // -> 

    // Превключване на състоянието на светодиода
    // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  int touches = ts.read_touch_number();  // проверка за докосване на тъча
  if (touches > 0) {
    touches = 0;
    uint16_t x = ts.read_touch1_x();
    uint16_t y = ts.read_touch1_y();
    uint16_t newX = 480 - y;
    uint16_t newY = x;
    Serial.print("X: ");
    Serial.print(newX);
    Serial.print(" Y: ");
    Serial.println(newY);
    handleTouch(newX, newY);  // -> variable_set_triangles.h
    x = 0;
    y = 0;

    delay(250);
  }
}

// Пинове за използване:
// J5:
// 02: +5.5V
// 12: IO02 (аналогов вход - от изход ОУ2)
// 22: IO23 (опитахме, но не се получава и отива на IO26)
// 30: IO26 (аналогов вход - от изход ОУ1)
// 32: IO35 (аналогов вход за Vcc)
// J6:
// 08: GND
// 18: ESP32 EN
// 28: IO25 (светодиод)
// 38: +3.3V
