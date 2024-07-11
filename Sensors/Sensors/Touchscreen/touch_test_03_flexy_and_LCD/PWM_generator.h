#ifndef PWM_GENERATOR_H
#define PWM_GENERATOR_H

#include <Arduino.h>

extern int8_t minPwm;
extern int8_t maxPwm;

void initializePWM(uint8_t pin) {
    pinMode(pin, OUTPUT);
}

void setPWM(uint8_t pin, float pressureDifference) {
    // Ограничаване на стойността на pressureDifference в интервала [0, 100]
    pressureDifference = constrain(pressureDifference, minPwm, maxPwm);
    // Мащабиране на стойността към интервала [0, 255]
    uint8_t dutyCycle = map(pressureDifference, minPwm, maxPwm, 255, 0);
    analogWrite(pin, dutyCycle);
}

#endif
