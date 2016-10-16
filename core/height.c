#include "pod.h"

uint32_t readDistanceSensor(int sensorno) {
  // TODO: Actually read in distance value
  return 1;
}

int heightRead(pod_state_t *state) {
  setPodField(&(state->skate_front_left_z), readDistanceSensor(0));
  setPodField(&(state->skate_front_right_z), readDistanceSensor(1));
  setPodField(&(state->skate_rear_left_z), readDistanceSensor(2));
  setPodField(&(state->skate_rear_right_z), readDistanceSensor(3));

  // TODO: Read in new values and put them into the state struct

  /*
  pod_state_t * state = getPodState();
  int i;

  for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
    int32_t new_value = 0L; // Read in value for wheel pressure sensor i
    setPodField(&(state->wheel_pressures[i]), new_value);
  }

  for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
    int32_t new_value = 0L; // Read in value for ebrake pressure sensor i
    setPodField(&(state->ebrake_pressures[i]), new_value);
  }
  */

  return 0;
}
