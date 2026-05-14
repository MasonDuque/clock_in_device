#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

enum class DeviceState {
  IDLE,
  CLOCKED_IN,
  MENU,
};

void initStateMachine();
void setState(DeviceState state);
DeviceState getState();
const char* stateToString(DeviceState state);

#endif
