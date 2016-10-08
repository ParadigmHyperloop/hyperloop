#include "pod.h"

long maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
long minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(long distance) {
    if(distance > maxSafeDistanceAwayFromRail || distance < minSafeDistanceAwayFromRail) {
        //CHANGE ME! Change state to Emergency!
    }
}

long read_lateral_sensor(int sensorno) {
  //CHANGE ME! Read in value assume units in mm
  return 0;
}

void *lateralMain(void *arg) {
    long leftLateralDistance = 0;
    long rightLateralDistance = 0;
    while(1) {
        leftLateralDistance = read_lateral_sensor(0); //CHANGE ME! use proper sensor ids
        rightLateralDistance = read_lateral_sensor(1); //CHANGE ME! use proper sensor ids
        checkTolerance(leftLateralDistance);
        checkTolerance(rightLateralDistance);
    }
}
