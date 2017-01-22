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

typedef enum solenoid_state { kSolenoidError, kSolenoidOpen, kSolenoidClosed } solenoid_state_t;

typedef enum relay_state {
  kRelayError,
  kRelayOff, // Relay is not actuated
  kRelayOn   // Relay is actuated
} relay_state_t;

typedef enum solenoid_type {
  kSolenoidNormallyOpen,
  kSolenoidNormallyClosed
} solenoid_type_t;

typedef struct pod_solenoid {
  // The GPIO pin used to control this solenoid
  int gpio;
  // The Human Readable name of the solenoid
  char name[MAX_NAME];
  // The current value of this solenoid (0 for default position, 1 for active)
  int value;
  // Prevent this solenoid from changing state without an explicit unlock
  bool locked;
  // The logic type of the solenoid (Normally Open or Normally Closed)
  solenoid_type_t type;
} solenoid_t;

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
 * Structure of the sensor data in the Shared PRU Memory
 */
typedef struct {
  // MUX 0
  thermocouple_raw_t reg_thermo[N_REG_THERMO];
  thermocouple_raw_t hp_thermo[N_HP_THERMO];
  spare_raw_t thermo_0_spare[16 - N_REG_THERMO - N_HP_THERMO];

  // MUX 1
  thermocouple_raw_t reg_surf_thermo[N_REG_SURF_THERMO];
  thermocouple_raw_t clamp_line_thermo[N_CLAMP_PAD_THERMO];
  thermocouple_raw_t clamp_pad_thermo[N_POWER_THERMO];
  thermocouple_raw_t frame_thermo[N_FRAME_THERMO];
  spare_raw_t thermo_1_spare[16 - N_REG_SURF_THERMO - N_CLAMP_PAD_THERMO -
                             N_POWER_THERMO - N_FRAME_THERMO];

  // MUX 2
  transducer_raw_t reg_pressure[N_REG_PRESSURE];
  transducer_raw_t clamp_pressure[N_CLAMP_PRESSURE];
  transducer_raw_t lateral_pressure[N_LAT_FILL_PRESSURE];
  transducer_raw_t skate_pressure[N_SKATE_PRESSURE];
  transducer_raw_t hp_pressure[N_HP_PRESSURE];
  spare_raw_t pressure_spare[16 - N_REG_PRESSURE - N_CLAMP_PRESSURE -
                             N_LAT_FILL_PRESSURE - N_SKATE_PRESSURE -
                             N_HP_PRESSURE];

  // MUX 3
  distance_raw_t corner_distance[N_CORNER_DISTANCE];
  distance_raw_t wheel_distance[N_WHEEL_DISTANCE];
  distance_raw_t lateral_distance[N_LATERAL_DISTANCE];
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
  char name[MAX_NAME];
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

  pod_value_t rotvel_x;
  pod_value_t rotvel_y;
  pod_value_t rotvel_z;

  pod_value_t quaternion_real;
  pod_value_t quaternion_i;
  pod_value_t quaternion_j;
  pod_value_t quaternion_k;

  pod_value_t imu_calibration_x;
  pod_value_t imu_calibration_y;
  pod_value_t imu_calibration_z;

  // Lateral Sensors
  sensor_t lateral_distance[N_LATERAL_DISTANCE];

  // Distance From Tube Bottom
  sensor_t corner_distance[N_CORNER_DISTANCE];
  sensor_t wheel_distance[N_WHEEL_DISTANCE];

  // Skate Sensors and Solonoids
  solenoid_t skate_solonoids[N_SKATE_SOLONOIDS];
  sensor_t skate_pressure[N_SKATE_PRESSURE];

  // LP Packages
  sensor_t reg_thermo[N_REG_THERMO];
  sensor_t reg_surf_thermo[N_REG_THERMO];
  sensor_t reg_pressure[N_REG_PRESSURE];

  // Frame Temperature
  sensor_t frame_thermo;

  // Clamping Brakes
  solenoid_t clamp_engage_solonoids[N_CLAMP_ENGAGE_SOLONOIDS];
  solenoid_t clamp_release_solonoids[N_CLAMP_RELEASE_SOLONOIDS];

  // Clamp Tank Pressure
  sensor_t clamp_pressure[N_CLAMP_PRESSURE];

  // Clamp pad thermocouples
  sensor_t clamp_thermocouples[N_CLAMP_PAD_THERMO];

  // Wheel Brake Sensors and Solonoids
  solenoid_t wheel_solonoids[N_WHEEL_SOLONOIDS];

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

  // Relief
  solenoid_t vent_solenoid;

  // HP Fill
  solenoid_t hp_fill_valve;

  // HP Transducer
  sensor_t hp_pressure;
  sensor_t hp_thermo;

  // LP Fill
  solenoid_t lp_fill_valve[N_LP_FILL_SOLENOIDS];

  // Lateral Fill
  solenoid_t lateral_fill_solenoids[N_LAT_FILL_SOLENOIDS];
  sensor_t lateral_pressure[N_LAT_FILL_PRESSURE];

  // Pointers to all the solenoids that are connected to the relays
  // (Don't think too much about this one, it is really just a convienience)
  solenoid_t *relays[N_RELAY_CHANNELS];
  sensor_t *sensors[N_MUX_INPUTS*N_MUXES];

  int imu;
  int logging_socket;
  uint64_t last_ping;
  uint64_t last_transition;
  pod_value_t core_speed;
  enum pod_caution cautions;
  enum pod_warning warnings;

  // TODO: Temporary
  int tmp_skates;
  int tmp_brakes;
  int tmp_clamps;

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

float get_sensor(sensor_t *sensor);
void set_sensor(sensor_t *sensor, float val);
float update_sensor(sensor_t *sensor, int32_t raw);

/**
 * Manual override handlers
 */
void override_surface(uint64_t surfaces, bool override);
bool is_surface_overriden(uint64_t surface);

uint64_t time_in_state(void);
#endif
