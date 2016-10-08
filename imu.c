#include "pod.h"

long maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
long standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
long maximumSafeDistanceBeforeBraking = 125;

long lastCheckTime = 0; //Should be the method from main to get the current system time, assume milliseconds.
long timeSinceLast = 0; //In milliseconds

long accelToVelocity(pod_state_t *podState) {
    return (getPodField(&(podState->accel_x)) * timeSinceLast / 1000);
}

long velocityToDistance(pod_state_t *podState) {
    return (getPodField(&(podState->velocity_x)) * timeSinceLast / 1000);
}

void pushingChecks(pod_state_t *podState) {
    if (getPodField(&(podState->position_x)) > maximumSafeDistanceBeforeBraking || getPodField(&(podState->velocity_x)) > maximumSafeForwardVelocity) {
        //CHANGE ME!!! Make state Emergency
    }
    else if (getPodField(&(podState->accel_x)) <= 0) {
        //CHANGE ME!!! Make state Coasting
    }
}

void coastingChecks(pod_state_t *podState) {
    if (getPodField(&(podState->position_x)) > maximumSafeDistanceBeforeBraking || getPodField(&(podState->velocity_x)) > maximumSafeForwardVelocity) {
        //CHANGE ME!!! Make state Emergency
    }
    else if (getPodField(&(podState->position_x)) > standardDistanceBeforeBraking) {
        //CHANGE ME!! Make state Braking
    }
}

void brakingChecks(pod_state_t *podState) {
    //QUESTION: Are there emergency state criteria for Braking state?
    if (getPodField(&(podState->velocity_x)) <= 0) {
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

        setPodField(&(podState->accel_x), 0); //CHANGE ME!!! Get acceleration from Sensor assume m/s^2. Assume long.
        setPodField(&(podState->velocity_x), accelToVelocity(podState));
        setPodField(&(podState->position_x), getPodField(&(podState->position_x)) + velocityToDistance(podState));

        podMode = getPodMode(); //CHANGE ME!!! Should read from the pthread.

        switch (podMode) {
            case Pushing :
                pushingChecks(podState);
                break;
            case Coasting :
                coastingChecks(podState);
                break;
            case Braking :
                brakingChecks(podState);
                break;
            default :
                break;
        }

        usleep(IMU_THREAD_SLEEP);
    }

    return NULL;
}
