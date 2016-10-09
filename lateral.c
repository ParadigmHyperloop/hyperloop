#include "pod.h"

uint32_t maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
uint32_t minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(uint32_t distance) {
    if(distance > maxSafeDistanceAwayFromRail || distance < minSafeDistanceAwayFromRail) {
        setPodMode(Emergency);
    }
}

uint32_t readLateralSensor(uint8_t sensorno) {
  //CHANGE ME! Read in value assume units in mm
  return 6;
}

void *lateralMain(void *arg) {
    debug("[lateralMain] Thread Start");
    pod_state_t *podState = getPodState();
    while(1) {
        setPodField(&(podState->lateral_left), readLateralSensor(0));
        setPodField(&(podState->lateral_right), readLateralSensor(1));
        checkTolerance(getPodField(&(podState->lateral_left)));
        checkTolerance(getPodField(&(podState->lateral_left)));

        usleep(LATERAL_THREAD_SLEEP);
    }
}
