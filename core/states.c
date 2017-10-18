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
    "POST",    "Boot",      "HPFill",   "Load",
    "Standby", "Armed",     "Pushing",   "Coasting", "Braking",
    "Vent",    "Retrieval", "Emergency", "Shutdown", "Manual"};

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
      {POST, Manual, Boot, Emergency, Shutdown, NonState},
      {Boot, Manual, HPFill, Emergency, Shutdown, NonState},
      {HPFill, Manual, Load, Standby, Emergency, NonState},
      {Load, Manual, Standby, Emergency, NonState},
      {Standby, Manual, Load, Armed, Emergency, NonState},
      {Armed, Manual, Standby, Pushing, Coasting, Braking, Emergency, NonState},
      {Pushing, Manual, Coasting, Braking, Emergency, NonState},
      {Coasting, Manual, Braking, Pushing, Emergency, NonState},
      {Braking, Manual, Pushing, Vent, Emergency, Standby, NonState},
      {Vent, Manual, Retrieval, Emergency, NonState},
      {Retrieval, Manual, Shutdown, NonState},
      {Emergency, Manual, Vent, NonState},
      {Shutdown, Manual, NonState},
      {Manual, NonState}
  };

  // Ensure that the pod's current state can always transition to itself
  assert(transitions[current_mode][0] == current_mode);

  pod_mode_t i_state;
  // Do not include Current Mode => Same Current Mode
  int i = 1;

  while ((i_state = transitions[current_mode][i]) != NonState) {
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
                 &(_pod.skate_solonoids[2]), &(_pod.skate_solonoids[3]),
                 &(_pod.clamp_engage_solonoids[0]),
                 &(_pod.clamp_release_solonoids[0]),
                 &(_pod.clamp_engage_solonoids[1]),
                 &(_pod.clamp_release_solonoids[1]),
                 &(_pod.hp_fill_valve), &(_pod.vent_solenoid)},
      .sensors = {0},
      .launch_time = 0,
      .pusher_plate = POD_VALUE_INITIALIZER_INT32,
      .shutdown = Halt,
      .last_pusher_seen = 0,
      .func_test = false,
      .return_to_standby = RETURN_TO_STANDBY,
      .engaged_brakes = 0,
      .manual_emergency = false};
  
  memcpy(&_pod, &_init_pod, sizeof(_pod));
  
  pod_t *pod = &_pod;
  
  debug("Applied pod_t template to local_pod");

  char name[MAX_NAME];

  // -----------------------
  // INITIALIZE MANUAL STATE
  // -----------------------
  pod->manual.primary_brake = kClampBrakeClosed;
  pod->manual.secondary_brake = kClampBrakeClosed;
  pod->manual.vent = kSolenoidClosed;
  pod->manual.fill = kSolenoidClosed;
  pod->manual.battery_a = kSolenoidOpen;
  pod->manual.battery_b = kSolenoidOpen;
  pod->manual.skate_a = kSolenoidClosed;
  pod->manual.skate_b = kSolenoidClosed;
  pod->manual.skate_c = kSolenoidClosed;
  pod->manual.skate_d = kSolenoidClosed;
  pod->manual.mpye_a = MPYE_CLOSED_SETPOINT;
  pod->manual.mpye_b = MPYE_CLOSED_SETPOINT;
  pod->manual.mpye_c = MPYE_CLOSED_SETPOINT;
  pod->manual.mpye_d = MPYE_CLOSED_SETPOINT;

  // ----------------
  // INITIALIZE MPYES
  // ----------------
  int i;
  unsigned int mpye_pins[N_MPYES] = MPYE_CHANNELS;
  
  debug("Initializing MPYEs");
  for (i = 0; i < N_MPYES; i++) {
    snprintf(name, MAX_NAME, "mpye_%c", (i * 2) + 'a');
    mpye_init(&pod->mpye[i],
              name,
              &pod->i2c[SSR_I2C_BUS],
              mpye_pins[i] < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
              mpye_pins[i] % 16);
  }

  // --------------------
  // INITIALIZE SOLENOIDS
  // --------------------
  
  debug("Initializing Skate Solenoids");
  unsigned short skate_pins[] = SKATE_SOLENOIDS;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    snprintf(name, MAX_NAME, "skt_%c", (i * 2) + 'a');

    solenoid_init(&pod->skate_solonoids[i],
                  name,
                  &pod->i2c[SSR_I2C_BUS],
                  skate_pins[i] < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_1_ADDRESS,
                  skate_pins[i] % 16,
                  kSolenoidNormallyClosed);
  }

  debug("Initializing Brake Solenoids");

  unsigned short clamp_engage_pins[] = CLAMP_ENGAGE_SOLONOIDS;
  for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
    snprintf(name, MAX_NAME, "clmp_eng_%d", i);
    
    solenoid_init(&pod->clamp_engage_solonoids[i],
                  name,
                  &pod->i2c[SSR_I2C_BUS],
                  clamp_engage_pins[i] < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
                  clamp_engage_pins[i] % 16,
                  kSolenoidNormallyClosed);
  }

  unsigned short clamp_release_pins[] = CLAMP_RELEASE_SOLONOIDS;
  for (i = 0; i < N_CLAMP_RELEASE_SOLONOIDS; i++) {
    snprintf(name, MAX_NAME, "clmp_rel_%d", i);
    
    solenoid_init(&pod->clamp_release_solonoids[i],
                  name,
                  &pod->i2c[SSR_I2C_BUS],
                  clamp_release_pins[i] < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
                  clamp_release_pins[i] % 16,
                  kSolenoidNormallyClosed);
  }

  unsigned short battery_pack_pins[] = BATTERY_PACK_RELAYS;
  for (i = 0; i < N_BATTERY_PACK_RELAYS; i++) {
    snprintf(name, MAX_NAME, "pack_%d", i);
    
    solenoid_init(&pod->battery_pack_relays[i],
                  name,
                  &pod->i2c[SSR_I2C_BUS],
                  battery_pack_pins[i] < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
                  battery_pack_pins[i] % 16,
                  kSolenoidNormallyClosed);
  }
  
  debug("Initializing Fill and Vent Valves");

  snprintf(name, MAX_NAME, "hp_fill");

  solenoid_init(&pod->hp_fill_valve,
                name,
                &pod->i2c[SSR_I2C_BUS],
                HP_FILL_SOLENOID < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
                HP_FILL_SOLENOID % 16,
                kSolenoidNormallyClosed);
  
  
  snprintf(name, MAX_NAME, "vent");
  
  solenoid_init(&pod->vent_solenoid,
                name,
                &pod->i2c[SSR_I2C_BUS],
                VENT_SOLENOID < 16 ? SSR_BOARD_1_ADDRESS : SSR_BOARD_2_ADDRESS,
                VENT_SOLENOID % 16,
                kSolenoidNormallyOpen);
  

  debug("Initializing Distance Sensors");

  // ----------------
  // Distance Sensors
  // ----------------
  int levitation_distance[] = LEVITATION_DISTANCE_SENSORS;
  for (i = 0; i < N_LEVITATION_DISTANCE; i++) {
    int id = N_ADC_CHANNELS * LEVITATION_DISTANCE_ADC + levitation_distance[i];
    pod->sensors[id] = &(_pod.levitation_distance[i]);
    pod->levitation_distance[i] = (sensor_t){.sensor_id = id,
                                             .name = {0},
                                             .value = POD_VALUE_INITIALIZER_FL,
                                             .cal_a = DISTANCE_CALIBRATION_A,
                                             .cal_b = DISTANCE_CALIBRATION_B,
                                             .cal_c = DISTANCE_CALIBRATION_C,
                                             .alpha = 0.7,
                                             .offset = 0.0,
                                             .adc_num = LEVITATION_DISTANCE_ADC,
                                             .input = levitation_distance[i]};
    snprintf(pod->levitation_distance[i].name, MAX_NAME, "lev_%d", i);
  }

  int pusher_distance[] = PUSHER_DISTANCE_INPUTS;
  for (i = 0; i < N_PUSHER_DISTANCE; i++) {
    int id = N_ADC_CHANNELS * PUSHER_DISTANCE_ADC + pusher_distance[i];
    pod->sensors[id] = &(_pod.pusher_plate_distance[i]);
    pod->pusher_plate_distance[i] = (sensor_t){.sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = DISTANCE_CALIBRATION_A,
      .cal_b = DISTANCE_CALIBRATION_B,
      .cal_c = DISTANCE_CALIBRATION_C,
      .alpha = 0.7,
      .offset = 0.0,
      .adc_num = PUSHER_DISTANCE_ADC,
      .input = pusher_distance[i]};
    snprintf(pod->pusher_plate_distance[i].name, MAX_NAME, "pusher_%d", i);
  }
  
  
  int id;
  // --------------------
  // Pressure Transducers
  // --------------------

  debug("Initializing Transducers");

  int hp_pressure = HP_PRESSURE_INPUT;
  id = N_ADC_CHANNELS * PRESSURE_ADC + hp_pressure;
  pod->sensors[id] = &(_pod.hp_pressure);
  pod->hp_pressure = (sensor_t){.sensor_id = id,
                                .name = {0},
                                .value = POD_VALUE_INITIALIZER_FL,
                                .cal_a = HP_TRANSDUCER_CALIBRATION_A,
                                .cal_b = HP_TRANSDUCER_CALIBRATION_B,
                                .cal_c = HP_TRANSDUCER_CALIBRATION_C,
                                .alpha = 0.7,
                                .offset = 0.0,
                                .adc_num = PRESSURE_ADC,
                                .input = HP_PRESSURE_INPUT};
  snprintf(pod->hp_pressure.name, MAX_NAME, "hp_pressure");

  int reg_pressures[] = REG_PRESSURE_INPUTS;
  for (i = 0; i < N_REG_PRESSURE; i++) {
    id = N_ADC_CHANNELS * REG_PRESSURE_ADC + reg_pressures[i];
    pod->sensors[id] = &(_pod.reg_pressure[i]);
    pod->reg_pressure[i] = (sensor_t){.sensor_id = id,
                                      .name = {0},
                                      .value = POD_VALUE_INITIALIZER_FL,
                                      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
                                      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
                                      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
                                      .alpha = 0.4,
                                      .offset = 0.0,
                                      .adc_num = PRESSURE_ADC,
                                      .input = reg_pressures[i]};
    snprintf(pod->reg_pressure[i].name, MAX_NAME, "reg_pressure_%c", i + 'a');
  }

  int clamp_pressure[] = CLAMP_PRESSURE_INPUTS;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    id = N_ADC_CHANNELS * CLAMP_PRESSURE_ADC + clamp_pressure[i];
    pod->sensors[id] = &(_pod.clamp_pressure[i]);
    pod->clamp_pressure[i] = (sensor_t){.sensor_id = id,
                                        .name = {0},
                                        .value = POD_VALUE_INITIALIZER_FL,
                                        .cal_a = LP_TRANSDUCER_CALIBRATION_A,
                                        .cal_b = LP_TRANSDUCER_CALIBRATION_B,
                                        .cal_c = LP_TRANSDUCER_CALIBRATION_C,
                                        .alpha = 0.4,
                                        .offset = 0.0,
                                        .adc_num = PRESSURE_ADC,
                                        .input = clamp_pressure[i]};
    snprintf(pod->clamp_pressure[i].name, MAX_NAME, "clamp_pressure_%d", i);
  }
  
  int brake_tank_pressure[] = BRAKE_TANK_PRESSURE_INPUTS;
  for (i = 0; i < N_BRAKE_TANK_PRESSURE; i++) {
    id = N_ADC_CHANNELS * BRAKE_TANK_PRESSURE_ADC + brake_tank_pressure[i];
    pod->sensors[id] = &(_pod.brake_tank_pressure[i]);
    pod->brake_tank_pressure[i] = (sensor_t){.sensor_id = id,
      .name = {0},
      .value = POD_VALUE_INITIALIZER_FL,
      .cal_a = LP_TRANSDUCER_CALIBRATION_A,
      .cal_b = LP_TRANSDUCER_CALIBRATION_B,
      .cal_c = LP_TRANSDUCER_CALIBRATION_C,
      .alpha = 0.4,
      .offset = 0.0,
      .adc_num = PRESSURE_ADC,
      .input = brake_tank_pressure[i]};
    snprintf(pod->brake_tank_pressure[i].name, MAX_NAME, "brake_tank_pressure_%d", i);
  }


  id = N_ADC_CHANNELS * HP_FILL_VALVE_ADC + pusher_distance[i];
  pod->sensors[id] = &(_pod.hp_fill_valve_open);
  pod->hp_fill_valve_open = (sensor_t){.sensor_id = id,
    .name = {0},
    .value = POD_VALUE_INITIALIZER_FL,
    .cal_a = 0,
    .cal_b = 1,
    .cal_c = 0,
    .alpha = 1.0,
    .offset = 0.0,
    .adc_num = HP_FILL_VALVE_ADC,
    .input = HP_FILL_VALVE_OPEN_SWITCH};
  snprintf(pod->hp_fill_valve_open.name, MAX_NAME, "fill_open");
  
  
  id = N_ADC_CHANNELS * HP_FILL_VALVE_ADC + pusher_distance[i];
  pod->sensors[id] = &(_pod.hp_fill_valve_close);
  pod->hp_fill_valve_close = (sensor_t){.sensor_id = id,
    .name = {0},
    .value = POD_VALUE_INITIALIZER_FL,
    .cal_a = 0,
    .cal_b = 1,
    .cal_c = 0,
    .alpha = 1.0,
    .offset = 0.0,
    .adc_num = HP_FILL_VALVE_ADC,
    .input = HP_FILL_VALVE_OPEN_SWITCH};
  snprintf(pod->hp_fill_valve_close.name, MAX_NAME, "fill_close");
  

  
  debug("Initializing Thermocouples");

  // -------------
  // Thermocouples
  // -------------
