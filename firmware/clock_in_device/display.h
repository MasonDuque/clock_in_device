#ifndef DISPLAY_H
#define DISPLAY_H

#include "state_machine.h"

void initDisplay();
void clearDisplay();
void renderUi();
void showStartupStatus(const String& line1, const String& line2 = "", const String& line3 = "");

#endif
