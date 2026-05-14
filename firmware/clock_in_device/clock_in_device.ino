#include <Wire.h>

#include "buttons.h"
#include "buzzer.h"
#include "display.h"
#include "leds.h"
#include "pins.h"
#include "sd_logger.h"
#include "state_machine.h"

void handleButtonPress(int number);

void setup() {
  Serial.begin(115200);

  Wire.begin(Pins::kI2cSda, Pins::kI2cScl);

  initDisplay();
  Buttons::begin();
  LEDs::begin();
  Buzzer::begin();

  initStateMachine();

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
  showCurrentState(getState());
}

void loop() {
  for (int i = 0; i < Buttons::kCount; ++i) {
    if (Buttons::isPressed(i)) {
      handleButtonPress(Buttons::numberForIndex(i));
    }
  }
}

void handleButtonPress(int number) {
  if (number == 1) {
    setState(DeviceState::CLOCKED_IN);
  } else if (number == 2) {
    setState(DeviceState::IDLE);
  } else if (number == 3) {
    setState(DeviceState::MENU);
  } else if (number == 4) {
    setState(DeviceState::IDLE);
  }

  showCurrentState(getState());
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
