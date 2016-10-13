#include "pod.h"

uint32_t maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
uint32_t standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
uint32_t maximumSafeDistanceBeforeBraking = 125;

// TODO: I'm assuming you can get all 6 values in one datagram
typedef struct {
  // 32-Bit
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t wx;
  uint32_t wy;
  uint32_t wz;
} imu_datagram_t;

imu_datagram_t readIMUDatagram() {
  // TODO: Implement proper reading from the IMU serial data feed
  return (imu_datagram_t){ 1UL, 0UL, 0UL, 0UL, 0UL, 0UL };
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushingChecks(pod_state_t *podState) {
    if (getPodField(&(podState->position_x)) > maximumSafeDistanceBeforeBraking) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(podState->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(podState->accel_x)) <= 0) {
        setPodMode(Coasting);
    }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coastingChecks(pod_state_t *podState) {
    if (getPodField(&(podState->position_x)) > maximumSafeDistanceBeforeBraking || getPodField(&(podState->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(podState->position_x)) > standardDistanceBeforeBraking) {
        setPodMode(Braking);
    }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void brakingChecks(pod_state_t *podState) {
    if (outside(PRIMARY_BRAKING_ACCEL_X_MIN, getPodField(&(podState->accel_x)), PRIMARY_BRAKING_ACCEL_X_MAX)) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(podState->velocity_x)) <= 0) {
        setPodMode(Shutdown);
    }
}

void * imuMain(void *arg) {
    debug("[imuMain] Thread Start");

    pod_state_t *podState = getPodState();
    pod_mode_t podMode = getPodMode();

    uint64_t lastCheckTime = getTime();

    while (getPodMode() != Shutdown) {

        imu_datagram_t imu_reading = readIMUDatagram();

        uint64_t currentCheckTime = getTime(); //Same as above, assume milliseconds
        uint64_t t = lastCheckTime - currentCheckTime;
        lastCheckTime = currentCheckTime;

        uint32_t position = getPodField(&(podState->position_x));
        uint32_t velocity = getPodField(&(podState->velocity_x));
        uint32_t acceleration = getPodField(&(podState->accel_x));

        // Calculate the new_velocity (oldv + (olda + newa) / 2)
        uint32_t new_velocity = (velocity + (t * ((acceleration + imu_reading.x) / 2)));
        uint32_t new_position = (position + (t * ((new_velocity + imu_reading.x) / 2)));

        setPodField(&(podState->position_x), new_position);
        setPodField(&(podState->velocity_x), new_velocity);
        setPodField(&(podState->accel_x), imu_reading.x);

        podMode = getPodMode();

        switch (podMode) {
            case Pushing:
                pushingChecks(podState);
                break;
            case Coasting:
                coastingChecks(podState);
                break;
            case Braking:
                brakingChecks(podState);
                break;
            default:
                break;
        }

        usleep(IMU_THREAD_SLEEP);
    }

    return NULL;
}
