#include "buzzer.h"

#include <Arduino.h>

#include "pins.h"

namespace Buzzer {
void begin() { pinMode(Pins::kBuzzer, OUTPUT); }

void startupChirp() { tone(Pins::kBuzzer, 1200, 100); }

void playClockInTone() { tone(Pins::kBuzzer, 1400, 80); }

void playClockOutTone() { tone(Pins::kBuzzer, 750, 120); }
}  // namespace Buzzer
