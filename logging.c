#include "pod.h"

#define LOGGING_INTERVAL 500000 // Half Second
void* loggingMain(void *arg) {

  while (1) {
    debug("Logging System -> Dumping State");
    pod_state_t * state = getPodState();

    // TODO: Use the freaking Mutexes
    debug("acl: x: %ld, y: %ld, z: %ld",  state->accel_x.value,
            state->accel_y.value,
            state->accel_z.value);


    debug("vel: x: %ld, y: %ld, z: %ld",  state->velocity_x.value,
            state->velocity_y.value,
            state->velocity_z.value);


    debug("pos: x: %ld, y: %ld, z: %ld",  state->position_x.value,
            state->position_y.value,
            state->position_z.value);

    usleep(LOGGING_INTERVAL);
  }
}
