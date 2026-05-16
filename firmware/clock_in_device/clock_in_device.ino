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
bool gButtonLatched[Buttons::kCount] = {false, false, false, false, false, false};
uint32_t gLastButtonEventMs = 0;
constexpr uint32_t kButtonRepeatGuardMs = 140;
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
    const bool pressed = Buttons::isPressed(i);
    if (pressed && !gButtonLatched[i] &&
        millis() - gLastButtonEventMs >= kButtonRepeatGuardMs) {
      AppState::handleButton(Buttons::numberForIndex(i));
      SdLogger::logButtonPress(Buttons::numberForIndex(i));
      gLastButtonEventMs = millis();
      renderUi();
    }
    gButtonLatched[i] = pressed;
  }

  const AppState::Screen screenBeforeUpdate = AppState::currentScreen();
  AppState::update();
  if (AppState::currentScreen() != screenBeforeUpdate) {
    renderUi();
  }
  if (AppState::currentScreen() == AppState::Screen::PROJECT_DETAIL &&
      AppState::currentClockState() == AppState::ClockState::CLOCKED_IN &&
      millis() - gLastRenderMs >= 1000) {
    gLastRenderMs = millis();
    renderUi();
  }
}
