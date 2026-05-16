#include "state_machine.h"

#include "buzzer.h"
#include "leds.h"

namespace AppState {
namespace {
constexpr int kMaxProjectCount = 3;
Project kProjects[kMaxProjectCount] = {
    {"Dress", 10 * 3600 + 12 * 60 + 13},
    {"Shirt", 2 * 24 * 3600 + 3 * 3600 + 53 * 60},
    {"Quilt", 42 * 60},
};
int gProjectCount = kMaxProjectCount;

Screen gScreen = Screen::MAIN_MENU;
int gMenuIndex = 0;         // 0 = Continue, 1 = Delete.
int gProjectIndex = 0;
int gDeleteProjectIndex = 0;
int gPendingDeleteProjectIndex = -1;
ClockState gClockState = ClockState::CLOCKED_OUT;
uint32_t gSessionStartMs = 0;
uint32_t gDeleteFeedbackStartMs = 0;
const char* gLastDeletedProjectName = nullptr;
constexpr uint32_t kDeleteFeedbackDurationMs = 1200;
}

void init() {
  gScreen = Screen::MAIN_MENU;
  gMenuIndex = 0;
  gProjectIndex = 0;
  gDeleteProjectIndex = 0;
  gPendingDeleteProjectIndex = -1;
  gClockState = ClockState::CLOCKED_OUT;
  gSessionStartMs = 0;
  gDeleteFeedbackStartMs = 0;
  gLastDeletedProjectName = nullptr;
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
        gDeleteProjectIndex = 0;
        gPendingDeleteProjectIndex = -1;
        gScreen = (gProjectCount == 0) ? Screen::DELETE_PROJECT_EMPTY
                                       : Screen::DELETE_PROJECT_LIST;
      }
    }
    return;
  }

  if (gScreen == Screen::DELETE_PROJECT_EMPTY) {
    if (buttonNumber == 3) gScreen = Screen::MAIN_MENU;
    return;
  }

  if (gScreen == Screen::DELETE_PROJECT_LIST) {
    if (buttonNumber == 5 && gProjectCount > 0) {
      gDeleteProjectIndex = (gDeleteProjectIndex - 1 + gProjectCount) % gProjectCount;
    } else if (buttonNumber == 4 && gProjectCount > 0) {
      gDeleteProjectIndex = (gDeleteProjectIndex + 1) % gProjectCount;
    } else if (buttonNumber == 3) {
      gScreen = Screen::MAIN_MENU;
    } else if (buttonNumber == 6 && gProjectCount > 0) {
      gPendingDeleteProjectIndex = gDeleteProjectIndex;
      gScreen = Screen::DELETE_PROJECT_CONFIRM;
    }
    return;
  }

  if (gScreen == Screen::DELETE_PROJECT_CONFIRM) {
    if (buttonNumber == 3) {
      gPendingDeleteProjectIndex = -1;
      gLastDeletedProjectName = nullptr;
      gScreen = Screen::DELETE_PROJECT_LIST;
    } else if (buttonNumber == 6 && gPendingDeleteProjectIndex >= 0 &&
               gPendingDeleteProjectIndex < gProjectCount) {
      gLastDeletedProjectName = kProjects[gPendingDeleteProjectIndex].name;
      for (int i = gPendingDeleteProjectIndex; i < gProjectCount - 1; ++i) {
        kProjects[i] = kProjects[i + 1];
      }
      gProjectCount--;
      if (gProjectIndex >= gProjectCount) {
        gProjectIndex = gProjectCount > 0 ? gProjectCount - 1 : 0;
      }
      if (gDeleteProjectIndex >= gProjectCount) {
        gDeleteProjectIndex = gProjectCount > 0 ? gProjectCount - 1 : 0;
      }
      gDeleteFeedbackStartMs = millis();
      gScreen = Screen::DELETE_PROJECT_FEEDBACK;
    }
    return;
  }

  if (gScreen == Screen::PROJECT_LIST) {
    if (gProjectCount == 0) {
      if (buttonNumber == 3) gScreen = Screen::MAIN_MENU;
      return;
    }
    if (buttonNumber == 5) {
      gProjectIndex = (gProjectIndex - 1 + gProjectCount) % gProjectCount;
    } else if (buttonNumber == 4) {
      gProjectIndex = (gProjectIndex + 1) % gProjectCount;
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

void update() {
  if (gScreen == Screen::DELETE_PROJECT_FEEDBACK &&
      millis() - gDeleteFeedbackStartMs >= kDeleteFeedbackDurationMs) {
    gPendingDeleteProjectIndex = -1;
    gScreen = (gProjectCount == 0) ? Screen::DELETE_PROJECT_EMPTY
                                   : Screen::DELETE_PROJECT_LIST;
  }
}

Screen currentScreen() { return gScreen; }
int currentMenuIndex() { return gMenuIndex; }
int selectedProjectIndex() { return gProjectIndex; }
int selectedDeleteProjectIndex() { return gDeleteProjectIndex; }
int pendingDeleteProjectIndex() { return gPendingDeleteProjectIndex; }
const char* lastDeletedProjectName() { return gLastDeletedProjectName; }
ClockState currentClockState() { return gClockState; }

uint32_t displayedProjectSeconds() {
  if (gProjectCount == 0) return 0;
  uint32_t total = kProjects[gProjectIndex].savedSeconds;
  if (gScreen == Screen::PROJECT_DETAIL && gClockState == ClockState::CLOCKED_IN) {
    total += (millis() - gSessionStartMs) / 1000;
  }
  return total;
}

const Project* projects() { return kProjects; }
int projectCount() { return gProjectCount; }

}  // namespace AppState
