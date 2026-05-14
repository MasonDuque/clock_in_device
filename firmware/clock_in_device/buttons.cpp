#include "buttons.h"

#include "pins.h"

namespace Buttons {
const int kPins[kCount] = {Pins::kButton1, Pins::kButton2, Pins::kButton3,
                           Pins::kButton4, Pins::kButton5, Pins::kButton6};

void begin() {
  for (int i = 0; i < kCount; ++i) {
    pinMode(kPins[i], INPUT_PULLUP);
  }
}

bool isPressed(int index) {
  if (index < 0 || index >= kCount) return false;
  return digitalRead(kPins[index]) == LOW;
}

int numberForIndex(int index) { return index + 1; }
}  // namespace Buttons
