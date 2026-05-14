#include "leds.h"

#include <Arduino.h>

#include "pins.h"

namespace LEDs {
void begin() {
  pinMode(Pins::kGreenLed, OUTPUT);
  pinMode(Pins::kRedLed, OUTPUT);
  off();
}

void showGreen() {
  digitalWrite(Pins::kGreenLed, HIGH);
  digitalWrite(Pins::kRedLed, LOW);
}

void showRed() {
  digitalWrite(Pins::kRedLed, HIGH);
  digitalWrite(Pins::kGreenLed, LOW);
}

void off() {
  digitalWrite(Pins::kGreenLed, LOW);
  digitalWrite(Pins::kRedLed, LOW);
}
}  // namespace LEDs
