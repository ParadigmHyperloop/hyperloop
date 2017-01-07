#ifndef OPENLOOP_POD_STATES_H
#define OPENLOOP_POD_STATES_H
#include "../pod.h"

#define N_POD_STATES 15

typedef enum {
  NonState = -1, // NULL STATE, not a valid state, used to terminate arrays
  POST = 0,
  Boot = 1,
  LPFill = 2,
  HPFill = 3,
  Load = 4,
  Standby = 5,
  Armed = 6,
  Pushing = 7,
  Coasting = 8,
  Braking = 9,
  Vent = 10,
  Retrieval = 11,
  Emergency = 12,
  Shutdown = 13,
} pod_mode_t;

typedef struct pod_value {
  union {
    int32_t int32;
    float fl;
  } value;
  pthread_rwlock_t lock;
} pod_value_t;

#define POD_VALUE_INITIALIZER_FL                                               \
  {                                                                            \
    { .fl = 0.0 }                                                              \
    , PTHREAD_RWLOCK_INITIALIZER                                               \
  }
#define POD_VALUE_INITIALIZER_INT32                                            \
  {                                                                            \
    { .int32 = 0 }                                                             \
    , PTHREAD_RWLOCK_INITIALIZER                                               \
  }

typedef struct {
  int select_pins[N_MUX_SELECT_PINS];
  int line_pin;
} pod_mux_t;

typedef enum solenoid_state { kSolenoidOpen, kSolenoidClosed } solenoid_state_t;

typedef enum relay_state {
  kRelayOff, // Relay is not actuated
  kRelayOn   // Relay is actuated
} relay_state_t;

typedef enum solenoid_type {
  kSolenoidNormallyOpen,
  kSolenoidNormallyClosed
} solenoid_type_t;

typedef struct pod_solenoid {
  int gpio;
  int value;
  bool locked;
  solenoid_type_t type;
} pod_solenoid_t;
/**
 * Information from the battery control boards
 */
typedef struct {
  pod_value_t voltage;
  pod_value_t current;
  pod_value_t temperature;
  pod_value_t charge;
  pod_value_t remaining_time;
} pod_battery_t;

typedef uint32_t thermocouple_raw_t;
typedef uint32_t transducer_raw_t;
typedef uint32_t photodiode_raw_t;
typedef uint32_t aux_raw_t;
typedef uint32_t sharp_raw_t;
typedef uint32_t omron_raw_t;

/**
 * Structure of the sensor data in the Shared PRU Memory
 */
typedef struct {
  thermocouple_raw_t lp_thermocouples[N_LP_THERMOCOUPLES];
  thermocouple_raw_t hp_thermocouple;
  thermocouple_raw_t caliper_thermocouples[N_WHEEL_THERMOCOUPLES];
  thermocouple_raw_t ebrake_line_thermocouples[N_EBRAKE_LINE_THERMOCOUPLES];
  thermocouple_raw_t ebrake_pad_thermocouples[N_EBRAKE_PAD_THERMOCOUPLES];
  thermocouple_raw_t shell_thermocouples[N_SHELL_THERMOCOUPLES];
  thermocouple_raw_t regulator_thermocouples[N_LP_REGULATOR_THERMOCOUPLES];
  thermocouple_raw_t power_thermocouples[N_POWER_THERMOCOUPLES];
  transducer_raw_t lp_regulator_transducers[N_LP_REGULATOR_TRANSDUCERS];
  transducer_raw_t hp_transducer;
  transducer_raw_t shell_transducers[N_SHELL_TRANSDUCERS];
  transducer_raw_t skate_transducers[N_SKATE_TRANSDUCERS];
  photodiode_raw_t shell_photodiodes[N_SHELL_PHOTODIODES];
  photodiode_raw_t wheel_photodiodes[N_WHEEL_PHOTODIODES];
  aux_raw_t aux_A;
  // TODO: Break out N_LATERAL_SENSORS into Left and Right
  photodiode_raw_t left_lateral_distance[N_LATERAL_SENSORS / 2];
  aux_raw_t aux_B;
  photodiode_raw_t right_lateral_distance[N_LATERAL_SENSORS / 2];
  aux_raw_t aux_C;
  omron_raw_t skate_omrons[N_SKATE_OMRONS];
  omron_raw_t wheel_omrons[N_WHEEL_OMRONS];
  aux_raw_t aux_D;
} sensor_pack_t;

typedef struct {
  uint32_t request_lock;
  const uint32_t lock_confirmed;
  sensor_pack_t data;
} pru_com_t;
/**
 * Bundles information for analog sensor reading
 */
