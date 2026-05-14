#ifndef DISPLAY_H
#define DISPLAY_H

void initDisplay();
void showStartup();
void showSdInitializing();
void showSdOk();
void showSdFailed();
void showFileWrittenOk();
void showFileWriteFailed();
void showPressButtons();
void showButtonPressed(int number);
void showGreenLed();
void showRedLed();
void showLoggedToSd();
void showSdWriteError();
void clearDisplay();

#endif
