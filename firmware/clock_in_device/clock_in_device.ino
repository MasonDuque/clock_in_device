#include <Wire.h>

#include "buttons.h"
#include "buzzer.h"
#include "display.h"
#include "leds.h"
#include "pins.h"
#include "sd_logger.h"

void handleButtonPress(int number);

void setup() {
  Serial.begin(115200);

  Wire.begin(Pins::kI2cSda, Pins::kI2cScl);

  initDisplay();
  Buttons::begin();
  LEDs::begin();
  Buzzer::begin();

  showStartup();
  showSdInitializing();

  if (SdLogger::begin()) {
    showSdOk();

    if (SdLogger::writeStartupTest()) {
      showFileWrittenOk();
    } else {
      showFileWriteFailed();
    }
  } else {
    showSdFailed();
  }

  Buzzer::startupChirp();

  delay(2000);

  clearDisplay();
  showPressButtons();
}

void loop() {
  for (int i = 0; i < Buttons::kCount; ++i) {
    if (Buttons::isPressed(i)) {
      handleButtonPress(Buttons::numberForIndex(i));
    }
  }
}

void handleButtonPress(int number) {
  showButtonPressed(number);

  if (number % 2 == 0) {
    LEDs::showGreen();
    showGreenLed();
    Buzzer::playEvenTone();
  } else {
    LEDs::showRed();
    showRedLed();
    Buzzer::playOddTone();
  }

  if (SdLogger::logButtonPress(number)) {
    showLoggedToSd();
  } else {
    showSdWriteError();
  }

  delay(250);
  LEDs::off();
}
