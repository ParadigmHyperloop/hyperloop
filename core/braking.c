#include "pod.h"

// Read the presure sensors for the brakes
int readBraking(pod_state_t *state) {
  debug("[readBraking] Reading presure sensors for brakes");

  // TODO: Read in new values and put them into the state struct

  /*
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
