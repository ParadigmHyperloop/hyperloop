#include "pod.h"

uint32_t readDistanceSensor(int sensorno) {
    //TODO: Actually read in distance value
    return 0;
}

void distanceCheck(pod_state_t *podState) {
    //Are there any checks we want to make with the photo-electric sensor?
}

void *distanceMain(void *arg) {
    debug("[photoelectricMain] Thread Start");
    pod_state_t *podState = getPodState();
    while (getPodMode() != Shutdown) {
        setPodField(&(podState->skate_front_left_z), readDistanceSensor(0));
        setPodField(&(podState->skate_front_right_z), readDistanceSensor(1));
        setPodField(&(podState->skate_rear_left_z), readDistanceSensor(2));
        setPodField(&(podState->skate_rear_right_z), readDistanceSensor(3));

        distanceCheck(podState);

        usleep(DISTANCE_THREAD_SLEEP);
    }
    return NULL;
}
