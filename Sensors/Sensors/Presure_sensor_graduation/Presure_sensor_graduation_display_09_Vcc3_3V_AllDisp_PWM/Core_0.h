#ifndef CORE_0_H
#define CORE_0_H

#include <Arduino.h>

extern float presureReal; // Деклариране на глобалната променлива

void generatePWM(void *parameter) {
  const float pwmMax = 100.0; // Максимална стойност за коефициента на запълване (100%)
  const int period = 1000; // Период на PWM в микросекунди (1 ms)

  for (;;) {
    float pwmValueFloat = (presureReal / 0.35) * pwmMax; // Изчисляване на коефициента на запълване
    pwmValueFloat = constrain(pwmValueFloat, 0.0, pwmMax); // Ограничаване на pwmValue в допустимите граници

    int highTime = (int)((pwmValueFloat * period) / pwmMax); // Време за HIGH състояние в микросекунди
    int lowTime = period - highTime; // Време за LOW състояние в микросекунди

    digitalWrite(LED_PIN, LOW); // Включване на светодиода (LOW)
    delayMicroseconds(highTime); // Забавяне за HIGH състояние

    digitalWrite(LED_PIN, HIGH); // Изключване на светодиода (HIGH)
    delayMicroseconds(lowTime); // Забавяне за LOW състояние

    // Малка пауза за да се предотврати Watchdog рестарт
    vTaskDelay(1); 
  }
}

#endif // CORE_0_H
