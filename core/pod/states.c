#include "../pod.h"
#include "../pod-helpers.h"

bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode);

char *pod_mode_names[N_POD_STATES] = {
    "POST",    "Boot",      "LPFill",    "HPFill",   "Load",
    "Standby", "Armed",     "Pushing",   "Coasting", "Braking",
    "Vent",    "Retrieval", "Emergency", "Shutdown"};

pod_t _pod = {
    .mode = Boot,
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
    .rotvel_x = POD_VALUE_INITIALIZER_FL,
    .rotvel_z = POD_VALUE_INITIALIZER_FL,
    .rotvel_y = POD_VALUE_INITIALIZER_FL,
    .quaternion_real = POD_VALUE_INITIALIZER_FL,
    .quaternion_i = POD_VALUE_INITIALIZER_FL,
    .quaternion_j = POD_VALUE_INITIALIZER_FL,
    .quaternion_k = POD_VALUE_INITIALIZER_FL,
    .overrides = 0ULL,
    .overrides_mutex = PTHREAD_RWLOCK_INITIALIZER,
    .imu = -1,
    .logging_socket = -1,
    .last_ping = 0,
    .relays = {&(_pod.skate_solonoids[0]), &(_pod.skate_solonoids[1]),
               &(_pod.skate_solonoids[2]), &(_pod.clamp_engage_solonoids[0]),
               &(_pod.clamp_release_solonoids[0]),
               &(_pod.clamp_engage_solonoids[1]),
               &(_pod.clamp_release_solonoids[1]),
               &(_pod.wheel_solonoids[0]), &(_pod.wheel_solonoids[1]),
               &(_pod.wheel_solonoids[2]), &(_pod.hp_fill_valve),
               &(_pod.vent_solenoid), &(_pod.lp_fill_valve[0]),
               &(_pod.lp_fill_valve[1]),
               &(_pod.lateral_fill_solenoids[0]),
               &(_pod.lateral_fill_solenoids[1])},
    .sensors = {0},
    .pusher_plate = POD_VALUE_INITIALIZER_INT32};

