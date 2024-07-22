#ifndef SOFT_BACKLIGHT_H
#define SOFT_BACKLIGHT_H

bool backlightState = false;

void initBacklight() {
  pinMode(TFT_BL, OUTPUT);  // Предполага се, че подсветката е свързана към пин TFT_BL
  analogWrite(TFT_BL, 0);  // Изключване на подсветката в началото
}

// void toggleBacklight() {
//   backlightState = !backlightState;
//   if (backlightState) {
//     digitalWrite(TFT_BL, HIGH);  // Включване на подсветката
//   } else {
//     digitalWrite(TFT_BL, LOW);  // Изключване на подсветката
//   }
// }

#endif
