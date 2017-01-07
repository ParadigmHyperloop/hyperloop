#include "../pod.h"

bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode);

char *pod_mode_names[N_POD_STATES] = {
    "POST",    "Boot",      "LPFill",    "HPFill",   "Load",
    "Standby", "Armed",     "Pushing",   "Coasting", "Braking",
    "Vent",    "Retrieval", "Emergency", "Shutdown"};

pod_t _pod = {.mode = Boot,
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
              .overrides_mutex = PTHREAD_RWLOCK_INITIALIZER,
              .imu = -1,
              .logging_socket = -1,
              .last_ping = 0};

/**
* Sets the given control surfaces into override mode
*/
void override_surface(uint64_t surfaces, bool override) {
  pod_t *pod = get_pod();
  if (override) {
    pthread_rwlock_wrlock(&(pod->overrides_mutex));
    pod->overrides |= surfaces;
    pthread_rwlock_unlock(&(pod->overrides_mutex));
  } else {
    pthread_rwlock_wrlock(&(pod->overrides_mutex));
    pod->overrides &= ~surfaces;
    pthread_rwlock_unlock(&(pod->overrides_mutex));
  }
}

/**
* Sets the given control surfaces into override mode
*/
bool is_surface_overriden(uint64_t surface) {
  bool manual = false;
  pod_t *pod = get_pod();
  pthread_rwlock_rdlock(&(pod->overrides_mutex));
  manual = (bool)((pod->overrides & surface) != 0);
  pthread_rwlock_unlock(&(pod->overrides_mutex));
  return manual;
}

int init_pod(void) {
  pod_t *pod = get_pod();
  debug("initializing pod at %p", pod);

  int i;
  int skate_pins[] = SKATE_SOLENOIDS;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {

    pod->skate_solonoids[i] = (pod_solenoid_t){.gpio = skate_pins[i],
                                               .value = 0,
                                               .type = kSolenoidNormallyClosed,
                                               .locked = false};
  }

  int ebrake_pins[] = EBRAKE_SOLONOIDS;
  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    pod->ebrake_solonoids[i] = (pod_solenoid_t){.gpio = ebrake_pins[i],
                                                .value = 0,
                                                .type = kSolenoidNormallyOpen,
                                                .locked = false};
  }

  int wheel_pins[] = WHEEL_SOLONOIDS;
  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    pod->wheel_solonoids[i] = (pod_solenoid_t){.gpio = wheel_pins[i],
                                               .value = 0,
                                               .type = kSolenoidNormallyClosed,
                                               .locked = false};
  }

  int lp_fill_valves[] = LP_FILL_SOLENOIDS;
  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    pod->lp_fill_valve[i] = (pod_solenoid_t){.gpio = lp_fill_valves[i],
                                             .value = 0,
                                             .type = kSolenoidNormallyClosed,
                                             .locked = false};
  }

  pod->hp_fill_valve = (pod_solenoid_t){.gpio = HP_FILL_SOLENOID,
                                        .value = 0,
                                        .type = kSolenoidNormallyClosed,
                                        .locked = false};

  pod->vent_solenoid = (pod_solenoid_t){.gpio = VENT_SOLENOID,
                                        .value = 0,
                                        .type = kSolenoidNormallyOpen,
                                        .locked = false};

  pthread_rwlock_init(&(pod->mode_mutex), NULL);

  // assert(sem_init(&(pod->boot_sem), 0, 0) == 0);
  pod->boot_sem = sem_open(POD_BOOT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0);

  if (pod->boot_sem == SEM_FAILED) {
    error("boot_sem failed to open");
    return -1;
  }

  pod->initialized = get_time();
  return 0;
}

pod_t *get_pod(void) {
  if (!_pod.initialized) {
    warn("Pod is not initialized");
  }

  return &_pod;
}

pod_mode_t get_pod_mode(void) {
  pthread_rwlock_rdlock(&(get_pod()->mode_mutex));

  pod_mode_t mode = get_pod()->mode;

  pthread_rwlock_unlock(&(get_pod()->mode_mutex));

  return mode;
}

bool set_pod_mode(pod_mode_t new_mode, char *reason, ...) {
  static char msg[MAX_LOG_LINE];

  va_list arg;
  va_start(arg, reason);
  vsnprintf(&msg[0], MAX_LOG_LINE, reason, arg);
  va_end(arg);
  pod_t *pod = get_pod();
  pod_mode_t old_mode = get_pod_mode();

  warn("Pod Mode Transition %s => %s. reason: %s", pod_mode_names[old_mode],
       pod_mode_names[new_mode], msg);

  if (validate_transition(old_mode, new_mode)) {
    pthread_rwlock_wrlock(&(pod->mode_mutex));
    get_pod()->mode = new_mode;
    pthread_rwlock_unlock(&(pod->mode_mutex));
    warn("Request to set mode from %s to %s: approved",
         pod_mode_names[old_mode], pod_mode_names[new_mode]);
    return true;
  } else {
    warn("Request to set mode from %s to %s: denied", pod_mode_names[old_mode],
         pod_mode_names[new_mode]);
    return false;
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

float get_sensor(sensor_t *sensor) {
  float value = get_value_f(&(sensor->value)) + sensor->offset;
  return value;
}
