#include "pod.h"

pod_state_t gPodState = Boot;

/**
 * Determines if the new state is a valid state
 */
bool validPodState(pod_state_t new_state) {
  if (new_state == Emergency) {
    return true;
  }

  const static pod_state_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
    {Boot, Ready, Emergency, Shutdown, _nil}, // Boot
    {Ready, Pushing, Emergency, _nil}, // Ready
    {Pushing, Coasting, Braking, Emergency, _nil}, // Pushing
    {Coasting, Braking, Emergency, _nil}, // Coasting
    {Braking, Shutdown, Emergency, _nil}, // Braking
    {Emergency, Shutdown, _nil}, // Emergency
    {Shutdown, _nil} // Shutdown
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[gPodState][0] == gPodState);

  pod_state_t i_state;
  int i = 0;

  while ((i_state = transitions[gPodState][i]) != _nil) {
    if (i_state == new_state) {
      return true;
    }
  }

  return false;
}

pod_state_t getPodState(void) {
  return gPodState;
}

int setPodState(pod_state_t new_state) {
  if (validPodState(new_state)) {
    gPodState = new_state;
    return 0;
  } else {
    return -1;
  }
}

// returns the time in microseconds
// TODO: Make nanoseconds
long getTime() {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return (currentTime.tv_sec * (int)1e6 + currentTime.tv_usec);
}
