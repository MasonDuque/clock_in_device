#include "display.h"

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void initDisplay() {
  lcd.init();
  lcd.backlight();
}

void showStartup() {
  lcd.setCursor(0, 0);
  lcd.print("Project Tracker");
}

void showSdInitializing() {
  lcd.setCursor(0, 1);
  lcd.print("Initializing SD");
}

void showSdOk() {
  lcd.setCursor(0, 2);
  lcd.print("SD Card OK      ");
}

void showSdFailed() {
  lcd.setCursor(0, 2);
  lcd.print("SD Card FAILED  ");
}

void showFileWrittenOk() {
  lcd.setCursor(0, 3);
  lcd.print("File Written OK ");
}

void showFileWriteFailed() {
  lcd.setCursor(0, 3);
  lcd.print("Write Failed    ");
}

void showPressButtons() {
  lcd.setCursor(0, 0);
  lcd.print("Press Buttons");
}

void showButtonPressed(int number) {
  lcd.setCursor(0, 1);
  lcd.print("Button ");
  lcd.print(number);
  lcd.print(" Pressed   ");
}

void showGreenLed() {
  lcd.setCursor(0, 2);
  lcd.print("GREEN LED       ");
}

void showRedLed() {
  lcd.setCursor(0, 2);
  lcd.print("RED LED         ");
}

void showLoggedToSd() {
  lcd.setCursor(0, 3);
  lcd.print("Logged to SD    ");
}

void showSdWriteError() {
  lcd.setCursor(0, 3);
  lcd.print("SD Write Error  ");
}

void clearDisplay() { lcd.clear(); }
