#include "pod.h"

long maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
long standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
long maximumSafeDistanceBeforeBraking = 125;

long lastCheckTime = 0; //Should be the method from main to get the current system time, assume milliseconds.
long timeSinceLast = 0; //In milliseconds

long accelToVelocity(pod_state_t *podState) {
    return (getPodField(&(podState.accel_x)) * timeSinceLast / 1000);
}

long velocityToDistance(pod_state_t *podState) {
    return (getPodField(&(podState.velocity_x)) * timeSinceLast / 1000);
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
    
    pod_state_t *podState = getPodState();
    pod_mode_t podMode = getPodMode();

    while (getPodMode() != Shutdown) {
        // TODO: initializing hackery
        if (lastCheckTime == 0) {
          lastCheckTime = getTime();
        }

        long currentCheckTime = getTime(); //Same as above, assume milliseconds
        timeSinceLast = lastCheckTime - currentCheckTime;
        lastCheckTime = currentCheckTime;

        setPodField(&(podState.accel_x), 0); //CHANGE ME!!! Get acceleration from Sensor assume m/s^2. Assume long.
        setPodField(&(podState.velocity_x), accelToVelocity(podState));
        setPodField(&(podState.position_x), getPodField(&(podState.position_x)) + velocityToDistance(podState))

        podMode = getPodMode();

        switch (podMode) {
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
