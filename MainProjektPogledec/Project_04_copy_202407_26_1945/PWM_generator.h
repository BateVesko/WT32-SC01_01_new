#ifndef PWM_GENERATOR_H
#define PWM_GENERATOR_H

#include <Arduino.h>
//********************
extern int8_t minPwm;
extern int8_t maxPwm;
//********************
void initializePWM(uint8_t pin) {
    pinMode(pin, OUTPUT);  // инициализира пин IO25
}

void setPWM(uint8_t pin, float pressureDifference) {
    pressureDifference = constrain(pressureDifference, minPwm, maxPwm);  // Ограничаване на стойността на pressureDifference в интервала [minPwm, maxPwm]
    uint8_t dutyCycle = map(pressureDifference, minPwm, maxPwm, 255, 0);  // Мащабиране на стойността към интервала [0, 255]
    analogWrite(pin, dutyCycle);  // зареждане PWM на пин IO25
}

#endif
