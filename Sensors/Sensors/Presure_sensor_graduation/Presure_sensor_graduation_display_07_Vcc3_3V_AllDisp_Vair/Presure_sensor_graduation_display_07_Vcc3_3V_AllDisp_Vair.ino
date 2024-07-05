#include <Arduino.h>
#include <TFT_eSPI.h>

// Добавен аналогов пин за измерване на захранващо напрежение. 
// Визуализиране показания на АЦП за захранващо напрежение. 
// Формиране на трети ред на екрана, показващ разликата между двете стойности: 'Δ: <ADC> - <Vcc>'.
// Захранването на сензора и ОУ е пренвърлено на 3.3В.
// Добавяме четвърти ред, показващ изход ОУ2.

#define ANALOG_PIN_OU2 2  // Използваме GPIO2 за аналогов вход (изход ОУ2)
#define ANALOG_PIN_OU1 26 // Използваме GPIO26 за аналогов вход (изход ОУ1)
#define VCC_PIN 35       // Използваме GPIO35 за измерване на Vcc чрез делител на напрежение
#define LED_PIN 25       // Използваме GPIO25 за светодиод

const float D = 0.110;
const float d = 0.070;
const int numMeasurements = 10;  // Брой измервания за осредняване
const int numAverages = 10;  // Брой усреднени стойности за допълнително осредняване
const int sizeText = 2;
int indentText = 10 * sizeText;
int lineSpacing = 7 * sizeText + 6 * sizeText;
int heightText = 7 * sizeText;
int lengthText = 6 * sizeText;

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

int ADCoffset = 0; // Първоначална стойност на налягането

float AirSpeed(float pressureReal, float D, float d) {
    const float g = 9.81; // Ускорение на свободното падане в м/с²

    // Изчисляване на скоростта в по-малката тръба
    float velocity = sqrt((2 * g * pressureReal) / (1 - pow(d, 4) / pow(D, 4)));
    return velocity;
}

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
  tft.setTextSize(sizeText);  // Задаваме размера на текста

  pinMode(LED_PIN, OUTPUT);  // Настройка на пина за светодиода като изход

  // Първоначално измерване за ADCoffset
  for (int i = 0; i < numMeasurements; i++) {
    adcReadings_OU2[i] = analogRead(ANALOG_PIN_OU2);
    adcReadings_OU1[i] = analogRead(ANALOG_PIN_OU1);
    vccReadings[i] = analogRead(VCC_PIN);
  }

  int adcTotal_OU2 = 0;
  for (int i = 0; i < numMeasurements; i++) {
    adcTotal_OU2 += adcReadings_OU2[i];
  }
  ADCoffset = adcTotal_OU2 / numMeasurements;
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

    // Изчисляване на скоростта на въздушния поток
    int pressureDifference = finalAdcAverage_OU2 - ADCoffset;
    float presureReal = (10.0 / 864) * pressureDifference;  // Линейна зависимост
    float airSpeed = AirSpeed(presureReal, D, d);  // Примерни стойности за D и d
    float debit = 3.14159 * pow((D / 2), 2) * airSpeed * 3600;

    int numberSize = 6;
    char buffer[numberSize];

    // Визуализиране на стойностите на дисплея
    tft.setCursor(indentText, 10); // Ред 1
    tft.print("Vcc: ");
    tft.fillRect(indentText + 5 * 6 * sizeText, 10, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 5 * 6 * sizeText, 10);
    tft.print(finalVccAverage);
    //tft.print(" units");

    tft.setCursor(indentText, 10 + 1 * lineSpacing); // Ред 2
    tft.print("Vsensor: ");
    tft.fillRect(indentText + 9 * 6 * sizeText, 10 + 1 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 9 * 6 * sizeText, 10 + 1 * lineSpacing);
    tft.print(finalAdcAverage_OU1);
    //tft.print(" units");

    tft.setCursor(indentText, 10 + 2 * lineSpacing); // Ред 3
    tft.print("Diff: ");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 2 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 2 * lineSpacing);
    tft.print(delta);
    //tft.print(" units");

    tft.setCursor(indentText, 10 + 3 * lineSpacing); // Ред 4
    tft.print("ADCoffset: ");
    tft.fillRect(indentText + 11 * 6 * sizeText, 10 + 3 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 11 * 6 * sizeText, 10 + 3 * lineSpacing);
    tft.print(ADCoffset);

    tft.setCursor(indentText, 10 + 4 * lineSpacing); // Ред 5
    tft.print("ADC: ");
    tft.fillRect(indentText + 5 * 6 * sizeText, 10 + 4 * lineSpacing, 120, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 5 * 6 * sizeText, 10 + 4 * lineSpacing);
    //tft.print(ADCoffset - finalAdcAverage_OU2);
    tft.print(pressureDifference);

    tft.setCursor(indentText, 10 + 5 * lineSpacing); // Ред 6
    tft.print("Presure =");
    tft.fillRect(indentText + 9 * 6 * sizeText, 10 + 5 * lineSpacing, numberSize * lengthText, heightText, TFT_RED);  // Изчистване на предишното показание
    tft.setCursor(indentText + 9 * 6 * sizeText, 10 + 5 * lineSpacing);
    //tft.print(presureReal, 2);
    sprintf(buffer, "%6.2f", presureReal); // Форматиране на числото
    tft.print(buffer);
    tft.print(" mmH2O");

    tft.setCursor(indentText, 10 + 6 * lineSpacing); // Ред 7
    tft.print("Vair =");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 6 * lineSpacing, 6 * lengthText, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 6 * lineSpacing);
    //tft.print(airSpeed, 2);
    sprintf(buffer, "%6.1f", airSpeed); // Форматиране на числото
    tft.print(buffer);
    tft.print(" m/s");

    tft.setCursor(indentText, 10 + 7 * lineSpacing); // Ред 8
    tft.print("Wair =");
    tft.fillRect(indentText + 6 * 6 * sizeText, 10 + 7 * lineSpacing, 6 * lengthText, heightText, TFT_BLUE);  // Изчистване на предишното показание
    tft.setCursor(indentText + 6 * 6 * sizeText, 10 + 7 * lineSpacing);
    //tft.print(debit, 2);
    sprintf(buffer, "%6d", debit); // Форматиране на числото
    tft.print(buffer);
    tft.print(" m3/h");

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
