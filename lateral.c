#include "pod.h"

long maxSafeDistanceAwayFromRail = 7; //CHANGE ME!! ARBITRARY In mm
long minSafeDistanceAwayFromRail = 5; //CHANGE ME!! ARBITRARY In mm

void checkTolerance(long distance) {
    if(distance > maxSafeDistanceAwayFromRail || distance < minSafeDistanceAwayFromRail) {
        //CHANGE ME! Change state to Emergency!
    }
}

void *lateralMain(void *arg) {
    long leftLateral;
    long rightLateral;
    while(1) {
        leftLateralDistance = 0; //CHANGE ME! Read in value assume units in mm
        rightLateralDistance = 0; //CHANGE ME! Read in value assume units in mm
        checkTolerance(leftLateralDistance);
        checkTolerance(rightLateralDistance);
    }
}
