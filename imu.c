#include "pod.h"

long maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
long standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
long maximumSafeDistanceBeforeBraking = 125;

#define PRIMARY_BRAKING_ACCEL_X_MIN -2.0
#define PRIMARY_BRAKING_ACCEL_X_MAX -1.0

// TODO: I'm assuming you can get all 6 values in one datagram
typedef struct {
  // 32-Bit
  unsigned long x;
  unsigned long y;
  unsigned long z;
  unsigned long wx;
  unsigned long wy;
  unsigned long wz;
} imu_datagram_t;

long acceleration = 0; //In m/s^2
long forwardVelocity = 0; //In m/s
long totalDistanceTraveled = 0; //In m
long lastCheckTime = 0; //Should be the method from main to get the current system time, assume milliseconds.
long timeSinceLast = 0; //In milliseconds

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (acceleration <= 0) {
        setPodMode(Coasting);
    }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coastingChecks() {
    if (totalDistanceTraveled > maximumSafeDistanceBeforeBraking || forwardVelocity > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (totalDistanceTraveled > standardDistanceBeforeBraking) {
        setPodMode(Braking);
    }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void brakingChecks() {
    if (outside(PRIMARY_BRAKING_ACCEL_X_MIN, acceleration, PRIMARY_BRAKING_ACCEL_X_MAX)) {
        setPodMode(Emergency);
    }

    if (forwardVelocity <= 0) {
        setPodMode(Shutdown);
    }
}


void * imuMain(void *arg) {
    debug("[imuMain] Thread Start");

    while (getPodState()->mode != Shutdown) {
        if (lastCheckTime == 0) {
          lastCheckTime = getTime();
        }

        long currentCheckTime = getTime(); //Same as above, assume milliseconds
        timeSinceLast = lastCheckTime - currentCheckTime;
        lastCheckTime = currentCheckTime;

        acceleration = 0; //CHANGE ME!!! Get acceleration from Sensor assume m/s^2. Assume long.
        forwardVelocity =
        totalDistanceTraveled += velocityToDistance();

        pod_mode_t podState = getPodMode(); //CHANGE ME!!! Should read from the pthread.

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

    return NULL;
}
