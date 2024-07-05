#include <Arduino.h>
#include "driver/ledc.h" // Включване на заглавния файл за LEDC функции

#define LED_PIN 25 // Използваме GPIO25 за светодиод

void setup() {
  Serial.begin(9600);  // Започваме сериен монитор на 9600 бауда

  // Настройка на PWM
  ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_HIGH_SPEED_MODE,
    .duty_resolution  = LEDC_TIMER_8_BIT,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = 5000,  // 5 kHz честота
    .clk_cfg          = LEDC_AUTO_CLK
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {
    .gpio_num       = LED_PIN,
    .speed_mode     = LEDC_HIGH_SPEED_MODE,
    .channel        = LEDC_CHANNEL_0,
    .intr_type      = LEDC_INTR_DISABLE,
    .timer_sel      = LEDC_TIMER_0,
    .duty           = 0, // Начална стойност 0
    .hpoint         = 0
  };
  ledc_channel_config(&ledc_channel);
}

void loop() {
  // Изпращане на PWM сигнал със 50% запълване (128 от 255)
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 128);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  delay(1000); // Изчакай 1 секунда

  // Изпращане на PWM сигнал с 0% запълване (0 от 255)
  ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  delay(1000); // Изчакай 1 секунда
}
