#include "pod.h"

/**
 * WARNING: Do Not Directly Access this struct, use getPodState() instead to
 * get a pointer to the pod state.
 */
extern pod_state_t __state;

void setPriority(pthread_t task, int priority) {
  struct sched_param sp;
  sp.sched_priority = priority;
  if (pthread_setschedparam(task, SCHED_RR, &sp)) {
    fprintf(stderr, "WARNING: failed to set thread"
                    "to real-time priority\n");
  }
}

void *imuMain(void *arg);
void *distanceMain(void *arg);
void *brakingMain(void *arg);
void *lateralMain(void *arg);
void *loggingMain(void *arg);
void *commandMain(void *arg);

void digitalWrite(int pin, int val) { }
/**
 * Panic Signal Handler.  This is only called if the shit has hit the fan
 * This function fires whenever the controller looses complete control in itself
 *
 * The controller sets the EBRAKE pins to LOW (engage) and then kills all it's
 * threads.  This is done to prevent threads from toggling the Ebrake pins OFF
 * for whatever reason.
 *
 * This is a pretty low level function because it is attempting to cut out the
 * entire controller logic and just make the pod safe
 */
void signal_handler(int sig) {
  __state.mode = Emergency;

  // Manually make the pod safe
  int ebrake_pins[] = EBRAKE_PINS;

  // Set all the ebrake pins to 0
  for (int i = 0; i < sizeof(ebrake_pins)/sizeof(int); i++) {
    fprintf(stderr, "[PANIC] Forcing Pin %d => 0", ebrake_pins[i]);
    digitalWrite(ebrake_pins[i], 0);
  }

  exit(EXIT_FAILURE);
}

void sigpipe_handler(int sig) {
  warn("SIGPIPE Recieved");
}

int main() {
  initializePodState();
  pod_state_t * state = getPodState();

  signal(POD_SIGPANIC, signal_handler);

  signal(SIGPIPE, sigpipe_handler);

  pthread_create(&(state->imu_thread), NULL, imuMain, NULL);
  pthread_create(&(state->distance_thread), NULL, distanceMain, NULL);
  pthread_create(&(state->braking_thread), NULL, brakingMain, NULL);
  pthread_create(&(state->lateral_thread), NULL, lateralMain, NULL);
  pthread_create(&(state->logging_thread), NULL, loggingMain, NULL);
  pthread_create(&(state->cmd_thread), NULL, commandMain, NULL);

  // we're using the built-in linux Round Roboin scheduling
  // priorities are 1-99, higher is more important
  // important not: this is not hard real-time
  setPriority(state->imu_thread, 30);
  setPriority(state->distance_thread, 25);
  setPriority(state->braking_thread, 30);
  setPriority(state->lateral_thread, 25);
  setPriority(state->logging_thread, 15);
  setPriority(state->cmd_thread, 15);

  pthread_join(state->imu_thread, NULL);
  pthread_join(state->distance_thread, NULL);
  pthread_join(state->braking_thread, NULL);
  pthread_join(state->lateral_thread, NULL);
  pthread_join(state->logging_thread, NULL);
  pthread_join(state->cmd_thread, NULL);

  return 0;
}
