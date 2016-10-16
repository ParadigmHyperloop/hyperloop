#include "cdefs.h"
#include "config.h"
#include <pthread.h>
#include <sys/queue.h>

#ifndef OPENLOOP_POD_H
#define OPENLOOP_POD_H

#define N_POD_STATES 8


typedef enum {
  Boot      = 0, // initializing systems, establishing network connections, ect
  Ready     = 1, // idle, stationary, ready for push
  Pushing   = 2, // pusher engaged,
  Coasting  = 3, // pusher disengaged, just coasting
  Braking   = 4, // normal braking mode
  Emergency = 5, // emergency braking
  Shutdown  = 6, // pod stationary and in a safe state
  _nil      = 7  // NULL STATE, not a valid state, used to terminate arrays
} pod_mode_t;

typedef struct pod_value {
  int32_t value; // TODO: either make this a union or more structs
  pthread_rwlock_t lock;
} pod_value_t;


#define POD_VALUE_INITIALIZER { 0, PTHREAD_RWLOCK_INITIALIZER }

/**
 * Defines the master state of the pod
 */
typedef struct pod_state {
  pod_value_t accel_x;
  pod_value_t accel_y;
  pod_value_t accel_z;

  pod_value_t velocity_x;
  pod_value_t velocity_z;
  pod_value_t velocity_y;

  pod_value_t position_x;
  pod_value_t position_y;
  pod_value_t position_z;

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
  pod_value_t skate_solonoids[N_SKATE_SOLONOIDS];
  pod_value_t skate_thermocouples[N_SKATE_SOLONOIDS];

  // EBrake Senors and Solonoids
  pod_value_t ebrake_solonoids[N_EBRAKE_SOLONOIDS];
  pod_value_t ebrake_pressures[N_EBRAKE_PRESSURES];
  pod_value_t ebrake_thermocouples[N_EBRAKE_SOLONOIDS];

  // Wheel Brake Sensors and Solonoids
  pod_value_t wheel_solonoids[N_WHEEL_SOLONOIDS];
  pod_value_t wheel_pressures[N_WHEEL_PRESSURES];
  pod_value_t wheel_thermocouples[N_WHEEL_THERMOCOUPLES];

  // Thread Tracking
  pthread_t imu_thread;
  pthread_t distance_thread;
  pthread_t braking_thread;
  pthread_t lateral_thread;
  pthread_t logging_thread;
  pthread_t cmd_thread;

  // Current Overall Pod Mode (Goal of the System)
  pod_mode_t mode;
  pthread_rwlock_t mode_mutex;

  bool initialized;
} pod_state_t;

typedef enum {
  Message,
  Telemetry
} log_type_t;

typedef struct {
  char *tag;
  uint32_t data;
} log_data_t;


union log_content {
  char * message;
  log_data_t data;
};

typedef struct log {
  log_type_t type;
  union {
    char * message;
    log_data_t data;
  } content;
//  STAILQ_ENTRY(struct log) entries;
} log_t;

/**
 * Sends the given message to all logging destinations
 */
int podLog(char * fmt, ...);

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
 * @return Returns 0 in the event of a sucessful state change, -1 on error
 */
int setPodMode(pod_mode_t new_state);

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
pod_mode_t getPodMode(void);

/**
 * @brief Gets a pointer to the pod state structure
 *
 * The pod state struct contains all the current state information for the pod
 * as well as mutexes for locking the values for reading and writing
 *
 * @return the current pod state as of calling
 */
pod_state_t * getPodState(void);

/**
 * Intiializes the pod's pod_state_t returned by getPodState()
 */
void initializePodState(void);

/**
 * Helper method to read value from pod_state
 */
int32_t getPodField(pod_value_t *pod_field);

/**
 * Helper method to change a value from pod_state
 */
void setPodField(pod_value_t *pod_field, int32_t newValue);

/**
 * Get the current time of the pod in microseconds
 *
 * TODO: Make this function return nanosecond precision
 * TODO: Make this function return a uint64_t
 * TODO: Make this function actually specify the timezone/base of the timestamp
 *
 * @return The current timestamp in microseconds
 */
uint64_t getTime(void);

void logDump(pod_state_t * state);

void podInterruptPanic(int subsystem, char *file, int line, char *notes, ...);

#endif
