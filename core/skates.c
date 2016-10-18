#include "pod.h"

uint32_t readDistanceSensor(int sensorno) {
  // TODO: Actually read in distance value
  return 1;
}

uint32_t readSkateThermocouple(int thermocoupleno) {
  // TODO: Actually read in distance value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

int skateRead(pod_state_t *state) {
  setPodField(&(state->skate_front_left_z), readDistanceSensor(0));
  setPodField(&(state->skate_front_right_z), readDistanceSensor(1));
  setPodField(&(state->skate_rear_left_z), readDistanceSensor(2));
  setPodField(&(state->skate_rear_right_z), readDistanceSensor(3));

  // TODO: Read in new values and put them into the state struct
  int i;
  for (i=0; i<N_SKATE_THERMOCOUPLES; i++) {
    int32_t new_value = readSkateThermocouple(i); // Read in value for relulator temp i
    setPodField(&(state->skate_thermocouples[i]), new_value);
  }

  return 0;
}
