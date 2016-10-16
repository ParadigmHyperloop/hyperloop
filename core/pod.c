#include "pod.h"

char *pod_mode_names[N_POD_STATES] = {"Boot",     "Ready",   "Pushing",
                                      "Coasting", "Braking", "Emergency",
                                      "Shutdown", "(null)"};

pod_state_t __state = {
    .mode = Boot,
    .initialized = false,
    .accel_x = POD_VALUE_INITIALIZER,
    .accel_y = POD_VALUE_INITIALIZER,
    .accel_z = POD_VALUE_INITIALIZER,
    .velocity_x = POD_VALUE_INITIALIZER,
    .velocity_z = POD_VALUE_INITIALIZER,
    .velocity_y = POD_VALUE_INITIALIZER,
    .position_x = POD_VALUE_INITIALIZER,
    .position_y = POD_VALUE_INITIALIZER,
    .position_z = POD_VALUE_INITIALIZER,
    .skate_front_left_z = POD_VALUE_INITIALIZER,
    .skate_front_right_z = POD_VALUE_INITIALIZER,
    .skate_rear_left_z = POD_VALUE_INITIALIZER,
    .skate_rear_right_z = POD_VALUE_INITIALIZER,
};

/**
 * Determines if the new mode is a valid mode
 *
 * @return whether the new mode is valid knowing the gPodState
 */
bool validPodMode(pod_mode_t current_state, pod_mode_t new_state) {
  const static pod_mode_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
      {Boot, Ready, Emergency, Shutdown, _nil},      // 0: Boot
      {Ready, Pushing, Emergency, _nil},             // 1: Ready
      {Pushing, Coasting, Braking, Emergency, _nil}, // 2: Pushing
      {Coasting, Braking, Emergency, _nil},          // 3: Coasting
      {Braking, Shutdown, Emergency, _nil},          // 4: Braking
      {Emergency, Shutdown, _nil},                   // 5: Emergency
      {Shutdown, _nil}                               // 6: Shutdown
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[current_state][0] == current_state);

  pod_mode_t i_state;
  int i = 0;

  while ((i_state = transitions[current_state][i]) != _nil) {
    debug("Checking %d == %d", i_state, new_state);
    if (i_state == new_state) {
      return true;
    }
    i++;
  }

  return false;
}

void initializePodState(void) {
  pod_state_t *state = getPodState();
  debug("initializing State at %p", state);

  pthread_rwlock_init(&(state->mode_mutex), NULL);

  int i;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    pthread_rwlock_init(&(state->skate_solonoids[i].lock), NULL);
  }

  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    pthread_rwlock_init(&(state->ebrake_solonoids[i].lock), NULL);
  }

  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    pthread_rwlock_init(&(state->wheel_solonoids[i].lock), NULL);
  }

  // assert(sem_init(&(state->boot_sem), 0, 0) == 0);
  state->boot_sem =
      sem_open("/openloop.pod.boot", O_CREAT, S_IRUSR | S_IWUSR, 0);
  assert(state->boot_sem != SEM_FAILED);

  state->initialized = true;
}

pod_state_t *getPodState(void) {
  if (!__state.initialized) {
    warn("Pod State is not initialized");
  }

  return &__state;
}

pod_mode_t getPodMode(void) {
  pthread_rwlock_rdlock(&(getPodState()->mode_mutex));

  pod_mode_t mode = getPodState()->mode;

  pthread_rwlock_unlock(&(getPodState()->mode_mutex));

  return mode;
}

int setPodMode(pod_mode_t new_mode, char *reason) {
  warn("Pod Mode Transition %d => %d. reason: %s", getPodState()->mode,
       new_mode, reason);

  if (validPodMode(getPodState()->mode, new_mode)) {
    getPodState()->mode = new_mode;
    warn("Request to set mode from %d to %d: approved", getPodState()->mode,
         new_mode);
    return 0;
  } else {
    warn("Request to set mode from %d to %d: denied", getPodState()->mode,
         new_mode);
    return -1;
  }
}

// returns the time in microseconds
// TODO: Make nanoseconds
uint64_t getTime() {
  struct timeval currentTime;

  assert(gettimeofday(&currentTime, NULL) == 0);

  return (currentTime.tv_sec * 1000000ULL) + currentTime.tv_usec;
}

int32_t getPodField(pod_value_t *pod_field) {
  pthread_rwlock_rdlock(&(pod_field->lock));
  int32_t value = pod_field->value;
  pthread_rwlock_unlock(&(pod_field->lock));
  return value;
}

void setPodField(pod_value_t *pod_field, int32_t newValue) {
  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}

/**
 * Trigger a full controller panic and kill everything.  This is a forced dumb
 * EBRAKE.
 */
void podInterruptPanic(int subsystem, char *file, int line, char *notes, ...) {
  static char msg[MAX_LOG_LINE];
  va_list arg;
  va_start(arg, notes);
  vsnprintf(&msg[0], MAX_LOG_LINE, notes, arg);
  va_end(arg);

  fprintf(stderr, "[PANIC] %s:%d -> %s\n", file, line, msg);

  kill(getpid(), POD_SIGPANIC);
}
