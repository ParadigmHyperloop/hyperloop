#include "cdefs.h"
#include "config.h"
#include <pthread.h>

#ifndef OPENLOOP_POD_H
#define OPENLOOP_POD_H

#define N_POD_STATES 7
typedef enum {
  Boot      = 0, // initializing systems, establishing network connections, ect
  Ready     = 1, // idle, stationary, ready for push
  Pushing   = 2, // pusher engaged,
  Coasting  = 3, // pusher disengaged, just coasting
  Braking   = 4, // normal braking mode
  Shutdown  = 5, // pod stationary and in a safe state
  Emergency = 6, // emergency braking
  _nil      = 7  // NULL STATE, not a valid state, used to terminate arrays
} pod_mode_t;

typedef struct pod_value {
  unsigned long value; // TODO: either make this a union or more structs
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

  pod_value_t lateral_left;
  pod_value_t lateral_right;

  pod_value_t skate_left_z;
  pod_value_t skate_right_z;

  pod_value_t photoelectric_r; // TODO: No Idea
  pod_value_t photoelectric_g; // TODO: No Idea
  pod_value_t photoelectric_b; // TODO: No Idea

  pod_value_t skate_solonoids[N_SKATE_SOLONOIDS];
  pod_value_t ebrake_solonoids[N_EBRAKE_SOLONOIDS];
  pod_value_t wheel_solonoids[N_WHEEL_SOLONOIDS];
  pod_value_t lateral_solonoids[N_LATERAL_SOLONOIDS];

  pthread_t kalman_thread;
  pthread_t photoelectric_thread;
  pthread_t imu_thread;
  pthread_t distance_thread;
  pthread_t braking_thread;
  pthread_t lateral_thread;
  pthread_t logging_thread;

  pod_mode_t mode;
  pthread_rwlock_t mode_mutex;

  bool initialized;
} pod_state_t;

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
 * Get the current time of the pod in microseconds
 *
 * TODO: Make this function return nanosecond precision
 * TODO: Make this function return a uint64_t
 * TODO: Make this function actually specify the timezone/base of the timestamp
 *
 * @return The current timestamp in microseconds
 */
unsigned long long getTime(void);

#endif
