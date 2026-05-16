#include "wifi_manager.h"

#include <WiFi.h>

namespace WiFiManager {
namespace {
constexpr char kWifiSsid[] = "YOUR_WIFI_NAME";
constexpr char kWifiPassword[] = "YOUR_WIFI_PASSWORD";
constexpr uint32_t kPollIntervalMs = 50;
}

void begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
}

Result connectWithTimeout(uint32_t timeoutMs) {
  WiFi.begin(kWifiSsid, kWifiPassword);

  const uint32_t startedAt = millis();
  while (millis() - startedAt < timeoutMs) {
    if (WiFi.status() == WL_CONNECTED) {
      return {true, WiFi.localIP()};
    }
    delay(kPollIntervalMs);
  }

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);
  return {false, IPAddress()};
}

}  // namespace WiFiManager
