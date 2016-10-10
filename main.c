#include "pod.h"

// locks sensor data (imu, distance, photoelectric)
pthread_mutex_t sensorDataMutex;
// locks states (pos, vel, quaternions)
pthread_mutex_t statesMutex;
// locks desired positions of plants
pthread_mutex_t plantCommandMutex;
// locks pof phase (testing, pusher, coasting, braking, etc)
pthread_mutex_t podPhaseMutex;
// locks emergency brake and lat control commands
pthread_mutex_t emergencyFlagMutex;


void setPriority(pthread_t task, int priority) {
  struct sched_param sp;
  sp.sched_priority = priority;
  if (pthread_setschedparam(task, SCHED_RR, &sp)) {
    fprintf(stderr, "WARNING: failed to set thread"
                    "to real-time priority\n");
  }
}

void *kalmanMain(void *arg);
void *photoelectricMain(void *arg);
void *imuMain(void *arg);
void *distanceMain(void *arg);
void *brakingMain(void *arg);
void *lateralMain(void *arg);
void *loggingMain(void *arg);
void *commandMain(void *arg);

int main() {
  // TODO: Remove this
  pthread_mutex_init(&sensorDataMutex, NULL);
  pthread_mutex_init(&statesMutex, NULL);
  pthread_mutex_init(&podPhaseMutex, NULL);
  pthread_mutex_init(&plantCommandMutex, NULL);
  pthread_mutex_init(&emergencyFlagMutex, NULL);

  initializePodState();
  pod_state_t * state = getPodState();

  pthread_create(&(state->kalman_thread), NULL, kalmanMain, NULL);
  pthread_create(&(state->photoelectric_thread), NULL, photoelectricMain, NULL);
  pthread_create(&(state->imu_thread), NULL, imuMain, NULL);
  pthread_create(&(state->distance_thread), NULL, distanceMain, NULL);
  pthread_create(&(state->braking_thread), NULL, brakingMain, NULL);
  pthread_create(&(state->lateral_thread), NULL, lateralMain, NULL);
  pthread_create(&(state->logging_thread), NULL, loggingMain, NULL);
  pthread_create(&(state->cmd_thread), NULL, commandMain, NULL);

  // we're using the built-in linux Round Roboin scheduling
  // priorities are 1-99, higher is more important
  // important not: this is not hard real-time
  setPriority(state->kalman_thread, 30);
  setPriority(state->photoelectric_thread, 30);
  setPriority(state->imu_thread, 30);
  setPriority(state->distance_thread, 25);
  setPriority(state->braking_thread, 30);
  setPriority(state->lateral_thread, 25);
  setPriority(state->logging_thread, 15);
  setPriority(state->cmd_thread, 15);

  while (1) {
    debug("Clock Ticked at %lld", getTime());
    usleep(1000000);
    fflush(stdout);
  }

  // TODO: Remove
  pthread_mutex_destroy(&sensorDataMutex);
  pthread_mutex_destroy(&statesMutex);
  pthread_mutex_destroy(&podPhaseMutex);
  pthread_mutex_destroy(&plantCommandMutex);
  pthread_mutex_destroy(&emergencyFlagMutex);

  return 0;
}
