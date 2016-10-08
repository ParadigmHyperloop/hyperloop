#include "pod.h"

long maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
long minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(long distance) {
    if(distance > maxSafeDistanceAwayFromRail || distance < minSafeDistanceAwayFromRail) {
        //CHANGE ME! Change state to Emergency!
    }
}

long readLateralSensor(int sensorno) {
  //CHANGE ME! Read in value assume units in mm
  return 0;
}

void *lateralMain(void *arg) {
    debug("[lateralMain] Thread Start");
    long leftLateralDistance = 0;
    long rightLateralDistance = 0;
    while(1) {
        leftLateralDistance = readLateralSensor(0); //CHANGE ME! use proper sensor ids
        rightLateralDistance = readLateralSensor(1); //CHANGE ME! use proper sensor ids
        checkTolerance(leftLateralDistance);
        checkTolerance(rightLateralDistance);

        usleep(LATERAL_THREAD_SLEEP);
    }
}
