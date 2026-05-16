#include "display.h"

#include <LiquidCrystal_I2C.h>

#include <cstring>

LiquidCrystal_I2C lcd(0x27, 20, 4);

namespace {
AppState::Screen gLastScreen = static_cast<AppState::Screen>(-1);
int gLastMenuIndex = -1;
int gLastProjectListSelected = -1;
int gLastProjectListWindowStart = -1;
int gLastDeleteListSelected = -1;
int gLastDeleteListWindowStart = -1;
int gLastPendingDeleteIndex = -1;
const char* gLastDeletedName = nullptr;
int gLastDetailProjectIndex = -1;
AppState::ClockState gLastDetailClockState = static_cast<AppState::ClockState>(-1);
uint32_t gLastDetailSeconds = UINT32_MAX;
String gLastDetailTimerText = "";
char gLastDetailTimerRow[21] = "                    ";

String formatCompact(uint32_t seconds) {
  uint32_t days = seconds / 86400;
  seconds %= 86400;
  uint32_t hours = seconds / 3600;
  seconds %= 3600;
  uint32_t minutes = seconds / 60;

  char buf[21];
  if (days > 0) {
    snprintf(buf, sizeof(buf), "%luD%02luH%02luM", days, hours, minutes);
  } else if (hours > 0) {
    snprintf(buf, sizeof(buf), "%luH%02luM", hours, minutes);
  } else {
    snprintf(buf, sizeof(buf), "%luM", minutes);
  }
  return String(buf);
}

String formatDetailed(uint32_t seconds) {
  uint32_t hours = seconds / 3600;
  seconds %= 3600;
  uint32_t minutes = seconds / 60;
  uint32_t secs = seconds % 60;

  char buf[21];
  snprintf(buf, sizeof(buf), "%luH %02luM %02luS", hours, minutes, secs);
  return String(buf);
}



void writeRowDiff(uint8_t row, const char* previous, const char* next) {
  int start = -1;
  for (int i = 0; i < 20; ++i) {
    if (previous[i] != next[i]) {
      if (start < 0) start = i;
    } else if (start >= 0) {
      char segment[21];
      const int len = i - start;
      memcpy(segment, &next[start], len);
      segment[len] = '\0';
      lcd.setCursor(start, row);
      lcd.print(segment);
      start = -1;
    }
  }

  if (start >= 0) {
    char segment[21];
    const int len = 20 - start;
    memcpy(segment, &next[start], len);
    segment[len] = '\0';
    lcd.setCursor(start, row);
    lcd.print(segment);
  }
}

void buildCenteredRow(const String& text, char* outRow) {
  memset(outRow, ' ', 20);
  outRow[20] = '\0';

  const String trimmed = text.substring(0, 20);
  int col = (20 - trimmed.length()) / 2;
  if (col < 0) col = 0;

  for (unsigned int i = 0; i < trimmed.length() && (col + (int)i) < 20; ++i) {
    outRow[col + i] = trimmed[i];
  }
}

void printCentered(uint8_t row, const String& text) {
  int col = (20 - text.length()) / 2;
  if (col < 0) col = 0;
  lcd.setCursor(col, row);
  lcd.print(text.substring(0, 20));
}

void clearRow(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("                    ");
}

void showMainMenu() {
  const int currentIndex = AppState::currentMenuIndex();
  if (gLastScreen != AppState::Screen::MAIN_MENU) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    lcd.setCursor(0, 0);
    lcd.print("Main Menu");
    gLastMenuIndex = -1;
  }

  if (gLastMenuIndex != currentIndex) {
    lcd.setCursor(0, 1);
    lcd.print(currentIndex == 0 ? ">Continue Project" : " Continue Project");
    lcd.setCursor(0, 2);
    lcd.print(currentIndex == 1 ? ">Delete Project" : " Delete Project");
    gLastMenuIndex = currentIndex;
  }
}

