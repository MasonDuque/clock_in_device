#include "web_dashboard.h"

#include <WebServer.h>
#include <WiFi.h>

#include "state_machine.h"

namespace WebDashboard {
namespace {
WebServer gServer(80);
bool gStarted = false;

String formatDuration(uint32_t totalSeconds) {
  const uint32_t hours = totalSeconds / 3600;
  const uint32_t minutes = (totalSeconds % 3600) / 60;
  const uint32_t seconds = totalSeconds % 60;

  char buffer[24];
  snprintf(buffer, sizeof(buffer), "%lu:%02lu:%02lu",
           static_cast<unsigned long>(hours),
           static_cast<unsigned long>(minutes),
           static_cast<unsigned long>(seconds));
  return String(buffer);
}

String currentClockStateText() {
  return AppState::currentClockState() == AppState::ClockState::CLOCKED_IN ? "Clocked In" : "Clocked Out";
}

void handleRoot() {
  String html;
  html.reserve(2048);

  html += "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Clock In Device</title></head><body>";
  html += "<h1>Clock In Device</h1>";

  html += "<p><strong>WiFi Status:</strong> ";
  html += WiFi.status() == WL_CONNECTED ? "Connected" : "Offline";
  html += "</p>";

  html += "<p><strong>IP Address:</strong> ";
  html += (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : String("N/A");
  html += "</p>";

  html += "<p><strong>Current State:</strong> ";
  html += currentClockStateText();
  html += "</p>";

  html += "<h2>Projects</h2>";

  const AppState::Project* projects = AppState::projects();
  const int count = AppState::projectCount();
  if (count == 0) {
    html += "<p>No projects configured.</p>";
  } else {
    html += "<ul>";
    for (int i = 0; i < count; ++i) {
      html += "<li><strong>";
      html += projects[i].name;
      html += "</strong>: ";

      uint32_t totalSeconds = projects[i].savedSeconds;
      if (AppState::currentScreen() == AppState::Screen::PROJECT_DETAIL &&
          AppState::selectedProjectIndex() == i &&
          AppState::currentClockState() == AppState::ClockState::CLOCKED_IN) {
        totalSeconds = AppState::displayedProjectSeconds();
      }

      html += formatDuration(totalSeconds);
      html += "</li>";
    }
    html += "</ul>";
  }

  html += "</body></html>";

  gServer.send(200, "text/html", html);
}
}  // namespace

void begin() {
  if (gStarted) return;

  gServer.on("/", handleRoot);
  gServer.begin();
  gStarted = true;
}

void handleClient() {
  if (!gStarted) return;
  gServer.handleClient();
}

}  // namespace WebDashboard