typedef struct {
  // The internal id number for the sensor
  int sensor_id;
  // The Human readable name of the sensor
  char name[63];
  // The last calibrated sensor value
  pod_value_t value;
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

/**
 * Defines the master state of the pod
 */
typedef struct pod {
  int version;
  pod_value_t accel_x;
  pod_value_t accel_y;
  pod_value_t accel_z;

  pod_value_t velocity_x;
  pod_value_t velocity_z;
  pod_value_t velocity_y;

  pod_value_t position_x;
  pod_value_t position_y;
  pod_value_t position_z;

  pod_value_t imu_calibration_x;
  pod_value_t imu_calibration_y;
  pod_value_t imu_calibration_z;

  // Lateral Sensors
  pod_value_t lateral_front_left;
  pod_value_t lateral_front_right;
  pod_value_t lateral_rear_left;
  pod_value_t lateral_rear_right;

  // Distance From Tube Bottom
  pod_value_t skate_front_left_z;
  pod_value_t skate_front_right_z;
  pod_value_t skate_rear_left_z;
  pod_value_t skate_rear_right_z;

  // Skate Sensors and Solonoids
  pod_solenoid_t skate_solonoids[N_SKATE_SOLONOIDS];
  pod_value_t skate_transducers[N_SKATE_TRANSDUCERS];

  // LP Packages
  sensor_t lp_reg_thermocouples[N_LP_REGULATOR_THERMOCOUPLES];
  sensor_t lp_reg_transducers[N_LP_REGULATOR_TRANSDUCERS];

  // EBrake Senors and Solonoids
  pod_solenoid_t ebrake_solonoids[N_EBRAKE_SOLONOIDS];
  // pod_value_t ebrake_pressures[N_EBRAKE_PRESSURES];
  sensor_t ebrake_thermocouples[N_EBRAKE_SOLONOIDS];

  sensor_t ebrake_transducers[N_EBRAKE_TRANSDUCERS];

  // Wheel Brake Sensors and Solonoids
  pod_solenoid_t wheel_solonoids[N_WHEEL_SOLONOIDS];
  // pod_value_t wheel_pressures[N_WHEEL_PRESSURES];
  sensor_t wheel_thermocouples[N_WHEEL_THERMOCOUPLES];

  // Pusher plate
  pod_value_t pusher_plate;

  // Batteries
  pod_battery_t batteries[N_BATTERIES];

  // Thread Tracking
  pthread_t core_thread;
  pthread_t logging_thread;
  pthread_t cmd_thread;

  // Current Overall Pod Mode (Goal of the System)
  pod_mode_t mode;
  pthread_rwlock_t mode_mutex;

  // Holds the pod in a boot state until set to 1 by an operator
  pod_value_t ready;

  // Relief
  pod_solenoid_t vent_solenoid;

  // HP Fill
  pod_solenoid_t hp_fill_valve;

  // HP Transducer
  sensor_t hp_transducer;

  // LP Fill
  pod_solenoid_t lp_fill_valve[N_LP_FILL_SOLENOIDS];

  // Lateral Fill
  pod_solenoid_t lateral_fill_solenoids[N_LAT_FILL_SOLENOIDS];
  sensor_t lateral_fill_transducers[N_LAT_FILL_TRANSDUCERS];

  pod_mux_t muxes[N_MUXES];

  int imu;
  int logging_socket;
  uint64_t last_ping;
  pod_value_t core_speed;
  enum pod_caution cautions;
  enum pod_warning warnings;

  // TODO: Temporary
  int tmp_skates;
  int tmp_brakes;
  int tmp_ebrakes;

  bool calibrated;

  sem_t *boot_sem;

  uint64_t start;
  uint64_t overrides;
  pthread_rwlock_t overrides_mutex;

  bool initialized;
} pod_t;

/**
 * @brief Set the new state of the pod's control algorithms.
 *
 * If the state given is a legal state that can be transitioned to, then the
 * controller's state is changed. If the new state is an illegal state, then
 * the state is not changed
 *
 * An example of a legal state change would be  pushing -> coasting
 *
 * An illegal state change would be emergency -> coasting
 *
 * As a helper, this will also POST to a semaphore that will unlock any threads
 * waiting on a state change.
 *
 * @return Returns true in the event of a sucessful state change, false on error
 */
bool set_pod_mode(pod_mode_t new_state, char *reason, ...);

/**
 * @brief Get the mode of the pod's control algorithms.
 *
 * The pod mode generally defines the way each of the separate modules
 * (braking, skates, ect) make decisions.  For example, each module may check
 * the pod state on each loop, if the the imu module changes the state to
 * braking. The IMU will also continue it's loop and if it notices that
 * decelleration has not occured, then it will set the pod state to emergency.
 * At this point, the braking thread will loop to this change and apply
 * emergency braking
 *
 * @return the current pod state as of calling
 */
pod_mode_t get_pod_mode(void);

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

/**
 * Helper method to read value from pod_state
 */
int32_t get_value(pod_value_t *pod_field);
float get_value_f(pod_value_t *pod_field);

/**
 * Helper method to change a value from pod_state
 */
void set_value(pod_value_t *pod_field, int32_t newValue);
void set_value_f(pod_value_t *pod_field, float newValue);

/**
 * Manual override handlers
 */
void override_surface(uint64_t surfaces, bool override);
bool is_surface_overriden(uint64_t surface);

#endif
