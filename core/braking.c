#include "pod.h"

uint32_t readWheelPressure(int thermocoupleno) {
  // TODO: Actually read in thermocouple value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

uint32_t readWheelThermocouple(int thermocoupleno) {
  // TODO: Actually read in thermocouple value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

uint32_t readEBrakePressure(int sensorno) {
  // TODO: Actually read in presure value
  // TODO: Probably want to abstract this down to a generic readPressure()
  return (getPodState()->tmp_brakes == 1 ? 50 : 0); // For simulator
}

uint32_t readEBrakeThermocouple(int thermocoupleno) {
  // TODO: Actually read in presure value
  // TODO: Probably want to abstract this down to a generic readPressure()
  return (getPodState()->tmp_ebrakes == 1 ? 50 : 0); // For simulator
}

// Read the presure sensors for the brakes
int readBraking(pod_state_t *state) {
  debug("[readBraking] Reading presure sensors for brakes");

  // TODO: Read in new values and put them into the state struct
  int i;

  for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
    int32_t new_value = readWheelPressure(i); // Read in value for wheel pressure sensor i
    setPodField(&(state->wheel_pressures[i]), new_value);
  }

  for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
    int32_t new_value = readWheelThermocouple(i); // Read in value for wheel thermal sensor i
    setPodField(&(state->wheel_thermocouples[i]), new_value);
  }

  for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
    int32_t new_value = readEBrakePressure(i); // Read in value for ebrake pressure sensor i
    setPodField(&(state->ebrake_pressures[i]), new_value);
  }

  for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
    int32_t new_value = readEBrakeThermocouple(i); // Read in value for ebrake thermal sensor i
    setPodField(&(state->ebrake_thermocouples[i]), new_value);
  }


  return 0;
}
