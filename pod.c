#include "pod.h"

pod_state_t gPodState = BOOT;

/**
 * Determines if the new state is a valid state
 */
bool valid_state(pod_state_t new_state) {
  if (new_state == EMERGENCY) {
    return true;
  }

  const static pod_state_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
    {BOOT, READY, EMERGENCY, SHUTDOWN, _NIL}, // BOOT
    {READY, PUSHING, EMERGENCY, _NIL}, // READY
    {PUSHING, COASTING, BRAKING, EMERGENCY, _NIL}, // PUSHING
    {COASTING, BRAKING, EMERGENCY, _NIL}, // COASTING
    {BRAKING, SHUTDOWN, EMERGENCY, _NIL}, // BRAKING
    {EMERGENCY, SHUTDOWN, _NIL}, // EMERGENCY
    {SHUTDOWN, _NIL} // SHUTDOWN
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[gPodState][0] == gPodState);

  pod_state_t i_state;
  int i = 0;

  while ((i_state = transitions[gPodState][i]) != _NIL) {
    if (i_state == new_state) {
      return true;
    }
  }

  return false;
}

pod_state_t get_pod_state(void) {
  return gPodState;
}

int set_pod_state(pod_state_t new_state) {
  if (valid_state(new_state)) {
    gPodState = new_state;
    return 0;
  } else {
    return -1;
  }
}
