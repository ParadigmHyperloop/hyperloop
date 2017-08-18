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

#ifndef PARADIGM_POD_DEFS_H
#define PARADIGM_POD_DEFS_H

typedef struct pod_value {
  union {
    int32_t int32;
    float fl;
  } value;
  pthread_rwlock_t lock;
} pod_value_t;

#define POD_VALUE_INITIALIZER_FL                                               \
{ {.fl = 0.0}, PTHREAD_RWLOCK_INITIALIZER }
#define POD_VALUE_INITIALIZER_INT32                                            \
{ {.int32 = 0}, PTHREAD_RWLOCK_INITIALIZER }

typedef enum clamp_brake_state {
  kClampBrakeClosed,
  kClampBrakeEngaged,
  kClampBrakeReleased
} clamp_brake_state_t;

typedef uint32_t thermocouple_raw_t;
typedef uint32_t transducer_raw_t;
typedef uint32_t photodiode_raw_t;
typedef uint32_t aux_raw_t;
typedef uint32_t distance_raw_t;
typedef uint32_t spare_raw_t;

/**
 * Bundles information for analog sensor reading
 */
typedef struct {
  // The internal id number for the sensor
  int sensor_id;
  // The Human readable name of the sensor
  char name[MAX_NAME];
  int mux;
  int input;
  // The last calibrated sensor value
  pod_value_t value;
  // The most recent raw ADC reading (-1 if no recent value)
  pod_value_t raw;
  // quadratic calibration coefficients ax^2 + bx + c where x is the raw value
  double cal_a;
  double cal_b;
  double cal_c;
  // low pass filter alpha
  double alpha;
  // Mannual offset
  double offset;
} sensor_t;

#define SENSOR_INITIALIZER                                                     \
{                                                                            \
.sensor_id = 0, .name = {0}, .value = POD_VALUE_INITIALIZER_INT32,         \
.cal_a = 0.0, .cal_b = 0.0, .cal_c = 0.0, .alpha = 0.0, .offset = 0.0      \
}

/**
 * Information from the battery control boards
 */
typedef struct {
  sensor_t voltage;
  sensor_t current;
  sensor_t temperature;
  sensor_t charge;
  sensor_t remaining_time;
} pod_battery_t;

typedef enum pod_caution {
  PodCautionNone = 0x00,
  LPThermocoupleLow = 0x1,
  LPThermocoupleHigh = 0x2,
  SkatePressureLow = 0x4,
  SkatePressureHigh = 0x8,
  BatteryChargeLow = 0xF,
  BatteryChargeHigh = 0x10,
  PodCautionAll = 0xFF
} pod_caution_t;

typedef enum pod_warning {
  PodWarningNone = 0x00,
  BatteryTempHigh = 0x1,
  BatteryTempLow = 0x2,
  PodWarningAll = 0xFF,
} pod_warning_t;

typedef enum pod_shutdown {
  Halt = 0,
  WarmReboot = 1,
  ColdReboot = 2,
} pod_shutdown_t;

/**
 * Defines the master state of the pod
 */
typedef struct pod {
  int version;
  char *name;
  pod_value_t accel_x;
  pod_value_t accel_y;
  pod_value_t accel_z;
  
  pod_value_t velocity_x;
  pod_value_t velocity_z;
  pod_value_t velocity_y;
  
  pod_value_t position_x;
  pod_value_t position_y;
  pod_value_t position_z;
  
  pod_value_t rotvel_x;
  pod_value_t rotvel_y;
  pod_value_t rotvel_z;
  
  pod_value_t variance_x;
  pod_value_t variance_y;
  pod_value_t variance_z;
  
  pod_value_t quaternion_real;
  pod_value_t quaternion_i;
  pod_value_t quaternion_j;
  pod_value_t quaternion_k;
  
  pod_value_t imu_calibration_x;
  pod_value_t imu_calibration_y;
  pod_value_t imu_calibration_z;
  
  // Solenoids
  solenoid_t skate_solonoids[N_SKATE_SOLONOIDS];
  solenoid_t clamp_engage_solonoids[N_CLAMP_ENGAGE_SOLONOIDS];
  solenoid_t clamp_release_solonoids[N_CLAMP_RELEASE_SOLONOIDS];
  solenoid_t wheel_solonoids[N_WHEEL_SOLONOIDS];
  solenoid_t vent_solenoid;
  solenoid_t hp_fill_valve;
  solenoid_t lp_fill_valve[N_LP_FILL_SOLENOIDS];
  solenoid_t lateral_fill_solenoids[N_LAT_FILL_SOLENOIDS];
  
  // MPYEs
  mpye_t mpye[N_MPYES];
  
  // Pressure Transducers
  sensor_t hp_pressure;
  sensor_t lateral_pressure[N_LAT_FILL_PRESSURE];
  sensor_t skate_pressure[N_SKATE_PRESSURE];
  sensor_t reg_pressure[N_REG_PRESSURE];
  
  // Thermocouples
  sensor_t hp_thermo;
  sensor_t power_thermo[N_POWER_THERMO];
  sensor_t clamp_pressure[N_CLAMP_PRESSURE];
  sensor_t reg_thermo[N_REG_THERMO];
  sensor_t reg_surf_thermo[N_REG_THERMO];
  sensor_t frame_thermo;
  sensor_t clamp_thermo[N_CLAMP_PAD_THERMO];
  
  // Distance Sensors
  sensor_t lateral_distance[N_LATERAL_DISTANCE];
  sensor_t corner_distance[N_CORNER_DISTANCE];
  sensor_t wheel_distance[N_WHEEL_DISTANCE];
  
  // RPM State
  pod_value_t rpms[N_WHEEL_PHOTO];
  uint64_t last_wheel_stripe[N_WHEEL_PHOTO];
  
  // Tube Stripe State
  
  // time when stripe was last seen by each sensor
  uint64_t last_tube_stripe[N_WHEEL_PHOTO];
  // Stripe counts as seem by each sensor individually
  pod_value_t stripe_counts[N_WHEEL_PHOTO];
  // Average time between stripes
  uint64_t stripe_intervals[N_WHEEL_PHOTO];
  // The actual stripe count
  pod_value_t stripe_count;
  
  // Pusher plate
  pod_value_t pusher_plate;
  pod_value_t pusher_plate_raw;
  uint64_t last_pusher_plate_low;
  bool pusher_plate_override;
  
  uint64_t launch_time;
  
  // Batteries
  pod_battery_t battery[N_BATTERIES];
  
  // Thread Tracking
  pthread_t core_thread;
  pthread_t logging_thread;
  pthread_t cmd_thread;
  
  // Current Overall Pod Mode (Goal of the System)
  pod_mode_t mode;
  pthread_rwlock_t mode_mutex;
  
  // Holds the pod in a boot state until set to 1 by an operator
  pod_value_t ready;
  
  // Pointers to all the solenoids that are connected to the relays
  // (Don't think too much about this one, it is really just a convienience)
  solenoid_t *relays[N_RELAY_CHANNELS];
  sensor_t *sensors[N_MUX_INPUTS * N_MUXES];
  
  bus_t i2c[N_I2C_BUSSES];

  int imu;
  int logging_socket;
  int logging_fd;
  int hw_watchdog;
  char logging_filename[PATH_MAX];
  uint64_t last_ping;
  uint64_t last_transition;
  pod_value_t core_speed;
  enum pod_caution cautions;
  enum pod_warning warnings;
  
  bool calibrated;
  
  sem_t *boot_sem;
  uint64_t last_imu_reading;
  uint64_t start;
  uint64_t overrides;
  pthread_rwlock_t overrides_mutex;
  
  pod_shutdown_t shutdown;
  
  bool initialized;
} pod_t;


