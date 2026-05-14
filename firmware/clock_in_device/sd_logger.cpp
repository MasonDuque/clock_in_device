#include "sd_logger.h"

#include <SD.h>
#include <SPI.h>

#include "pins.h"

namespace SdLogger {
bool begin() {
  SPI.begin(Pins::kSdSck, Pins::kSdMiso, Pins::kSdMosi, Pins::kSdChipSelect);
  return SD.begin(Pins::kSdChipSelect);
}

bool writeStartupTest() {
  File testFile = SD.open("/test.txt", FILE_WRITE);
  if (!testFile) return false;

  testFile.println("ESP32-S3 SD Test");
  testFile.close();
  return true;
}

bool logButtonPress(int number) {
  File logFile = SD.open("/log.txt", FILE_APPEND);
  if (!logFile) return false;

  logFile.print("Button ");
  logFile.print(number);
  logFile.println(" pressed");
  logFile.close();

  Serial.print("Logged Button ");
  Serial.println(number);
  return true;
}
}  // namespace SdLogger
