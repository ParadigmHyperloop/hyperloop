#include "pod.h"
/**
 * TODO: Do Not Need to do anything with this anymore
 */

uint32_t maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
uint32_t minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(uint32_t distance) {
    if(outside(minSafeDistanceAwayFromRail, distance, maxSafeDistanceAwayFromRail)) {
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
    while (getPodMode() != Shutdown) {
        setPodField(&(podState->lateral_front_left), readLateralSensor(0));
        setPodField(&(podState->lateral_front_right), readLateralSensor(1));
        setPodField(&(podState->lateral_rear_left), readLateralSensor(0));
        setPodField(&(podState->lateral_rear_right), readLateralSensor(1));

        checkTolerance(getPodField(&(podState->lateral_front_left)));
        checkTolerance(getPodField(&(podState->lateral_front_right)));
        checkTolerance(getPodField(&(podState->lateral_rear_left)));
        checkTolerance(getPodField(&(podState->lateral_rear_right)));

        usleep(LATERAL_THREAD_SLEEP);
    }
    return NULL;
}
