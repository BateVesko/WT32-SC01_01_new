#ifndef PWM_GENERATOR_H
#define PWM_GENERATOR_H

#include <Arduino.h>

void initializePWM(uint8_t pin) {
    pinMode(pin, OUTPUT);
}

void setPWM(uint8_t pin, float pressureDifference) {
    // Ограничаване на стойността на pressureDifference в интервала [0, 100]
    pressureDifference = constrain(pressureDifference, 0, 100);
    // Мащабиране на стойността към интервала [0, 255]
    uint8_t dutyCycle = map(pressureDifference, 0, 100, 255, 0);
    analogWrite(pin, dutyCycle);
}

#endif
