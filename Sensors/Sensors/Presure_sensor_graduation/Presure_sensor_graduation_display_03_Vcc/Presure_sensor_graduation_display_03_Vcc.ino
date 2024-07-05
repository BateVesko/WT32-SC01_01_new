#include <Arduino.h>
#include <TFT_eSPI.h>

// Добавен аналогов пин за измерване на захранващо напрежение. 
// Визуализиране показания на АЦП за захранващо напрежение. 
// Формиране на трети ред на екрана, показващ разликата между двете стойности: 'Δ: <ADC> - <Vcc>'.
// Захранването на сензора и ОУ е пренвърлено на 3.3В.

#define ANALOG_PIN 2  // Използваме GPIO2 за аналогов вход
#define VCC_PIN 35    // Използваме GPIO35 за измерване на Vcc чрез делител на напрежение

const int numMeasurements = 10;  // Брой измервания за осредняване
const int numAverages = 10;  // Брой усреднени стойности за допълнително осредняване

int adcReadings[numMeasurements];  // Масив за съхранение на измерванията
int adcAverages[numAverages];  // Масив за съхранение на усреднените стойности
int vccReadings[numMeasurements];  // Масив за съхранение на измерванията на Vcc
int vccAverages[numAverages];  // Масив за съхранение на усреднените стойности на Vcc

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
    adcReadings[i] = 0;
    vccReadings[i] = 0;
  }
  for (int i = 0; i < numAverages; i++) {
    adcAverages[i] = 0;
    vccAverages[i] = 0;
  }

  Serial.println("Инициализация завършена");

  tft.begin();
  tft.setRotation(3);  // Ротация на екрана в позиция 3
  tft.fillScreen(TFT_BLUE);  // Задаваме син фон
  tft.setTextColor(TFT_YELLOW, TFT_BLUE);  // Задаваме жълти букви на син фон
  tft.setTextSize(4);  // Задаваме размера на текста
}

void loop() {
  unsigned long currentTime = millis();

  // Осредняване на 10 измервания през 0,1 секунда
  if (currentTime - lastMeasurementTime >= 100) {
    lastMeasurementTime = currentTime;

    adcReadings[adcIndex] = analogRead(ANALOG_PIN);
    vccReadings[adcIndex] = analogRead(VCC_PIN);
    adcIndex = (adcIndex + 1) % numMeasurements;

    if (adcIndex == 0) {
      // Изчисляване на средната стойност на 10 измервания
      int adcTotal = 0;
      int vccTotal = 0;
      for (int i = 0; i < numMeasurements; i++) {
        adcTotal += adcReadings[i];
        vccTotal += vccReadings[i];
      }
      int adcAverage = adcTotal / numMeasurements;
      int vccAverage = vccTotal / numMeasurements;

      // Съхраняване на усреднената стойност в масива с усреднени стойности
      adcAverages[avgIndex] = adcAverage;
      vccAverages[avgIndex] = vccAverage;
      avgIndex = (avgIndex + 1) % numAverages;
    }
  }

  // Принтиране на осредненото измерване на всеки 2 секунди
  if (currentTime - lastPrintTime >= 2000) {
    lastPrintTime = currentTime;

    // Осредняване на последните 10 усреднени стойности
    int adcAvgTotal = 0;
    int vccAvgTotal = 0;
    for (int i = 0; i < numAverages; i++) {
      adcAvgTotal += adcAverages[i];
      vccAvgTotal += vccAverages[i];
    }
    int finalAdcAverage = adcAvgTotal / numAverages;
    int finalVccAverage = vccAvgTotal / numAverages;
    int delta = finalAdcAverage - finalVccAverage;

    // Принтиране на осредненото измерване
    Serial.print("АЦП усреднено = ");
    Serial.println(finalAdcAverage);

    // Принтиране на захранващото напрежение
    Serial.print("Vcc = ");
    Serial.println(finalVccAverage);

    // Принтиране на разликата
    Serial.print("Δ = ");
    Serial.println(delta);

    // Визуализиране на стойностите на дисплея
    tft.setCursor(10, 10);  // Задаваме курсора горе вляво
    tft.print("Vcc: ");
    tft.fillRect(100, 10, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.print(finalVccAverage);
    tft.print(" units");
    tft.println((char)247); // Unicode за градуса символ (°)

    tft.setCursor(10, 50);  // Преместване на курсора за следващото показание
    tft.print("ADC: ");
    tft.fillRect(100, 50, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.print(finalAdcAverage);
    tft.println((char)247); // Unicode за градуса символ (°)

    // Визуализиране на разликата
    tft.setCursor(10, 90);  // Преместване на курсора за показване на разликата
tft.print((char)0xCE);  // Unicode за символа Δ
tft.print((char)0x94);  // Unicode за символа Δ
    tft.print(": ");
    tft.fillRect(100, 90, 120, 28, TFT_BLUE);  // Изчистване на предишното показание
    tft.println(delta);
  }
}

// Пинове за използване:
// J5:
// 02: +5.5V
// 12: IO02 (аналогов вход)
// 22: IO23 
// 32: IO35 (аналогов вход за Vcc)
// J6:
// 04: +3.3V
// 14: IO15 (светодиод)
// 24: IO0
// 34: GND
