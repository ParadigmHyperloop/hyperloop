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
  pthread_mutex_t mutex;
} pod_value_t;

/**
 * Defines the master state of the pod
 */
typedef struct pod_state {
  struct pod_value accel_x;
  struct pod_value accel_y;
  struct pod_value accel_z;

  struct pod_value velocity_x;
  struct pod_value velocity_z;
  struct pod_value velocity_y;

  struct pod_value position_x;
  struct pod_value position_y;
  struct pod_value position_z;

  struct pod_value lateral_left;
  struct pod_value lateral_right;

  struct pod_value skate_left_z;
  struct pod_value skate_right_z;

  struct pod_value photoelectric_r; // TODO: No Idea
  struct pod_value photoelectric_g; // TODO: No Idea
  struct pod_value photoelectric_b; // TODO: No Idea

  struct pod_value skate_solonoids[N_SKATE_SOLONOIDS];
  struct pod_value ebrake_solonoids[N_EBRAKE_SOLONOIDS];
  struct pod_value wheel_solonoids[N_WHEEL_SOLONOIDS];
  struct pod_value lateral_solonoids[N_LATERAL_SOLONOIDS];


  pod_mode_t mode;
} pod_state_t;


/**
 * @brief Set the new state of the pod's control algorithms.  Setting the state
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
int setPodState(pod_mode_t new_state);


/**
 * @brief Set the new state of the pod's control algorithms.  Setting the state
 *
 * The pod state generally defines the way each of the separate modules
 * (braking, skates, ect) make decisions.  For example, each module may check
 * the pod state on each loop, if the the imu module changes the state to
 * braking. The IMU will also continue it's loop and if it notices that
 * decelleration has not occured, then it will set the pod state to emergency.
 * At this point, the braking thread will loop to this change and apply
 * emergency braking
 *
 * @return the current pod state as of calling
 */
pod_mode_t getPodState(void);

/**
 * Get the current time of the pod in microseconds
 *
 * TODO: Make this function return nanosecond precision
 * TODO: Make this function return a uint64_t
 * TODO: Make this function actually specify the timezone/base of the timestamp
 *
 * @return The current timestamp in microseconds
 */
long getTime(void);

#endif
