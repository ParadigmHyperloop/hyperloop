#include "pod.h"

long readDistanceSensor(int sensorno) {
    //TODO: Actually read in distance value
    return 0;
}

void distanceCheck(pod_state_t *podState) {
    //Are there any checks we want to make with the photo-electric sensor?
}

void *distanceMain(void *arg) {
    debug("[photoelectricMain] Thread Start");
    pod_state_t *podState = getPodState();
    while(1) {
        setPodField(&(podState->skate_left_z), readDistanceSensor(0));
        setPodField(&(podState->skate_right_z), readDistanceSensor(0));
        
        distanceCheck(podState);
        
        usleep(DISTANCE_THREAD_SLEEP);
    }
}
