#include "state_machine.h"

namespace {
DeviceState currentState = DeviceState::IDLE;
}

void initStateMachine() { currentState = DeviceState::IDLE; }

void setState(DeviceState state) { currentState = state; }

DeviceState getState() { return currentState; }

const char* stateToString(DeviceState state) {
  switch (state) {
    case DeviceState::IDLE:
      return "IDLE";
    case DeviceState::CLOCKED_IN:
      return "CLOCKED_IN";
    case DeviceState::MENU:
      return "MENU";
    default:
      return "UNKNOWN";
  }
}
