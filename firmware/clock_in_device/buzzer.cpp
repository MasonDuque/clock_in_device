#include "buzzer.h"

#include <Arduino.h>

#include "pins.h"

namespace Buzzer {
void begin() { pinMode(Pins::kBuzzer, OUTPUT); }

void startupChirp() { tone(Pins::kBuzzer, 1200, 100); }

void playEvenTone() { tone(Pins::kBuzzer, 1200, 80); }

void playOddTone() { tone(Pins::kBuzzer, 800, 80); }
}  // namespace Buzzer
