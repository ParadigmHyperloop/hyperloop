#include "../pod.h"

/**
 * Determines if the new mode is a valid mode
 *
 * @return whether the new mode is valid knowing the gPodState
 */
bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode) {
  const static pod_mode_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
      {NonState, NonState},
      {POST, Boot, Emergency, NonState},
      {Boot, LPFill, Emergency, NonState},
      {LPFill, HPFill, Emergency, NonState},
      {HPFill, Load, Emergency, NonState},
      {Load, Standby, Emergency, NonState},
      {Standby, Load, Armed, Emergency, NonState},
      {Armed, Standby, Pushing, Emergency, NonState},
      {Pushing, Coasting, Braking, Emergency, NonState},
      {Coasting, Braking, Pushing, Emergency, NonState},
      {Braking, Pushing, Vent, Emergency, NonState},
      {Vent, Retrieval, Emergency, NonState},
      {Retrieval, NonState},
      {Emergency, Vent, NonState},
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[current_mode][0] == current_mode);

  pod_mode_t i_state;
  // Do not include Current Mode => Same Current Mode
  int i = 1;

  while ((i_state = transitions[current_mode][i]) != NonState) {
    // debug("Checking %s == %s", pod_mode_names[i_state],
    // pod_mode_names[new_mode]);
    if (i_state == new_mode) {
      return true;
    }
    i++;
  }

  return false;
}
