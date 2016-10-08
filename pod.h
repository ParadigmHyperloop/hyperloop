#include "cdefs.h"

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
int set_pod_state(pod_state_t new_state);


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
pod_state_t get_pod_state(void);

#endif
