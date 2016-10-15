#include "pod.h"
#include "pod-helpers.h"

int32_t maximumSafeForwardVelocity = 25; //CHANGE ME! ARBITRARY!
int32_t standardDistanceBeforeBraking = 75; //CHANGE ME! ARBITRARY!
int32_t maximumSafeDistanceBeforeBraking = 125;

// TODO: I'm assuming you can get all 6 values in one datagram
typedef struct {
  // 32-Bit: All units are based in milimeters (1/1,000 m)
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t wx;
  int32_t wy;
  int32_t wz;
} imu_datagram_t;

imu_datagram_t readIMUDatagram(uint64_t t) {
  // TODO: Implement proper reading from the IMU serial data feed
  static uint64_t i = 0;
  int32_t ax = 1000L; // 1 m/s/s

  if (t > 60ULL * 1000ULL * 1000ULL) {
    ax = -100;
  }

  printf("%llu\n", i);
  i++;
  switch ((i << 4) & 0x1) {
    case 0:
      return (imu_datagram_t){ ax, 8L, 0L, 0L, 0L, 0L };
    case 1:
      return (imu_datagram_t){ ax, -8L, 0L, 0L, 0L, 0L };
    default:
      exit(101);
  }

}

void emergencyChecks(pod_state_t * state) {
  if (podIsStopped(state) && emergencyBrakesEngaged(state) && primaryBrakesEngaged(state)) {
    setPodMode(Shutdown);
  }
}
/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushingChecks(pod_state_t * state) {
    if (getPodField(&(state->position_x)) > maximumSafeDistanceBeforeBraking) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(state->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(state->accel_x)) <= 0) {
        setPodMode(Coasting);
    }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coastingChecks(pod_state_t * state) {
    if (getPodField(&(state->position_x)) > maximumSafeDistanceBeforeBraking || getPodField(&(state->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(state->position_x)) > standardDistanceBeforeBraking) {
        setPodMode(Braking);
    }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void brakingChecks(pod_state_t * state) {
    if (outside(PRIMARY_BRAKING_ACCEL_X_MIN, getPodField(&(state->accel_x)), PRIMARY_BRAKING_ACCEL_X_MAX)) {
        setPodMode(Emergency);
    }
    else if (getPodField(&(state->velocity_x)) <= 0) {
        setPodMode(Shutdown);
    }
}

void * imuMain(void *arg) {

    static uint64_t start_time = 0;

    start_time = getTime();

    debug("[imuMain] Thread Start");

    pod_state_t * state = getPodState();
    pod_mode_t podMode = getPodMode();

    uint64_t lastCheckTime = getTime();

    while (getPodMode() != Shutdown) {
        uint64_t currentCheckTime = getTime(); //Same as above, assume milliseconds
        if (currentCheckTime - start_time > 9000000ULL) { sleep(120); exit(121); }
        imu_datagram_t imu_reading = readIMUDatagram(currentCheckTime - start_time);

        assert((currentCheckTime - lastCheckTime) < INT32_MAX);

        int32_t t_usec = (int32_t) (currentCheckTime - lastCheckTime);
        lastCheckTime = currentCheckTime;

        int32_t position = getPodField(&(state->position_x));
        int32_t velocity = getPodField(&(state->velocity_x));
        int32_t acceleration = getPodField(&(state->accel_x));

        // Calculate the new_velocity (oldv + (olda + newa) / 2)

        int32_t dv = ((t_usec * ((acceleration + imu_reading.x) / 2)) / 1000000LL);

        printf("%d\n", dv);
        assertUInt32Addition(velocity, dv);
        int32_t new_velocity = (velocity + dv);

        int32_t dx = ((t_usec * ((velocity + new_velocity) / 2)) / 1000000LL);
        int32_t new_position = (position + dx);

        setPodField(&(state->position_x), new_position);
        setPodField(&(state->velocity_x), new_velocity);
        setPodField(&(state->accel_x), imu_reading.x);

        logDump(state);

        podMode = getPodMode();

        switch (podMode) {
            case Pushing:
                pushingChecks(state);
                break;
            case Coasting:
                coastingChecks(state);
                break;
            case Braking:
                brakingChecks(state);
                break;
            case Emergency:
                emergencyChecks(state);
            default:
                break;
        }


        usleep(IMU_THREAD_SLEEP * 1000);
    }

    return NULL;
}
