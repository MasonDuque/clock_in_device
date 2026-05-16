#include <Wire.h>

#include "buttons.h"
#include "buzzer.h"
#include "display.h"
#include "leds.h"
#include "pins.h"
#include "sd_logger.h"
#include "state_machine.h"

namespace {
uint32_t gLastRenderMs = 0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(Pins::kI2cSda, Pins::kI2cScl);

  initDisplay();
  Buttons::begin();
  LEDs::begin();
  Buzzer::begin();

  SdLogger::begin();
  SdLogger::writeStartupTest();

  AppState::init();
  Buzzer::startupChirp();
  renderUi();
}

void loop() {
  for (int i = 0; i < Buttons::kCount; ++i) {
    if (Buttons::isPressed(i)) {
      AppState::handleButton(Buttons::numberForIndex(i));
      SdLogger::logButtonPress(Buttons::numberForIndex(i));
      delay(170);
      renderUi();
    }
  }

  AppState::update();
  if (millis() - gLastRenderMs >= 1000) {
    gLastRenderMs = millis();
    renderUi();
  }
}
