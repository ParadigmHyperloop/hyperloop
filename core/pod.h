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

#include "cdefs.h"
#include "config.h"
#include <pthread.h>
#include <sys/queue.h>

#ifndef OPENLOOP_POD_H
#define OPENLOOP_POD_H

#define N_POD_STATES 8

typedef enum {
  Boot = 0,      // initializing systems, establishing network connections, ect
  Ready = 1,     // idle, stationary, ready for push
  Pushing = 2,   // pusher engaged,
  Coasting = 3,  // pusher disengaged, just coasting
  Braking = 4,   // normal braking mode
  Emergency = 5, // emergency braking
  Shutdown = 6,  // pod stationary and in a safe state
  _nil = 7       // NULL STATE, not a valid state, used to terminate arrays
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
  pthread_t core_thread;
  pthread_t logging_thread;
  pthread_t cmd_thread;

  // Current Overall Pod Mode (Goal of the System)
  pod_mode_t mode;
  pthread_rwlock_t mode_mutex;

  // Holds the pod in a boot state until set to 1 by an operator
  pod_value_t ready;

  int tmp_skates;
  int tmp_brakes;
  int tmp_ebrakes;

  sem_t *boot_sem;

  uint64_t start;
  bool initialized;
} pod_state_t;

typedef enum { Message = 1, Telemetry = 2 } log_type_t;

typedef struct {
  char name[64];
  uint32_t value;
} log_data_t;

typedef struct log {
  log_type_t type;
  union {
    char message[MAX_LOG_LINE];
    log_data_t data;
  } content;
  STAILQ_ENTRY(log) entries;
} log_t;

/**
 * Sends the given message to all logging destinations
 */
int podLog(char *fmt, ...);

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
int setPodMode(pod_mode_t new_state, char *reason, ...);

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
pod_state_t *getPodState(void);

/**
 * Intiializes the pod's pod_state_t returned by getPodState()
 */
int initializePodState(void);

/**
 * Helper method to read value from pod_state
 */
int32_t getPodField(pod_value_t *pod_field);
float getPodField_f(pod_value_t *pod_field);

/**
 * Helper method to change a value from pod_state
 */
void setPodField(pod_value_t *pod_field, int32_t newValue);
void setPodField_f(pod_value_t *pod_field, float newValue);

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

void logDump(pod_state_t *state);

void podInterruptPanic(int subsystem, char *file, int line, char *notes, ...);

/**
 * Open the serial line to the IMU
 *
 * Returns the fd of the IMU (also stored in global imuFd var) or -1 on fail
 */
int imuConnect(void);

void pod_exit(int code);
#endif
