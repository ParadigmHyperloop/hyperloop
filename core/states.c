/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#include "pod.h"

bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode);

char *pod_mode_names[N_POD_STATES] = {
    "POST",    "Boot",      "LPFill",    "HPFill",   "Load",
    "Standby", "Armed",     "Pushing",   "Coasting", "Braking",
    "Vent",    "Retrieval", "Emergency", "Shutdown"};

/**
 * Global Pod Structure.  This stores the entire state of the pod
 *
 * Use init_pod() and get_pod() instead of interacting with this global
 *
 * @todo Make this global static to prevent external access
 */
pod_t _pod;

/**
 * Determines if the new mode is a valid mode
 *
 * @return whether the new mode is valid knowing the gPodState
 */
bool validate_transition(pod_mode_t current_mode, pod_mode_t new_mode) {
  const static pod_mode_t transitions[N_POD_STATES][N_POD_STATES + 1] = {
      {POST, Boot, Emergency, Shutdown, NonState},
      {Boot, LPFill, Emergency, Shutdown, NonState},
      {LPFill, HPFill, Emergency, NonState},
      {HPFill, Load, Emergency, NonState},
      {Load, Standby, Emergency, NonState},
      {Standby, Load, Armed, Emergency, NonState},
      {Armed, Standby, Pushing, Emergency, NonState},
      {Pushing, Coasting, Braking, Emergency, NonState},
      {Coasting, Braking, Pushing, Emergency, NonState},
      {Braking, Pushing, Vent, Emergency, NonState},
      {Vent, Retrieval, Emergency, NonState},
      {Retrieval, Shutdown, NonState},
      {Emergency, Vent, NonState},
      {Shutdown, NonState},
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

uint64_t time_in_state(void) {
  return (get_time_usec() - get_pod()->last_transition);
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
  static pod_t _init_pod = (pod_t){
      .mode = Boot,
      .name = POD_NAME,
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
      .logging_fd = -1,
      .logging_filename = TELEMETRY_LOG_BIN,
      .last_ping = 0,
      .relays = {&(_pod.skate_solonoids[0]), &(_pod.skate_solonoids[1]),
                 &(_pod.skate_solonoids[2]), &(_pod.clamp_engage_solonoids[0]),
                 &(_pod.clamp_release_solonoids[0]),
                 &(_pod.clamp_engage_solonoids[1]),
                 &(_pod.clamp_release_solonoids[1]), &(_pod.wheel_solonoids[0]),
                 &(_pod.wheel_solonoids[1]), &(_pod.wheel_solonoids[2]),
                 &(_pod.hp_fill_valve), &(_pod.vent_solenoid),
                 &(_pod.lp_fill_valve[0]), &(_pod.lp_fill_valve[1]),
                 &(_pod.lateral_fill_solenoids[0]),
                 &(_pod.lateral_fill_solenoids[1])},
      .sensors = {0},
      .launch_time = 0,
      .pusher_plate = POD_VALUE_INITIALIZER_INT32,
      .shutdown = Halt};
  pod_t local_pod;

  memcpy(&local_pod, &_init_pod, sizeof(local_pod));

  pod_t *pod = &local_pod;

  // ----------------
  // INITIALIZE MPYES
  // ----------------
  int i;
  for (i = 0; i < N_MPYES; i++) {
    pod->mpye[i] = (mpye_t) {.pin = 0,
                             .locked = false,
                             .queued = false,
                             .value = 0};
  }

  // --------------------
  // INITIALIZE SOLENOIDS
  // --------------------
  
  int skate_pins[] = SKATE_SOLENOIDS;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    info("Setting Up Skate Solenoid %d of %d on pin %d", i, N_SKATE_SOLONOIDS, skate_pins[i]);
    pod->skate_solonoids[i] = (solenoid_t){.gpio = skate_pins[i],
                                           .value = 0,
                                           .type = kSolenoidNormallyClosed,
                                           .locked = false};
    snprintf(pod->skate_solonoids[i].name, MAX_NAME, "skt_%c%c", (i * 2) + 'a',
             (i * 2) + 'b');
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
    pod->lateral_fill_solenoids[i] =
        (solenoid_t){.gpio = lat_fill_solenoids[i],
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
    int id = N_MUX_INPUTS * CORNER_DISTANCE_MUX + corner_distance[i];
    pod->sensors[id] = &(_pod.corner_distance[i]);
    pod->corner_distance[i] = (sensor_t){.sensor_id = id,
                                         .name = {0},
                                         .value = POD_VALUE_INITIALIZER_FL,
                                         .cal_a = 0,
                                         .cal_b = 1,
                                         .cal_c = 0,
                                         .alpha = 1.0,
                                         .offset = 0.0,
                                         .mux = DISTANCE_MUX,
                                         .input = corner_distance[i]};
    snprintf(pod->corner_distance[i].name, MAX_NAME, "corner_%d", i);
  }

  int lateral_distance[] = LATERAL_DISTANCE_INPUTS;
  for (i = 0; i < N_LATERAL_DISTANCE; i++) {
    int id = N_MUX_INPUTS * LATERAL_DISTANCE_MUX + lateral_distance[i];
    pod->sensors[id] = &(_pod.lateral_distance[i]);
    pod->lateral_distance[i] = (sensor_t){.sensor_id = id,
                                          .name = {0},
                                          .value = POD_VALUE_INITIALIZER_FL,
                                          .cal_a = 0,
                                          .cal_b = 1,
                                          .cal_c = 0,
                                          .alpha = 1.0,
                                          .offset = 0.0,
                                          .mux = DISTANCE_MUX,
                                          .input = lateral_distance[i]};
    snprintf(pod->lateral_distance[i].name, MAX_NAME, "lateral_%d", i);
  }

  int wheel_distance[] = WHEEL_DISTANCE_INPUTS;
  for (i = 0; i < N_WHEEL_DISTANCE; i++) {
    int id = N_MUX_INPUTS * WHEEL_DISTANCE_MUX + wheel_distance[i];
    pod->sensors[id] = &(_pod.wheel_distance[i]);
    pod->wheel_distance[i] = (sensor_t){.sensor_id = id,
                                        .name = {0},
                                        .value = POD_VALUE_INITIALIZER_FL,
                                        .cal_a = 0,
                                        .cal_b = 1,
                                        .cal_c = 0,
                                        .alpha = 1.0,
                                        .offset = 0.0,
                                        .mux = DISTANCE_MUX,
                                        .input = wheel_distance[i]};
    snprintf(pod->wheel_distance[i].name, MAX_NAME, "wheel_%d", i);
  }

  // --------------------
  // Pressure Transducers
  // --------------------

  int hp_pressure = HP_PRESSURE_INPUT;
  int id = N_MUX_INPUTS * PRESSURE_MUX + hp_pressure;
  pod->sensors[id] = &(_pod.hp_pressure);
  pod->hp_pressure = (sensor_t){.sensor_id = id,
                                .name = {0},
                                .value = POD_VALUE_INITIALIZER_FL,
                                .cal_a = 0,
                                .cal_b = 1,
                                .cal_c = 0,
                                .alpha = 1.0,
                                .offset = 0.0,
                                .mux = PRESSURE_MUX,
                                .input = HP_PRESSURE_INPUT};
  snprintf(pod->hp_pressure.name, MAX_NAME, "hp_pressure");

  int reg_pressures[] = REG_PRESSURE_INPUTS;
  for (i = 0; i < N_REG_PRESSURE; i++) {
    id = N_MUX_INPUTS * REG_PRESSURE_MUX + reg_pressures[i];
    pod->sensors[id] = &(_pod.reg_pressure[i]);
    pod->reg_pressure[i] = (sensor_t){.sensor_id = id,
                                      .name = {0},
                                      .value = POD_VALUE_INITIALIZER_FL,
                                      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
                                      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
                                      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
                                      .alpha = 1.0,
                                      .offset = 0.0,
                                      .mux = PRESSURE_MUX,
                                      .input = reg_pressures[i]};
    snprintf(pod->reg_pressure[i].name, MAX_NAME, "reg_pressure_%c", i + 'a');
  }

  int clamp_pressure[] = CLAMP_PRESSURE_INPUTS;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    id = N_MUX_INPUTS * CLAMP_PRESSURE_MUX + clamp_pressure[i];
    pod->sensors[id] = &(_pod.clamp_pressure[i]);
    pod->clamp_pressure[i] = (sensor_t){.sensor_id = id,
                                        .name = {0},
                                        .value = POD_VALUE_INITIALIZER_FL,
                                        .cal_a = LP_TRANSDUCER_CALIBRATION_A,
                                        .cal_b = LP_TRANSDUCER_CALIBRATION_B,
                                        .cal_c = LP_TRANSDUCER_CALIBRATION_C,
                                        .alpha = 1.0,
                                        .offset = 0.0,
                                        .mux = PRESSURE_MUX,
                                        .input = clamp_pressure[i]};
    snprintf(pod->clamp_pressure[i].name, MAX_NAME, "clamp_pressure_%d", i);
  }

  int lateral_pressure[] = LAT_FILL_PRESSURE_INPUTS;
  for (i = 0; i < N_LAT_FILL_PRESSURE; i++) {
    id = N_MUX_INPUTS * LAT_FILL_PRESSURE_MUX + lateral_pressure[i];
    pod->sensors[id] = &(_pod.lateral_pressure[i]);
    pod->lateral_pressure[i] = (sensor_t){.sensor_id = id,
                                          .name = {0},
                                          .value = POD_VALUE_INITIALIZER_FL,
                                          .cal_a = LP_TRANSDUCER_CALIBRATION_A,
                                          .cal_b = LP_TRANSDUCER_CALIBRATION_B,
                                          .cal_c = LP_TRANSDUCER_CALIBRATION_C,
                                          .alpha = 1.0,
                                          .offset = 0.0,
                                          .mux = PRESSURE_MUX,
                                          .input = lateral_pressure[i]};

    snprintf(pod->lateral_pressure[i].name, MAX_NAME, "lateral_pressure_%d", i);
  }

  // -------------
  // Thermocouples
  // -------------
  int reg_thermo[] = REG_THERMO_INPUTS;
  for (i = 0; i < N_REG_THERMO; i++) {
    id = N_MUX_INPUTS * REG_THERMO_MUX + reg_thermo[i];
    pod->sensors[id] = &(_pod.reg_thermo[i]);
    pod->reg_thermo[i] = (sensor_t){.sensor_id = id,
                                    .name = {0},
                                    .value = POD_VALUE_INITIALIZER_FL,
                                    .cal_a = FLOW_THERMO_CALIBRATION_A,
                                    .cal_b = FLOW_THERMO_CALIBRATION_B,
                                    .cal_c = FLOW_THERMO_CALIBRATION_C,
                                    .alpha = 0.01,
                                    .offset = 0.0,
                                    .mux = REG_THERMO_MUX,
                                    .input = reg_thermo[i]};

    snprintf(pod->reg_thermo[i].name, MAX_NAME, "reg_thermo_%d", i);
  }

  int reg_surf_thermo[] = REG_SURF_THERMO_INPUTS;
  for (i = 0; i < N_REG_SURF_THERMO; i++) {
    id = N_MUX_INPUTS * REG_SURF_THERMO_MUX + reg_surf_thermo[i];
    pod->sensors[id] = &(_pod.reg_surf_thermo[i]);
    pod->reg_surf_thermo[i] = (sensor_t){.sensor_id = id,
                                         .name = {0},
                                         .value = POD_VALUE_INITIALIZER_FL,
                                         .cal_a = WHITE_THERMO_CALIBRATION_A,
                                         .cal_b = WHITE_THERMO_CALIBRATION_B,
                                         .cal_c = WHITE_THERMO_CALIBRATION_C,
                                         .alpha = 0.01,
                                         .offset = 0.0,
                                         .mux = REG_SURF_THERMO_MUX,
                                         .input = reg_surf_thermo[i]};

    snprintf(pod->reg_surf_thermo[i].name, MAX_NAME, "reg_surf_thermo_%d", i);
  }

  int power_thermo[] = POWER_THERMO_INPUTS;
  for (i = 0; i < N_POWER_THERMO; i++) {
    id = N_MUX_INPUTS * POWER_THERMO_MUX + power_thermo[i];
    pod->sensors[id] = &(_pod.power_thermo[i]);
    pod->power_thermo[i] = (sensor_t){.sensor_id = id,
                                      .name = {0},
                                      .value = POD_VALUE_INITIALIZER_FL,
                                      .cal_a = WHITE_THERMO_CALIBRATION_A,
                                      .cal_b = WHITE_THERMO_CALIBRATION_B,
                                      .cal_c = WHITE_THERMO_CALIBRATION_C,
                                      .alpha = 0.01,
                                      .offset = 0.0,
                                      .mux = POWER_THERMO_MUX,
                                      .input = power_thermo[i]};

    snprintf(pod->power_thermo[i].name, MAX_NAME, "power_thermo_%d", i);
  }

  int clamp_pad_thermo[] = CLAMP_PAD_THERMO_INPUTS;
  for (i = 0; i < N_CLAMP_PAD_THERMO; i++) {
    id = N_MUX_INPUTS * CLAMP_PAD_THERMO_MUX + clamp_pad_thermo[i];
    pod->sensors[id] = &(_pod.clamp_thermo[i]);
    pod->clamp_thermo[i] = (sensor_t){.sensor_id = id,
                                      .name = {0},
                                      .value = POD_VALUE_INITIALIZER_FL,
                                      .cal_a = WHITE_THERMO_CALIBRATION_A,
                                      .cal_b = WHITE_THERMO_CALIBRATION_B,
                                      .cal_c = WHITE_THERMO_CALIBRATION_C,
                                      .alpha = 0.01,
                                      .offset = 0.0,
                                      .mux = CLAMP_PAD_THERMO_MUX,
                                      .input = clamp_pad_thermo[i]};

    snprintf(pod->clamp_thermo[i].name, MAX_NAME, "clamp_pad_%d", i);
  }

  id = N_MUX_INPUTS * HP_THERMO_MUX + HP_THERMO_INPUT;
  pod->sensors[id] = &(_pod.hp_thermo);
  pod->hp_thermo = (sensor_t){.sensor_id = id,
                              .name = {0},
                              .value = POD_VALUE_INITIALIZER_FL,
                              .cal_a = FLOW_THERMO_CALIBRATION_A,
                              .cal_b = FLOW_THERMO_CALIBRATION_B,
                              .cal_c = FLOW_THERMO_CALIBRATION_C,
                              .alpha = 1.0,
                              .offset = 0.0,
                              .mux = HP_THERMO_MUX,
                              .input = HP_THERMO_INPUT};
  snprintf(pod->hp_thermo.name, MAX_NAME, "hp_thermo");

  pthread_rwlock_init(&(pod->mode_mutex), NULL);

  // assert(sem_init(&(pod->boot_sem), 0, 0) == 0);
  pod->boot_sem = sem_open(POD_BOOT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0);

  if (pod->boot_sem == SEM_FAILED) {
    error("boot_sem failed to open");
    return -1;
  }

  pod->initialized = true; // get_time_usec();

  // We are done, so overwrite the global _pod struct
  debug("Global Pod struct is located at %p", (void *)&_pod);
  memcpy(&_pod, &local_pod, sizeof(local_pod));
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
    pod->last_transition = get_time_usec();
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

float get_sensor(sensor_t *sensor) {
  float value = get_value_f(&(sensor->value)) + (float)sensor->offset;
  return value;
}

void set_sensor(sensor_t *sensor, float value) {
  set_value_f(&(sensor->value), value);
}

void queue_sensor(sensor_t *sensor, int32_t new_value) {
  set_value(&(sensor->raw), new_value);
}

float update_sensor(sensor_t *sensor) {
  // Grab the raw value out of the sensor
  float x = (float)get_value(&(sensor->raw));;

  if (x == -1.0f) {
    return x;
  }

  // Dequeue the raw sensor reading
  set_value(&(sensor->raw), -1);;

  float calibrated = ((float)sensor->cal_a * x * x) +
                     ((float)sensor->cal_b * x) + (float)sensor->cal_c;
  float filtered = (1.0f - (float)sensor->alpha) * get_sensor(sensor) +
                   ((float)sensor->alpha) * calibrated;
  set_sensor(sensor, filtered);
  return filtered;
}
