#include "pod.h"

long maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
long standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
long maximumSafeDistanceBeforeBraking = 125;

long acceleration = 0; //In m/s^2
long forwardVelocity = 0; //In m/s
long totalDistanceTraveled = 0; //In m
long lastCheckTime = 0; //Should be the method from main to get the current system time, assume milliseconds.
long timeSinceLast = 0; //In milliseconds

long accelToVelocity() {
    return (acceleration * timeSinceLast / 1000);
}

long velocityToDistance() {
    return (forwardVelocity * timeSinceLast / 1000);
}

void pushingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        //CHANGE ME!!! Make state Emergency
    }
    else if (acceleration <= 0) {
        //CHANGE ME!!! Make state Coasting
    }
}

void coastingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        //CHANGE ME!!! Make state Emergency
    }
    else if (totalDistanceTraveled > standardDistanceBeforeBraking) {
        //CHANGE ME!! Make state Braking
    }
}

void brakingChecks() {
    //QUESTION: Are there emergency state criteria for Braking state?
    if (forwardVelocity <= 0) {
        //CHANGE ME!! Make state Shutdown
    }
}


void * imuMain(void *arg) {
    debug("[imuMain] Thread Start");

    while (getPodState()->mode != Shutdown) {
        // TODO: initializing hackery
        if (lastCheckTime == 0) {
          lastCheckTime = getTime();
        }

        long currentCheckTime = getTime(); //Same as above, assume milliseconds
        timeSinceLast = lastCheckTime - currentCheckTime;
        lastCheckTime = currentCheckTime;

        acceleration = 0; //CHANGE ME!!! Get acceleration from Sensor assume m/s^2. Assume long.
        forwardVelocity = accelToVelocity();
        totalDistanceTraveled += velocityToDistance();

        pod_mode_t podState = Ready; //CHANGE ME!!! Should read from the pthread.

        switch (podState) {
            case Pushing :
                pushingChecks();
                break;
            case Coasting :
                coastingChecks();
                break;
            case Braking :
                brakingChecks();
                break;
            default :
                break;
        }

        usleep(IMU_THREAD_SLEEP);
    }
}
