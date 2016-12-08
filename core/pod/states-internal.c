#include "../pod.h"

/**
 * Determines if the new mode is a valid mode
 *
 * @return whether the new mode is valid knowing the gPodState
 */
bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode) {
  const static pod_mode_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
      {Boot, Ready, Emergency, Shutdown, _nil}, // 0: Boot
      {Ready, Pushing, Emergency, _nil},        // 1: Ready
      {Pushing, Coasting, Emergency, _nil},     // 2: Pushing
      {Coasting, Braking, Emergency, _nil},     // 3: Coasting
      {Braking, Shutdown, Emergency, _nil},     // 4: Braking
      {Emergency, Shutdown, _nil},              // 5: Emergency
      {Shutdown, _nil}                          // 6: Shutdown
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[current_mode][0] == current_mode);

  pod_mode_t i_state;
  // Do not include Current Mode => Same Current Mode
  int i = 1;

  while ((i_state = transitions[current_mode][i]) != _nil) {
    // debug("Checking %s == %s", pod_mode_names[i_state],
    // pod_mode_names[new_mode]);
    if (i_state == new_mode) {
      return true;
    }
    i++;
  }

  return false;
}
