#include "display.h"

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

namespace {
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
  clearRow(0); clearRow(1); clearRow(2); clearRow(3);
  lcd.setCursor(0, 0);
  lcd.print("Main Menu");
  lcd.setCursor(0, 1);
  lcd.print(AppState::currentMenuIndex() == 0 ? ">Continue Project" : " Continue Project");
  lcd.setCursor(0, 2);
  lcd.print(AppState::currentMenuIndex() == 1 ? ">Delete Project" : " Delete Project");
}

void showProjectList() {
  clearRow(0); clearRow(1); clearRow(2); clearRow(3);
  const AppState::Project* projects = AppState::projects();
  int selected = AppState::selectedProjectIndex();
  for (int row = 0; row < AppState::projectCount() && row < 4; ++row) {
    String left = String(projects[row].name);
    String right = formatCompact(projects[row].savedSeconds);
    char line[21];
    snprintf(line, sizeof(line), "%c%-9s%10s", row == selected ? '>' : ' ', left.c_str(), right.c_str());
    lcd.setCursor(0, row);
    lcd.print(line);
  }
}

void showProjectDetail() {
  clearRow(0); clearRow(1); clearRow(2); clearRow(3);
  const AppState::Project& project = AppState::projects()[AppState::selectedProjectIndex()];
  printCentered(0, String(project.name));
  printCentered(1, formatDetailed(AppState::displayedProjectSeconds()));
  String state = (AppState::currentClockState() == AppState::ClockState::CLOCKED_IN)
                     ? "CLOCKED IN"
                     : "CLOCKED OUT";
  printCentered(3, state);
}

void showDeletePlaceholder() {
  clearRow(0); clearRow(1); clearRow(2); clearRow(3);
  printCentered(1, "Delete not ready");
  printCentered(3, "Press Back");
}

}  // namespace

void initDisplay() {
  lcd.init();
  lcd.backlight();
}

void clearDisplay() { lcd.clear(); }

void renderUi() {
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
    case AppState::Screen::DELETE_PLACEHOLDER:
      showDeletePlaceholder();
      break;
  }
}