uint64_t time_in_state(void) {
  return (get_time() - get_pod()->last_transition);
}

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

  // --------------------
  // INITIALIZE SOLENOIDS
  // --------------------
  int i;
  int skate_pins[] = SKATE_SOLENOIDS;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    pod->skate_solonoids[i] = (solenoid_t){.gpio = skate_pins[i],
                                           .value = 0,
                                           .type = kSolenoidNormallyClosed,
                                           .locked = false};
    snprintf(pod->skate_solonoids[i].name, MAX_NAME, "skt_%c%c", (i*2) + 'a', (i*2) + 'b');
    setup_pin(skate_pins[i]);
  }

  int clamp_engage_pins[] = CLAMP_ENGAGE_SOLONOIDS;
  for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
    pod->clamp_engage_solonoids[i] =
        (solenoid_t){.gpio = clamp_engage_pins[i],
                     .value = 0,
                     .type = kSolenoidNormallyClosed,
                     .locked = false,
                     .name = {0}};
    snprintf(pod->clamp_engage_solonoids[i].name, MAX_NAME, "clmp_eng_%d", i);
    setup_pin(clamp_engage_pins[i]);
  }

  int clamp_release_pins[] = CLAMP_RELEASE_SOLONOIDS;
  for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
    pod->clamp_release_solonoids[i] =
        (solenoid_t){.gpio = clamp_release_pins[i],
                     .value = 0,
                     .type = kSolenoidNormallyClosed,
                     .locked = false};
    snprintf(pod->clamp_release_solonoids[i].name, MAX_NAME, "clmp_rel_%d", i);
    setup_pin(clamp_release_pins[i]);
  }

  int wheel_pins[] = WHEEL_SOLONOIDS;
  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    pod->wheel_solonoids[i] = (solenoid_t){.gpio = wheel_pins[i],
                                           .value = 0,
                                           .type = kSolenoidNormallyClosed,
                                           .locked = false};
    snprintf(pod->wheel_solonoids[i].name, MAX_NAME, "wheel_%d", i);
    setup_pin(wheel_pins[i]);
  }

  int lp_fill_valves[] = LP_FILL_SOLENOIDS;
  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    pod->lp_fill_valve[i] = (solenoid_t){.gpio = lp_fill_valves[i],
                                         .value = 0,
                                         .type = kSolenoidNormallyClosed,
                                         .locked = false};
    snprintf(pod->lp_fill_valve[i].name, MAX_NAME, "lp_fill_%d", i);
    setup_pin(lp_fill_valves[i]);
  }

  int lat_fill_solenoids[] = LAT_FILL_SOLENOIDS;
  for (i = 0; i < N_LAT_FILL_SOLENOIDS; i++) {
    pod->lateral_fill_solenoids[i] = (solenoid_t){.gpio = lat_fill_solenoids[i],
                                         .value = 0,
                                         .type = kSolenoidNormallyClosed,
                                         .locked = false};
    snprintf(pod->lateral_fill_solenoids[i].name, MAX_NAME, "lat_%d", i);
    setup_pin(lat_fill_solenoids[i]);
  }

  pod->hp_fill_valve = (solenoid_t){.gpio = HP_FILL_SOLENOID,
                                    .value = 0,
                                    .type = kSolenoidNormallyClosed,
                                    .locked = false};
  snprintf(pod->hp_fill_valve.name, MAX_NAME, "hp_fill");
  setup_pin(HP_FILL_SOLENOID);

  pod->vent_solenoid = (solenoid_t){.gpio = VENT_SOLENOID,
                                    .value = 0,
                                    .type = kSolenoidNormallyOpen,
                                    .locked = false};
  snprintf(pod->vent_solenoid.name, MAX_NAME, "vent");
  setup_pin(VENT_SOLENOID);

  // ----------------
  // Distance Sensors
  // ----------------
  int corner_distance[] = CORNER_DISTANCE_INPUTS;
  for (i = 0; i < N_CORNER_DISTANCE; i++) {
    int id = N_MUX_INPUTS*CORNER_DISTANCE_MUX + corner_distance[i];
    pod->sensors[id] = &(pod->corner_distance[i]);
    pod->corner_distance[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = 0,
      .cal_b = 1,
      .cal_c = 0,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = DISTANCE_MUX,
      .input = corner_distance[i]
    };
    snprintf(pod->sensors[id]->name, MAX_NAME, "vert_dist_%d", i);
  }

  int lateral_distance[] = LATERAL_DISTANCE_INPUTS;
  for (i = 0; i < N_LATERAL_DISTANCE; i++) {
    int id = N_MUX_INPUTS*LATERAL_DISTANCE_MUX + lateral_distance[i];
    pod->sensors[id] = &(pod->lateral_distance[i]);
    pod->lateral_distance[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = 0,
      .cal_b = 1,
      .cal_c = 0,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = DISTANCE_MUX,
      .input = lateral_distance[i]
    };
    snprintf(pod->sensors[id]->name, MAX_NAME, "lat_dist_%d", i);
  }

  int wheel_distance[] = WHEEL_DISTANCE_INPUTS;
  for (i = 0; i < N_WHEEL_DISTANCE; i++) {
    int id = N_MUX_INPUTS*WHEEL_DISTANCE_MUX + wheel_distance[i];
    pod->sensors[id] = &(pod->wheel_distance[i]);
    pod->wheel_distance[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = 0,
      .cal_b = 1,
      .cal_c = 0,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = DISTANCE_MUX,
      .input = wheel_distance[i]
    };
    snprintf(pod->sensors[id]->name, MAX_NAME, "wheel_dist_%d", i);
  }

  // --------------------
  // Pressure Transducers
  // --------------------

  int hp_pressure = HP_PRESSURE_INPUT;
  int id = N_MUX_INPUTS*PRESSURE_MUX + hp_pressure;
  pod->sensors[id] = &(pod->hp_pressure);
  pod->hp_pressure = (sensor_t){
    .sensor_id = id,
    .name = {0},
    .value = POD_VALUE_INITIALIZER_FL,
    .cal_a = 0,
    .cal_b = 1,
    .cal_c = 0,
    .alpha = 1.0,
    .offset = 0.0,
    .mux = PRESSURE_MUX,
    .input = HP_PRESSURE_INPUT
  };
  snprintf(pod->sensors[id]->name, MAX_NAME, "hp_psi");

  int reg_pressures[] = REG_PRESSURE_INPUTS;
  for (i = 0; i < N_REG_PRESSURE; i++) {
    int id = N_MUX_INPUTS*REG_PRESSURE_MUX + reg_pressures[i];
    pod->sensors[id] = &(pod->reg_pressure[i]);
    pod->reg_pressure[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = PRESSURE_MUX,
      .input = reg_pressures[i]
    };
    snprintf(pod->sensors[id]->name, MAX_NAME, "reg_psi_%c", i + 'a');
  }

  int clamp_pressure[] = CLAMP_PRESSURE_INPUTS;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    int id = N_MUX_INPUTS*CLAMP_PRESSURE_MUX + clamp_pressure[i];
    pod->sensors[id] = &(pod->clamp_pressure[i]);
    pod->clamp_pressure[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = PRESSURE_MUX,
      .input = clamp_pressure[i]
    };
    snprintf(pod->sensors[id]->name, MAX_NAME, "clmp_psi_%d", i);
  }

  int lateral_pressure[] = LAT_FILL_PRESSURE_INPUTS;
  for (i = 0; i < N_LAT_FILL_PRESSURE; i++) {
    int id = N_MUX_INPUTS*LAT_FILL_PRESSURE_MUX + lateral_pressure[i];
    pod->sensors[id] = &(pod->lateral_pressure[i]);
    pod->lateral_pressure[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
      .alpha = 1.0,
      .offset = 0.0,
      .mux = PRESSURE_MUX,
      .input = lateral_pressure[i]
    };

    snprintf(pod->sensors[id]->name, MAX_NAME, "lat_psi_%d", i);
  }

  // -------------
  // Thermocouples
  // -------------
  int reg_thermo[] = REG_THERMO_INPUTS;
  for (i = 0; i < N_REG_THERMO; i++) {
    int id = N_MUX_INPUTS*REG_THERMO_MUX + reg_thermo[i];
    pod->sensors[id] = &(pod->reg_thermo[i]);
    pod->reg_thermo[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = FLOW_THERMO_CALIBRATION_A,
      .cal_b = FLOW_THERMO_CALIBRATION_B,
      .cal_c = FLOW_THERMO_CALIBRATION_C,
      .alpha = 0.01,
      .offset = 0.0,
      .mux = REG_THERMO_MUX,
      .input = reg_thermo[i]
    };

    snprintf(pod->sensors[id]->name, MAX_NAME, "reg_thermo_%d", i);
  }

  int reg_surf_thermo[] = REG_SURF_THERMO_INPUTS;
  for (i = 0; i < N_REG_SURF_THERMO; i++) {
    int id = N_MUX_INPUTS*REG_SURF_THERMO_MUX + reg_surf_thermo[i];
    pod->sensors[id] = &(pod->reg_surf_thermo[i]);
    pod->reg_surf_thermo[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = WHITE_THERMO_CALIBRATION_A,
      .cal_b = WHITE_THERMO_CALIBRATION_B,
      .cal_c = WHITE_THERMO_CALIBRATION_C,
      .alpha = 0.01,
      .offset = 0.0,
      .mux = REG_SURF_THERMO_MUX,
      .input = reg_surf_thermo[i]
    };

    snprintf(pod->sensors[id]->name, MAX_NAME, "reg_surf_thermo_%d", i);
  }

  int power_thermo[] = POWER_THERMO_INPUTS;
  for (i = 0; i < N_POWER_THERMO; i++) {
    int id = N_MUX_INPUTS*POWER_THERMO_MUX + power_thermo[i];
    pod->sensors[id] = &(pod->power_thermo[i]);
    pod->power_thermo[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = WHITE_THERMO_CALIBRATION_A,
      .cal_b = WHITE_THERMO_CALIBRATION_B,
      .cal_c = WHITE_THERMO_CALIBRATION_C,
      .alpha = 0.01,
      .offset = 0.0,
      .mux = POWER_THERMO_MUX,
      .input = power_thermo[i]
    };

    snprintf(pod->sensors[id]->name, MAX_NAME, "power_thermo_%d", i);
  }

  int clamp_pad_thermo[] = CLAMP_PAD_THERMO_INPUTS;
  for (i = 0; i < N_CLAMP_PAD_THERMO; i++) {
    int id = N_MUX_INPUTS*CLAMP_PAD_THERMO_MUX + clamp_pad_thermo[i];
    pod->sensors[id] = &(pod->clamp_thermo[i]);
    pod->clamp_thermo[i] = (sensor_t){
      .sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = WHITE_THERMO_CALIBRATION_A,
      .cal_b = WHITE_THERMO_CALIBRATION_B,
      .cal_c = WHITE_THERMO_CALIBRATION_C,
      .alpha = 0.01,
      .offset = 0.0,
      .mux = CLAMP_PAD_THERMO_MUX,
      .input = clamp_pad_thermo[i]
    };

    snprintf(pod->sensors[id]->name, MAX_NAME, "clamp_pad_%d", i);
  }


  id = N_MUX_INPUTS*HP_THERMO_MUX + HP_THERMO_INPUT;
  pod->sensors[id] = &(pod->hp_thermo);
  pod->hp_thermo = (sensor_t){
    .sensor_id = id,
    .name = {0},
    .value = POD_VALUE_INITIALIZER_FL,
    .cal_a = FLOW_THERMO_CALIBRATION_A,
    .cal_b = FLOW_THERMO_CALIBRATION_B,
    .cal_c = FLOW_THERMO_CALIBRATION_C,
    .alpha = 1.0,
    .offset = 0.0,
    .mux = HP_THERMO_MUX,
    .input = HP_THERMO_INPUT
  };
  snprintf(pod->sensors[id]->name, MAX_NAME, "hp_thermo");

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
    pod->last_transition = get_time();
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
  if (newValue != newValue) {
    warn("Attempted to set NaN");
    return;
  }

  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value.fl = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}

float get_sensor(sensor_t *sensor) {
  float value = get_value_f(&(sensor->value)) + sensor->offset;
  return value;
}

void set_sensor(sensor_t *sensor, float value) {
  set_value_f(&(sensor->value), value);
}

float update_sensor(sensor_t *sensor, int32_t new_value) {
  float x = (float)new_value;
  float calibrated =
      (sensor->cal_a * x * x) + (sensor->cal_b * x) + sensor->cal_c;
  float filtered =
      (1.0 - sensor->alpha) * get_sensor(sensor) + (sensor->alpha) * calibrated;
  set_sensor(sensor, filtered);
  return filtered;
}