//  int reg_thermo[] = REG_THERMO_INPUTS;
//  for (i = 0; i < N_REG_THERMO; i++) {
//    id = N_ADC_CHANNELS * REG_THERMO_MUX + reg_thermo[i];
//    pod->sensors[id] = &(_pod.reg_thermo[i]);
//    pod->reg_thermo[i] = (sensor_t){.sensor_id = id,
//                                    .name = {0},
//                                    .value = POD_VALUE_INITIALIZER_FL,
//                                    .cal_a = FLOW_THERMO_CALIBRATION_A,
//                                    .cal_b = FLOW_THERMO_CALIBRATION_B,
//                                    .cal_c = FLOW_THERMO_CALIBRATION_C,
//                                    .alpha = 0.01,
//                                    .offset = 0.0,
//                                    .adc_num = REG_THERMO_MUX,
//                                    .input = reg_thermo[i]};
//
//    snprintf(pod->reg_thermo[i].name, MAX_NAME, "reg_thermo_%d", i);
//  }
//
//  int reg_surf_thermo[] = REG_SURF_THERMO_INPUTS;
//  for (i = 0; i < N_REG_SURF_THERMO; i++) {
//    id = N_ADC_CHANNELS * REG_SURF_THERMO_MUX + reg_surf_thermo[i];
//    pod->sensors[id] = &(_pod.reg_surf_thermo[i]);
//    pod->reg_surf_thermo[i] = (sensor_t){.sensor_id = id,
//                                         .name = {0},
//                                         .value = POD_VALUE_INITIALIZER_FL,
//                                         .cal_a = WHITE_THERMO_CALIBRATION_A,
//                                         .cal_b = WHITE_THERMO_CALIBRATION_B,
//                                         .cal_c = WHITE_THERMO_CALIBRATION_C,
//                                         .alpha = 0.01,
//                                         .offset = 0.0,
//                                         .adc_num = REG_SURF_THERMO_MUX,
//                                         .input = reg_surf_thermo[i]};
//
//    snprintf(pod->reg_surf_thermo[i].name, MAX_NAME, "reg_surf_thermo_%d", i);
//  }
//
//  int power_thermo[] = POWER_THERMO_INPUTS;
//  for (i = 0; i < N_POWER_THERMO; i++) {
//    id = N_ADC_CHANNELS * POWER_THERMO_MUX + power_thermo[i];
//    pod->sensors[id] = &(_pod.power_thermo[i]);
//    pod->power_thermo[i] = (sensor_t){.sensor_id = id,
//                                      .name = {0},
//                                      .value = POD_VALUE_INITIALIZER_FL,
//                                      .cal_a = WHITE_THERMO_CALIBRATION_A,
//                                      .cal_b = WHITE_THERMO_CALIBRATION_B,
//                                      .cal_c = WHITE_THERMO_CALIBRATION_C,
//                                      .alpha = 0.01,
//                                      .offset = 0.0,
//                                      .adc_num = POWER_THERMO_MUX,
//                                      .input = power_thermo[i]};
//
//    snprintf(pod->power_thermo[i].name, MAX_NAME, "power_thermo_%d", i);
//  }
//
//  int clamp_pad_thermo[] = CLAMP_PAD_THERMO_INPUTS;
//  for (i = 0; i < N_CLAMP_PAD_THERMO; i++) {
//    id = N_ADC_CHANNELS * CLAMP_PAD_THERMO_MUX + clamp_pad_thermo[i];
//    pod->sensors[id] = &(_pod.clamp_thermo[i]);
//    pod->clamp_thermo[i] = (sensor_t){.sensor_id = id,
//                                      .name = {0},
//                                      .value = POD_VALUE_INITIALIZER_FL,
//                                      .cal_a = WHITE_THERMO_CALIBRATION_A,
//                                      .cal_b = WHITE_THERMO_CALIBRATION_B,
//                                      .cal_c = WHITE_THERMO_CALIBRATION_C,
//                                      .alpha = 0.01,
//                                      .offset = 0.0,
//                                      .adc_num = CLAMP_PAD_THERMO_MUX,
//                                      .input = clamp_pad_thermo[i]};
//
//    snprintf(pod->clamp_thermo[i].name, MAX_NAME, "clamp_pad_%d", i);
//  }
//
//  id = N_ADC_CHANNELS * HP_THERMO_MUX + HP_THERMO_INPUT;
//  pod->sensors[id] = &(_pod.hp_thermo);
//  pod->hp_thermo = (sensor_t){.sensor_id = id,
//                              .name = {0},
//                              .value = POD_VALUE_INITIALIZER_FL,
//                              .cal_a = FLOW_THERMO_CALIBRATION_A,
//                              .cal_b = FLOW_THERMO_CALIBRATION_B,
//                              .cal_c = FLOW_THERMO_CALIBRATION_C,
//                              .alpha = 1.0,
//                              .offset = 0.0,
//                              .adc_num = HP_THERMO_MUX,
//                              .input = HP_THERMO_INPUT};
//  snprintf(pod->hp_thermo.name, MAX_NAME, "hp_thermo");

  pthread_rwlock_init(&(pod->mode_mutex), NULL);

  debug("Initializing Boot Semaphore");

  // assert(sem_init(&(pod->boot_sem), 0, 0) == 0);
  pod->boot_sem = sem_open(POD_BOOT_SEM, O_CREAT, S_IRUSR | S_IWUSR, 0);
  
  debug("Boot Sem is %p", (void*)pod->boot_sem);

  if (pod->boot_sem == SEM_FAILED) {
    error("boot_sem failed to open");
    return -1;
  }

  note("Telemetry Packets are %lu bytes in size", sizeof(telemetry_packet_t));

  pod->initialized = true; // get_time_usec();

  // done
  debug("Global Pod struct is located at %p (wrote to %p)", (void *)&_pod, (void*)pod);
  
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

bool force_pod_mode(pod_mode_t new_mode, char *reason, ...) {
  static char msg[MAX_LOG_LINE];
  
  va_list arg;
  va_start(arg, reason);
  vsnprintf(&msg[0], MAX_LOG_LINE, reason, arg);
  va_end(arg);
  pod_t *pod = get_pod();
  pod_mode_t old_mode = get_pod_mode();
  
  warn("Forcing Mode Transition %s => %s. reason: %s", pod_mode_names[old_mode],
       pod_mode_names[new_mode], msg);
  
  
  pthread_rwlock_wrlock(&(pod->mode_mutex));
  get_pod()->mode = new_mode;
  pod->last_transition = get_time_usec();
  pthread_rwlock_unlock(&(pod->mode_mutex));
  warn("Request to set mode from %s to %s: approved",
       pod_mode_names[old_mode], pod_mode_names[new_mode]);
  strncpy(pod->state_reason, reason, MAX_STATE_REASON_MSG);
  return true;
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
    strncpy(pod->state_reason, reason, MAX_STATE_REASON_MSG);
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
