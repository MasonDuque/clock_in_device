#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

namespace AppState {
enum class Screen {
  MAIN_MENU,
  PROJECT_LIST,
  PROJECT_DETAIL,
  ADD_PROJECT,
  ADD_PROJECT_FEEDBACK,
  ADD_PROJECT_ERROR,
  DELETE_PROJECT_LIST,
  DELETE_PROJECT_CONFIRM,
  DELETE_PROJECT_FEEDBACK,
  DELETE_PROJECT_EMPTY,
};

enum class ClockState {
  CLOCKED_OUT,
  CLOCKED_IN,
};

struct Project {
  char name[13];
  uint32_t savedSeconds;
};

void init();
void handleButton(int buttonNumber);
void update();

Screen currentScreen();
int currentMenuIndex();
int selectedProjectIndex();
int selectedDeleteProjectIndex();
int pendingDeleteProjectIndex();
const char* lastDeletedProjectName();
ClockState currentClockState();
uint32_t displayedProjectSeconds();
const Project* projects();
int projectCount();

const char* addProjectDraftName();
char addProjectSelectedChar();
const char* addProjectFeedbackName();
const char* addProjectErrorMessage();

}  // namespace AppState

#endif
