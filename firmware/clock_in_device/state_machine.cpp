#include "state_machine.h"

#include "buzzer.h"
#include "leds.h"

namespace AppState {
namespace {
constexpr int kProjectCount = 3;
Project kProjects[kProjectCount] = {
    {"Dress", 10 * 3600 + 12 * 60 + 13},
    {"Shirt", 2 * 24 * 3600 + 3 * 3600 + 53 * 60},
    {"Quilt", 42 * 60},
};

Screen gScreen = Screen::MAIN_MENU;
int gMenuIndex = 0;         // 0 = Continue, 1 = Delete.
int gProjectIndex = 0;
ClockState gClockState = ClockState::CLOCKED_OUT;
uint32_t gSessionStartMs = 0;
}

void init() {
  gScreen = Screen::MAIN_MENU;
  gMenuIndex = 0;
  gProjectIndex = 0;
  gClockState = ClockState::CLOCKED_OUT;
  gSessionStartMs = 0;
  LEDs::showRed();
}

void handleButton(int buttonNumber) {
  if (gScreen == Screen::MAIN_MENU) {
    if (buttonNumber == 5) {
      gMenuIndex = (gMenuIndex - 1 + 2) % 2;
    } else if (buttonNumber == 4) {
      gMenuIndex = (gMenuIndex + 1) % 2;
    } else if (buttonNumber == 6) {
      if (gMenuIndex == 0) {
        gScreen = Screen::PROJECT_LIST;
      } else {
        gScreen = Screen::DELETE_PLACEHOLDER;
      }
    }
    return;
  }

  if (gScreen == Screen::DELETE_PLACEHOLDER) {
    if (buttonNumber == 3) gScreen = Screen::MAIN_MENU;
    return;
  }

  if (gScreen == Screen::PROJECT_LIST) {
    if (buttonNumber == 5) {
      gProjectIndex = (gProjectIndex - 1 + kProjectCount) % kProjectCount;
    } else if (buttonNumber == 4) {
      gProjectIndex = (gProjectIndex + 1) % kProjectCount;
    } else if (buttonNumber == 3) {
      gScreen = Screen::MAIN_MENU;
    } else if (buttonNumber == 6) {
      gScreen = Screen::PROJECT_DETAIL;
      gClockState = ClockState::CLOCKED_OUT;
      gSessionStartMs = 0;
      LEDs::showRed();
    }
    return;
  }

  if (gScreen == Screen::PROJECT_DETAIL) {
    if (buttonNumber == 1) {
      if (gClockState == ClockState::CLOCKED_OUT) {
        gClockState = ClockState::CLOCKED_IN;
        gSessionStartMs = millis();
        LEDs::showGreen();
        Buzzer::playClockInTone();
      }
    } else if (buttonNumber == 2) {
      if (gClockState == ClockState::CLOCKED_IN) {
        const uint32_t elapsed = (millis() - gSessionStartMs) / 1000;
        kProjects[gProjectIndex].savedSeconds += elapsed;
      }
      gClockState = ClockState::CLOCKED_OUT;
      gSessionStartMs = 0;
      LEDs::showRed();
      Buzzer::playClockOutTone();
    } else if (buttonNumber == 3) {
      if (gClockState == ClockState::CLOCKED_IN) {
        const uint32_t elapsed = (millis() - gSessionStartMs) / 1000;
        kProjects[gProjectIndex].savedSeconds += elapsed;
      }
      gClockState = ClockState::CLOCKED_OUT;
      gSessionStartMs = 0;
      gScreen = Screen::PROJECT_LIST;
      LEDs::showRed();
    }
  }
}

void update() {}

Screen currentScreen() { return gScreen; }
int currentMenuIndex() { return gMenuIndex; }
int selectedProjectIndex() { return gProjectIndex; }
ClockState currentClockState() { return gClockState; }

uint32_t displayedProjectSeconds() {
  uint32_t total = kProjects[gProjectIndex].savedSeconds;
  if (gScreen == Screen::PROJECT_DETAIL && gClockState == ClockState::CLOCKED_IN) {
    total += (millis() - gSessionStartMs) / 1000;
  }
  return total;
}

const Project* projects() { return kProjects; }
int projectCount() { return kProjectCount; }

}  // namespace AppState
