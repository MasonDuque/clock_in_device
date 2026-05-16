#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

namespace WiFiManager {

struct Result {
  bool connected;
  IPAddress ip;
};

void begin();
Result connectWithTimeout(uint32_t timeoutMs);

}  // namespace WiFiManager

#endif