void showProjectList() {
  const int selected = AppState::selectedProjectIndex();
  const int projectCount = AppState::projectCount();
  const AppState::Project* projects = AppState::projects();
  if (projectCount == 0) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    printCentered(1, "No Projects");
    printCentered(3, "BACK=Menu");
    gLastProjectListSelected = selected;
    gLastProjectListWindowStart = 0;
    return;
  }

  const int visibleRows = min(projectCount, 4);
  int windowStart = selected - (visibleRows - 1);
  if (windowStart < 0) windowStart = 0;
  const int maxWindowStart = projectCount - visibleRows;
  if (windowStart > maxWindowStart) windowStart = maxWindowStart;

  const bool firstDraw = gLastScreen != AppState::Screen::PROJECT_LIST;
  const bool windowChanged = gLastProjectListWindowStart != windowStart;

  if (firstDraw || windowChanged) {
    for (int row = 0; row < visibleRows; ++row) {
      String left = String(projects[windowStart + row].name);
      String right = formatCompact(projects[windowStart + row].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), "%c%-9s%10s",
               (windowStart + row) == selected ? '>' : ' ', left.c_str(),
               right.c_str());
      lcd.setCursor(0, row);
      lcd.print(line);
    }
    for (int row = visibleRows; row < 4; ++row) {
      clearRow(row);
    }
  } else if (gLastProjectListSelected != selected) {
    const int previousRow = gLastProjectListSelected - windowStart;
    const int currentRow = selected - windowStart;
    if (previousRow >= 0 && previousRow < visibleRows) {
      String left = String(projects[gLastProjectListSelected].name);
      String right = formatCompact(projects[gLastProjectListSelected].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), " %-9s%10s", left.c_str(), right.c_str());
      lcd.setCursor(0, previousRow);
      lcd.print(line);
    }
    if (currentRow >= 0 && currentRow < visibleRows) {
      String left = String(projects[selected].name);
      String right = formatCompact(projects[selected].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), ">%-9s%10s", left.c_str(), right.c_str());
      lcd.setCursor(0, currentRow);
      lcd.print(line);
    }
  }

  gLastProjectListSelected = selected;
  gLastProjectListWindowStart = windowStart;
}

void showProjectDetail() {
  const int projectIndex = AppState::selectedProjectIndex();
  const AppState::ClockState clockState = AppState::currentClockState();
  const uint32_t seconds = AppState::displayedProjectSeconds();
  const AppState::Project& project = AppState::projects()[projectIndex];

  if (gLastScreen != AppState::Screen::PROJECT_DETAIL) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    gLastDetailProjectIndex = -1;
    gLastDetailClockState = static_cast<AppState::ClockState>(-1);
    gLastDetailSeconds = UINT32_MAX;
    gLastDetailTimerText = "";
    memcpy(gLastDetailTimerRow, "                    ", 21);
  }

  if (gLastDetailProjectIndex != projectIndex) {
    clearRow(0);
    printCentered(0, String(project.name));
    gLastDetailProjectIndex = projectIndex;
  }

  if (gLastDetailSeconds != seconds) {
    const String timerText = formatDetailed(seconds);
    char nextTimerRow[21];
    buildCenteredRow(timerText, nextTimerRow);

    if (gLastDetailTimerText.length() != timerText.length()) {
      int oldCol = (20 - gLastDetailTimerText.length()) / 2;
      if (oldCol < 0) oldCol = 0;
      lcd.setCursor(oldCol, 1);
      for (unsigned int i = 0; i < gLastDetailTimerText.length() && (oldCol + (int)i) < 20; ++i) {
        lcd.print(' ');
      }
    }

    writeRowDiff(1, gLastDetailTimerRow, nextTimerRow);

    memcpy(gLastDetailTimerRow, nextTimerRow, sizeof(gLastDetailTimerRow));
    gLastDetailTimerText = timerText;
    gLastDetailSeconds = seconds;
  }

  String state = (AppState::currentClockState() == AppState::ClockState::CLOCKED_IN)
                     ? "CLOCKED IN"
                     : "CLOCKED OUT";
  if (gLastDetailClockState != clockState) {
    clearRow(3);
    printCentered(3, state);
    gLastDetailClockState = clockState;
  }
}

