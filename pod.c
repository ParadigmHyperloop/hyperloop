#include "pod.h"


/**
 * Determines if the new state is a valid state
 *
 * @return whether the new state is valid knowing the gPodState
 */
bool validPodState(pod_mode_t current_state, pod_mode_t new_state) {
  if (new_state == Emergency) {
    return true;
  }

  const static pod_mode_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
    {Boot, Ready, Emergency, Shutdown, _nil}, // Boot
    {Ready, Pushing, Emergency, _nil}, // Ready
    {Pushing, Coasting, Braking, Emergency, _nil}, // Pushing
    {Coasting, Braking, Emergency, _nil}, // Coasting
    {Braking, Shutdown, Emergency, _nil}, // Braking
    {Emergency, Shutdown, _nil}, // Emergency
    {Shutdown, _nil} // Shutdown
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[current_state][0] == current_state);

  pod_mode_t i_state;
  int i = 0;

  while ((i_state = transitions[current_state][i]) != _nil) {
    if (i_state == new_state) {
      return true;
    }
  }

  return false;
}

void initializePodState(void) {

  pod_state_t * state = getPodState();
  debug("initializing State");
  debug("%p", state);

  pthread_mutex_init(&(state->accel_x.mutex), NULL);

  pthread_mutex_init(&(state->accel_y.mutex), NULL);
  pthread_mutex_init(&(state->accel_z.mutex), NULL);
  pthread_mutex_init(&(state->velocity_x.mutex), NULL);
  pthread_mutex_init(&(state->velocity_z.mutex), NULL);
  pthread_mutex_init(&(state->velocity_y.mutex), NULL);
  pthread_mutex_init(&(state->position_x.mutex), NULL);
  pthread_mutex_init(&(state->position_y.mutex), NULL);
  pthread_mutex_init(&(state->position_z.mutex), NULL);
  pthread_mutex_init(&(state->lateral_left.mutex), NULL);
  pthread_mutex_init(&(state->lateral_right.mutex), NULL);
  pthread_mutex_init(&(state->skate_left_z.mutex), NULL);
  pthread_mutex_init(&(state->skate_right_z.mutex), NULL);
  pthread_mutex_init(&(state->photoelectric_r.mutex), NULL);
  pthread_mutex_init(&(state->photoelectric_g.mutex), NULL);
  pthread_mutex_init(&(state->photoelectric_b.mutex), NULL);

  int i;
  for (i=0; i<N_SKATE_SOLONOIDS; i++) {
    pthread_mutex_init(&(state->skate_solonoids[i].mutex), NULL);
  }

  for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
    pthread_mutex_init(&(state->ebrake_solonoids[i].mutex), NULL);
  }

  for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
    pthread_mutex_init(&(state->wheel_solonoids[i].mutex), NULL);
  }

  for (i=0; i<N_LATERAL_SOLONOIDS; i++) {
    pthread_mutex_init(&(state->lateral_solonoids[i].mutex), NULL);
  }

  state->initialized = true;
}

pod_state_t * getPodState(void) {
  static pod_state_t state = { .mode = Boot, .initialized = false };

  if (!state.initialized) {
    warn("Pod State is not initialized");
  }
  return &state;
}

int setPodMode(pod_mode_t new_mode) {

  if (validPodState(getPodState()->mode, new_mode)) {
    getPodState()->mode = new_mode;
    debug("Request to set mode from %d to %d: approved", getPodState()->mode, new_mode);
    return 0;
  } else {
    debug("Request to set mode from %d to %d: denied", getPodState()->mode, new_mode);
    return -1;
  }
}

// returns the time in microseconds
// TODO: Make nanoseconds
unsigned long getTime() {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return (currentTime.tv_sec * (int)1e6 + currentTime.tv_usec);
}