/**
 * Sets the target flow through the skates on a scale of 0% to 100%
 */
int set_skate_target(int no, mpye_value_t val, bool override);

/**
 * Sets the caliper brake state
 */
int ensure_caliper_brakes(int no, solenoid_state_t val, bool override);

/**
 * Sets the clamp brake state
 */
int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override);

/**
 * Gets a bitmask representing the state of all the relays on the SSR board(s)
 */
relay_mask_t get_relay_mask(pod_t *pod);

/**
 * Performs a software self test on the pod's systems
 */
int self_tests(pod_t *pod);

/**
 * Sends the given message to all logging destinations
 */
int pod_log(char *fmt, ...);

/**
 * Dump entire pod_t to the network logging buffer
 */
void log_dump(pod_t *pod);

/**
 * Create a human understandable text description of the current pod status
 *
 * @param pod A pod with data that you want a report of
 * @param buf The buffer to put the report in
 * @param len The length of buf
 *
 * @return The length of the report in bytes, or -1 on failure
 */
int status_dump(pod_t *pod, char *buf, size_t len);

/**
 * Initiates a halt of all threads
 */
int pod_shutdown(pod_t *pod);

/**
 * Starts all threads for the pod
 */
int pod_start(pod_t *pod);

/**
 * Initializes the GPIO pins on the BBB for use with the pod
 */
void setup_pins(pod_t *state);

/**
 * Calibrate sensors based on currently read values (zero out)
 */
void pod_calibrate(void);

/**
 * Reset positional and sensor data to blank slate
 */
bool pod_reset(void);

/**
 * Shuts down the pod safely and performs any required cleanup actions
 */
void pod_exit(int code);









///////////////////

/**
 * @brief Gets a pointer to the pod state structure
 *
 * The pod state struct contains all the current state information for the pod
 * as well as mutexes for locking the values for reading and writing
 *
 * @return the current pod state as of calling
 */
pod_t *get_pod(void);

/**
 * Intiializes the pod's pod_t returned by get_pod()
 */
int init_pod(void);

static inline int32_t get_value(pod_value_t *pod_field) {
  pthread_rwlock_rdlock(&(pod_field->lock));
  int32_t value = pod_field->value.int32;
  pthread_rwlock_unlock(&(pod_field->lock));
  return value;
}

static inline float get_value_f(pod_value_t *pod_field) {
  pthread_rwlock_rdlock(&(pod_field->lock));
  float value = pod_field->value.fl;
  pthread_rwlock_unlock(&(pod_field->lock));
  return value;
}

static inline void set_value(pod_value_t *pod_field, int32_t newValue) {
  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value.int32 = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}

static inline void set_value_f(pod_value_t *pod_field, float newValue) {
#ifdef DEBUG
  if (newValue != newValue) {
    warn("Attempted to set NaN");
    return;
  }
#endif
  
  pthread_rwlock_wrlock(&(pod_field->lock));
  pod_field->value.fl = newValue;
  pthread_rwlock_unlock(&(pod_field->lock));
}


void queue_sensor(sensor_t *sensor, int32_t new_value);
float get_sensor(sensor_t *sensor);
void set_sensor(sensor_t *sensor, float val);
float update_sensor(sensor_t *sensor);

/**
 * Manual override handlers
 */
void override_surface(uint64_t surfaces, bool override);
bool is_surface_overriden(uint64_t surface);

////////////////

#endif /* PARADIGM_POD_H */
