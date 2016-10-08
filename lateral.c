#include "pod.h"

long maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
long minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(long distance) {
    if(distance > maxSafeDistanceAwayFromRail || distance < minSafeDistanceAwayFromRail) {
        setPodMode(Emergency);
    }
}

long readLateralSensor(int sensorno) {
  //CHANGE ME! Read in value assume units in mm
  return 0;
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
