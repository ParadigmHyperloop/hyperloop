/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#include "pod.h"

char *pod_mode_names[N_POD_STATES] = {"Boot",     "Ready",   "Pushing",
                                      "Coasting", "Braking", "Emergency",
                                      "Shutdown", "(null)"};

pod_state_t __state = {.mode = Boot,
                       .initialized = false,
                       .start = 0ULL,
                       .accel_x = POD_VALUE_INITIALIZER_FL,
                       .accel_y = POD_VALUE_INITIALIZER_FL,
                       .accel_z = POD_VALUE_INITIALIZER_FL,
                       .velocity_x = POD_VALUE_INITIALIZER_FL,
                       .velocity_z = POD_VALUE_INITIALIZER_FL,
                       .velocity_y = POD_VALUE_INITIALIZER_FL,
                       .position_x = POD_VALUE_INITIALIZER_FL,
                       .position_y = POD_VALUE_INITIALIZER_FL,
                       .position_z = POD_VALUE_INITIALIZER_FL,
                       .skate_front_left_z = POD_VALUE_INITIALIZER_INT32,
                       .skate_front_right_z = POD_VALUE_INITIALIZER_INT32,
                       .skate_rear_left_z = POD_VALUE_INITIALIZER_INT32,
                       .skate_rear_right_z = POD_VALUE_INITIALIZER_INT32,
                       .overrides = 0ULL,
                       .overrides_mutex = PTHREAD_RWLOCK_INITIALIZER};

/**
 * Sets the given control surfaces into override mode
 */
void override_surface(uint64_t surfaces, bool override) {
  pod_state_t *state = get_pod_state();
  if (override) {
    pthread_rwlock_wrlock(&(state->overrides_mutex));
    state->overrides |= surfaces;
    pthread_rwlock_unlock(&(state->overrides_mutex));
  } else {
    pthread_rwlock_wrlock(&(state->overrides_mutex));
    state->overrides &= ~surfaces;
    pthread_rwlock_unlock(&(state->overrides_mutex));
  }
}

/**
 * Sets the given control surfaces into override mode
 */
bool is_surface_overriden(uint64_t surface) {
  bool manual = false;
  pod_state_t *state = get_pod_state();
  pthread_rwlock_rdlock(&(state->overrides_mutex));
  manual = (bool)((state->overrides & surface) != 0);
  pthread_rwlock_unlock(&(state->overrides_mutex));
  return manual;
}
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

int init_pod_state(void) {
  pod_state_t *state = get_pod_state();
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
  state->boot_sem = sem_open(POD_BOOT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0);

  if (state->boot_sem == SEM_FAILED) {
    error("boot_sem failed to open");
    return -1;
  }

  state->initialized = get_time();
  return 0;
}

pod_state_t *get_pod_state(void) {
  if (!__state.initialized) {
    warn("Pod State is not initialized");
  }

  return &__state;
}

pod_mode_t get_pod_mode(void) {
  pthread_rwlock_rdlock(&(get_pod_state()->mode_mutex));

  pod_mode_t mode = get_pod_state()->mode;

  pthread_rwlock_unlock(&(get_pod_state()->mode_mutex));

  return mode;
}

int set_pod_mode(pod_mode_t new_mode, char *reason, ...) {
  static char msg[MAX_LOG_LINE];

  va_list arg;
  va_start(arg, reason);
  vsnprintf(&msg[0], MAX_LOG_LINE, reason, arg);
  va_end(arg);
  pod_state_t *state = get_pod_state();
  pod_mode_t old_mode = get_pod_mode();

  warn("Pod Mode Transition %s => %s. reason: %s", pod_mode_names[old_mode],
       pod_mode_names[new_mode], msg);

  if (validate_transition(old_mode, new_mode)) {
    pthread_rwlock_wrlock(&(state->mode_mutex));
    get_pod_state()->mode = new_mode;
    pthread_rwlock_unlock(&(state->mode_mutex));
    warn("Request to set mode from %s to %s: approved",
         pod_mode_names[old_mode], pod_mode_names[new_mode]);
    return 0;
  } else {
    warn("Request to set mode from %s to %s: denied", pod_mode_names[old_mode],
         pod_mode_names[new_mode]);
    return -1;
  }
}

// returns the time in microseconds
uint64_t get_time() {
  struct timeval currentTime;

  assert(gettimeofday(&currentTime, NULL) == 0);

  return (currentTime.tv_sec * 1000000ULL) + currentTime.tv_usec;
}

int32_t get_value(pod_value_t *pod_field) {
  pthread_rwlock_rdlock(&(pod_field->lock));
  int32_t value = pod_field->value.int32;
  pthread_rwlock_unlock(&(pod_field->lock));
  return value;
}

float get_value_f(pod_value_t *pod_field) {
  pthread_rwlock_rdlock(&(pod_field->lock));
  float value = pod_field->value.fl;
  pthread_rwlock_unlock(&(pod_field->lock));
  return value;
}

void set_value(pod_value_t *pod_field, int32_t newValue) {
  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value.int32 = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}

void set_value_f(pod_value_t *pod_field, float newValue) {
  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value.fl = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}

void pod_calibrate() {
  pod_state_t * state = get_pod_state();

  set_value_f(&(state->imu_calibration_x), get_value_f(&(state->accel_x)));
  set_value_f(&(state->imu_calibration_y), get_value_f(&(state->accel_y)));
  set_value_f(&(state->imu_calibration_z), get_value_f(&(state->accel_z)));
}

void pod_reset() {
  pod_state_t * state = get_pod_state();

  set_value_f(&(state->accel_x), 0.0);
  set_value_f(&(state->accel_y), 0.0);
  set_value_f(&(state->accel_z), 0.0);
  set_value_f(&(state->velocity_x), 0.0);
  set_value_f(&(state->velocity_z), 0.0);
  set_value_f(&(state->velocity_y), 0.0);
  set_value_f(&(state->position_x), 0.0);
  set_value_f(&(state->position_y), 0.0);
  set_value_f(&(state->position_z), 0.0);
}
/**
 * Trigger a full controller panic and kill everything.  This is a forced dumb
 * EBRAKE.
 */
void pod_panic(int subsystem, char *file, int line, char *notes, ...) {
  static char msg[MAX_LOG_LINE];
  va_list arg;
  va_start(arg, notes);
  vsnprintf(&msg[0], MAX_LOG_LINE, notes, arg);
  va_end(arg);

  fprintf(stderr, "[PANIC] %s:%d -> %s\n", file, line, msg);

  kill(getpid(), POD_SIGPANIC);
}
