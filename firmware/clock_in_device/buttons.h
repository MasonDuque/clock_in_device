#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

namespace Buttons {
constexpr int kCount = 6;
extern const int kPins[kCount];

void begin();
bool isPressed(int index);
int numberForIndex(int index);
}

#endif
