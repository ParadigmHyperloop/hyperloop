#include "../pod.h"

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