void showDeleteProjectList() {
  const int selected = AppState::selectedDeleteProjectIndex();
  const int projectCount = AppState::projectCount();
  if (projectCount == 0) {
    clearRow(1); clearRow(2); clearRow(3);
    gLastDeleteListSelected = selected;
    gLastDeleteListWindowStart = 0;
    return;
  }

  const int visibleRows = min(projectCount, 3);
  int windowStart = selected - (visibleRows - 1);
  if (windowStart < 0) windowStart = 0;
  const int maxWindowStart = projectCount - visibleRows;
  if (windowStart > maxWindowStart) windowStart = maxWindowStart;

  const bool firstDraw = gLastScreen != AppState::Screen::DELETE_PROJECT_LIST;
  const bool windowChanged = gLastDeleteListWindowStart != windowStart;
  const AppState::Project* projects = AppState::projects();

  if (firstDraw) {
    clearRow(0);
    lcd.setCursor(0, 0);
    lcd.print("Delete Project");
  }

  if (firstDraw || windowChanged) {
    for (int row = 0; row < visibleRows; ++row) {
      String left = String(projects[windowStart + row].name);
      String right = formatCompact(projects[windowStart + row].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), "%c%-9s%10s",
               (windowStart + row) == selected ? '>' : ' ',
               left.c_str(), right.c_str());
      lcd.setCursor(0, row + 1);
      lcd.print(line);
    }
    for (int row = visibleRows; row < 3; ++row) {
      clearRow(row + 1);
    }
  } else if (gLastDeleteListSelected != selected) {
    const int previousRow = gLastDeleteListSelected - windowStart;
    const int currentRow = selected - windowStart;
    if (previousRow >= 0 && previousRow < visibleRows) {
      String left = String(projects[gLastDeleteListSelected].name);
      String right = formatCompact(projects[gLastDeleteListSelected].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), " %-9s%10s", left.c_str(), right.c_str());
      lcd.setCursor(0, previousRow + 1);
      lcd.print(line);
    }
    if (currentRow >= 0 && currentRow < visibleRows) {
      String left = String(projects[selected].name);
      String right = formatCompact(projects[selected].savedSeconds);
      char line[21];
      snprintf(line, sizeof(line), ">%-9s%10s", left.c_str(), right.c_str());
      lcd.setCursor(0, currentRow + 1);
      lcd.print(line);
    }
  }

  gLastDeleteListSelected = selected;
  gLastDeleteListWindowStart = windowStart;
}

void showDeleteProjectConfirm() {
  const int pendingIdx = AppState::pendingDeleteProjectIndex();
  if (gLastScreen != AppState::Screen::DELETE_PROJECT_CONFIRM ||
      gLastPendingDeleteIndex != pendingIdx) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    lcd.setCursor(0, 0);
    lcd.print("Delete Project?");
    if (pendingIdx >= 0 && pendingIdx < AppState::projectCount()) {
      printCentered(1, String(AppState::projects()[pendingIdx].name));
    }
    printCentered(3, "OK=Yes BACK=No");
    gLastPendingDeleteIndex = pendingIdx;
  }
}

void showDeleteProjectFeedback() {
  const char* deleted = AppState::lastDeletedProjectName();
  if (gLastScreen != AppState::Screen::DELETE_PROJECT_FEEDBACK ||
      gLastDeletedName != deleted) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    printCentered(0, "Project Deleted");
    if (deleted != nullptr) {
      printCentered(1, String(deleted));
    }
    gLastDeletedName = deleted;
  }
}

void showDeleteProjectEmpty() {
  if (gLastScreen != AppState::Screen::DELETE_PROJECT_EMPTY) {
    clearRow(0); clearRow(1); clearRow(2); clearRow(3);
    printCentered(1, "No Projects");
    printCentered(3, "BACK=Menu");
  }
}

}  // namespace

void initDisplay() {
  lcd.init();
  lcd.backlight();
}

void clearDisplay() { lcd.clear(); }

void renderUi() {
  const AppState::Screen screen = AppState::currentScreen();
  if (screen != gLastScreen) {
    lcd.clear();
  }

  switch (AppState::currentScreen()) {
    case AppState::Screen::MAIN_MENU:
      showMainMenu();
      break;
    case AppState::Screen::PROJECT_LIST:
      showProjectList();
      break;
    case AppState::Screen::PROJECT_DETAIL:
      showProjectDetail();
      break;
    case AppState::Screen::DELETE_PROJECT_LIST:
      showDeleteProjectList();
      break;
    case AppState::Screen::DELETE_PROJECT_CONFIRM:
      showDeleteProjectConfirm();
      break;
    case AppState::Screen::DELETE_PROJECT_FEEDBACK:
      showDeleteProjectFeedback();
      break;
    case AppState::Screen::DELETE_PROJECT_EMPTY:
      showDeleteProjectEmpty();
      break;
  }

  gLastScreen = screen;
}
