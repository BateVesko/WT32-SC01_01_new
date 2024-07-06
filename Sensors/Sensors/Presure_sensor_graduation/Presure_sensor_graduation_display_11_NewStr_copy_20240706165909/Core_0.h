#ifndef CORE_0_H
#define CORE_0_H

#include <Arduino.h>
#include <driver/ledc.h>  // Включване на заглавния файл за LEDC функции

extern float presureReal; // Деклариране на глобалната променлива

void setupPWM() {
  const ledc_timer_bit_t pwmResolution = LEDC_TIMER_8_BIT; // 8-битова резолюция
  const ledc_channel_t pwmChannel = LEDC_CHANNEL_0;
  const int pwmFrequency = 5000; // 5 kHz честота

  ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_HIGH_SPEED_MODE,
    .duty_resolution  = pwmResolution,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = pwmFrequency,
    .clk_cfg          = LEDC_AUTO_CLK
  };
  if (ledc_timer_config(&ledc_timer) != ESP_OK) {
    Serial.println("LED Timer configuration failed!");
  }

  ledc_channel_config_t ledc_channel = {
    .gpio_num       = LED_PIN,
    .speed_mode     = LEDC_HIGH_SPEED_MODE,
    .channel        = pwmChannel,
    .intr_type      = LEDC_INTR_DISABLE,
    .timer_sel      = LEDC_TIMER_0,
    .duty           = 0, // Начална стойност 0
    .hpoint         = 0
  };
  if (ledc_channel_config(&ledc_channel) != ESP_OK) {
    Serial.println("LED Channel configuration failed!");
  }

  Serial.println("PWM setup completed!");
}

void generatePWM(void *parameter) {
  const int pwmMax = 255; // Максимална стойност за 8-битова резолюция

  for (;;) {
    float pwmValueFloat = (presureReal / 0.35) * pwmMax; // Изчисляване на коефициента на запълване
    int pwmValue = constrain((int)pwmValueFloat, 0, pwmMax); // Ограничаване на pwmValue в допустимите граници

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pwmValue);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    Serial.print("PWM Value: ");
    Serial.println(pwmValue);

    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1-секундна пауза за да се избегне Watchdog рестарт
  }
}

#endif // CORE_0_H
