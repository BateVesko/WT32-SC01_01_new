#ifndef PWM_GENERATOR_H
#define PWM_GENERATOR_H

#include <Arduino.h>
//********************
extern int8_t minPwm;
extern int8_t maxPwm;
//********************
void initializePWM(uint8_t pin) {
    pinMode(pin, OUTPUT);
}

void setPWM(uint8_t pin, float pressureDifference) {
    pressureDifference = constrain(pressureDifference, minPwm, maxPwm);  // Ограничаване на стойността на pressureDifference в интервала [minPwm, maxPwm]
    //pressureDifference = constrain(pressureDifference, 0, 100);  // Ограничаване на стойността на pressureDifference в интервала [0, 100]
    uint8_t dutyCycle = map(pressureDifference, minPwm, maxPwm, 255, 0);  // Мащабиране на стойността към интервала [0, 255]
    //uint8_t dutyCycle = map(pressureDifference, 0, 100, 255, 0);   // Мащабиране на стойността към интервала [0, 255]
    analogWrite(pin, dutyCycle);
}

#endif
