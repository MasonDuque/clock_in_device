#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

namespace AppState {
enum class Screen {
  MAIN_MENU,
  PROJECT_LIST,
  PROJECT_DETAIL,
  DELETE_PLACEHOLDER,
};

enum class ClockState {
  CLOCKED_OUT,
  CLOCKED_IN,
};

struct Project {
  const char* name;
  uint32_t savedSeconds;
};

void init();
void handleButton(int buttonNumber);
void update();

Screen currentScreen();
int currentMenuIndex();
int selectedProjectIndex();
ClockState currentClockState();
uint32_t displayedProjectSeconds();
const Project* projects();
int projectCount();

}  // namespace AppState

#endif
