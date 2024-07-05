#include <Arduino.h>
#include <TFT_eSPI.h>

// Добавен аналогов пин за измерване на захранващо напрежение. 
// Визуализиране показания на АЦП за захранващо напрежение. 
// Формиране на трети ред на екрана, показващ разликата между двете стойности: 'Δ: <ADC> - <Vcc>'.
// Захранването на сензора и ОУ е пренвърлено на 3.3В.
// Добавяме четвърти ред, показващ изход ОУ2.

#define ANALOG_PIN_OU2 2  // Използваме GPIO2 за аналогов вход (изход ОУ2)
#define ANALOG_PIN_OU1 26 // Използваме GPIO23 за аналогов вход (изход ОУ1)
#define VCC_PIN 35       // Използваме GPIO35 за измерване на Vcc чрез делител на напрежение
#define LED_PIN 25       // Използваме GPIO25 за светодиод

const int numMeasurements = 10;  // Брой измервания за осредняване
const int numAverages = 10;  // Брой усреднени стойности за допълнително осредняване

int adcReadings_OU2[numMeasurements];  // Масив за съхранение на измерванията на ОУ2
int adcReadings_OU1[numMeasurements];  // Масив за съхранение на измерванията на ОУ1
int vccReadings[numMeasurements];      // Масив за съхранение на измерванията на Vcc

int adcAverages_OU2[numAverages];      // Масив за съхранение на усреднените стойности на ОУ2
int adcAverages_OU1[numAverages];      // Масив за съхранение на усреднените стойности на ОУ1
int vccAverages[numAverages];          // Масив за съхранение на усреднените стойности на Vcc

int adcIndex = 0;  // Индекс за текущото измерване
int avgIndex = 0;  // Индекс за текущата усреднена стойност

unsigned long lastMeasurementTime = 0;
unsigned long lastPrintTime = 0;

TFT_eSPI tft = TFT_eSPI();  // Създаване на обект за TFT дисплея

void setup() {
  Serial.begin(9600);  // Започваме сериен монитор на 9600 бауда
  analogSetAttenuation(ADC_11db); // Настройка на затихването за обхват 0-3.3V

  Serial.println("Инициализация...");
  delay(5000);  // Пауза за уравновесяване на нивото на стенда

  // Инициализация на масивите с нули
  for (int i = 0; i < numMeasurements; i++) {
    adcReadings_OU2[i] = 0;
    adcReadings_OU1[i] = 0;
    vccReadings[i] = 0;
  }
  for (int i = 0; i < numAverages; i++) {
    adcAverages_OU2[i] = 0;
    adcAverages_OU1[i] = 0;
    vccAverages[i] = 0;
  }

  Serial.println("Инициализация завършена");

  tft.begin();
  tft.setRotation(3);  // Ротация на екрана в позиция 3
  tft.fillScreen(TFT_BLUE);  // Задаваме син фон
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);  // Задаваме жълти букви на син фон
  tft.setTextSize(4);  // Задаваме размера на текста

  pinMode(LED_PIN, OUTPUT);  // Настройка на пина за светодиода като изход
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

  // Принтиране на осредненото измерване на всеки 300 милисекунди
  if (currentTime - lastPrintTime >= 300) {
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

    // Визуализиране на стойностите на дисплея
    tft.setCursor(60, 30);
    tft.print("Vcc: ");
    tft.fillRect(180, 30, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(180, 30);
    tft.print(finalVccAverage);
    //tft.print(" units");

    tft.setCursor(60, 90);
    tft.print("Vsensor: ");
    tft.fillRect(276, 90, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(276, 90);
    tft.print(finalAdcAverage_OU1);
    //tft.print(" units");

    tft.setCursor(60, 150);
    tft.print("Diff: ");
    tft.fillRect(204, 150, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(204, 150);
    tft.print(delta);
    //tft.print(" units");

    tft.setCursor(60, 210);
    tft.print("ADC: ");
    tft.fillRect(180, 210, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(180, 210);
    tft.print(finalAdcAverage_OU2);
    //tft.print(" units");

    // Превключване на състоянието на светодиода
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
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
