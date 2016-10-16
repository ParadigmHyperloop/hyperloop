#include "pod.h"
#include "pod-helpers.h"

int imuRead(pod_state_t * state);
int lateralRead(pod_state_t * state);
int heightRead(pod_state_t * state);
int brakingRead(pod_state_t * state);

int32_t maximumSafeForwardVelocity = 25; // TODO: CHANGE ME! ARBITRARY!
int32_t standardDistanceBeforeBraking = 75; // TODO: CHANGE ME! ARBITRARY!
int32_t maximumSafeDistanceBeforeBraking = 125;

/**
 * Checks to be performed when the pod's state is Boot
 */
void bootChecks(pod_state_t * state) {
  if (getPodField(&(state->ready)) == 1) {

    // TODO: Other Pre-flight Checks that are not Human checked

    setPodMode(Ready, "Pod's Ready bit has been set");
  } else {
    info("Pod state is Boot, waiting for operator...");
  }
}

/**
 * Checks to be performed when the pod's state is Boot
 */
void readyChecks(pod_state_t * state) {
  if (getPodField(&(state->accel_x)) > 0) {
    setPodMode(Pushing, "Detecting Positive Acceleration");
  }
}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergencyChecks(pod_state_t * state) {
  if (podIsStopped(state) && emergencyBrakesEngaged(state) && primaryBrakesEngaged(state)) {
    setPodMode(Shutdown, "Pod has been determined to be in a safe state");
  }
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushingChecks(pod_state_t * state) {
    if (getPodField(&(state->position_x)) > maximumSafeDistanceBeforeBraking) {
        setPodMode(Emergency, "Pod Position is > max travel before braking");
    }
    else if (getPodField(&(state->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency, "Pod is going too fast");
    }
    else if (getPodField(&(state->accel_x)) <= 0) {
        setPodMode(Coasting, "Pod has negative acceleration in the X dir");
    }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coastingChecks(pod_state_t * state) {
    if (getPodField(&(state->position_x)) > maximumSafeDistanceBeforeBraking || getPodField(&(state->velocity_x)) > maximumSafeForwardVelocity) {
        setPodMode(Emergency, "Pod has travelled too far");
    }
    else if (getPodField(&(state->position_x)) > standardDistanceBeforeBraking) {
        setPodMode(Braking, "Pod has entered braking range of travel");
    }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void brakingChecks(pod_state_t * state) {
    if (outside(PRIMARY_BRAKING_ACCEL_X_MIN, getPodField(&(state->accel_x)), PRIMARY_BRAKING_ACCEL_X_MAX)) {
        setPodMode(Emergency, "Pod acceleration is NOT nominal");
    }
    else if (podIsStopped(state)) {
        setPodMode(Shutdown, "Pod has stopped");
    }
}

void heightCheck(pod_state_t *state) {
    // TODO: Make these checks bounded by min and max values
    bool ok = (getPodField(&(state->skate_rear_left_z)) > 0) &&
              (getPodField(&(state->skate_rear_right_z)) > 0) &&
              (getPodField(&(state->skate_front_left_z)) > 0) &&
              (getPodField(&(state->skate_front_right_z)) > 0);

    if (!ok) {
      setPodMode(Emergency, "A height sensor is returning 0");
    }
}

void lateralCheck(pod_state_t *state) {

    int errors = 0;
    if (outside(LATERAL_MIN, getPodField(&(state->lateral_front_left)), LATERAL_MAX)) {
      errors |= 0x1;
    }
    if (outside(LATERAL_MIN, getPodField(&(state->lateral_front_right)), LATERAL_MAX)) {
      errors |= 0x2;
    }
    if (outside(LATERAL_MIN, getPodField(&(state->lateral_rear_left)), LATERAL_MAX)) {
      errors |= 0x4;
    }
    if (outside(LATERAL_MIN, getPodField(&(state->lateral_rear_right)), LATERAL_MAX)) {
      errors |= 0x8;
    }

    if (errors) {
        error("lateral error mask: %X", errors);
        setPodMode(Emergency, "lateral sensor(s) is out of bounds");
    }
}

void setSkates(int value) {
  // TODO: Implement Me
  getPodState()->tmp_skates = value;
}

void setBrakes(int value) {
  // TODO: Implement Me
  getPodState()->tmp_brakes = value;
}

void adjustBrakes(pod_state_t * state) {
  switch (getPodMode()) {
    case Ready:
    case Pushing:
    case Coasting:
      setBrakes(0);
      break;
    case Boot:
    case Emergency:
    case Shutdown:
    case Braking:
      setBrakes(1);
      break;
    default:
      panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a brake decsion");
  }
}

void adjustSkates(pod_state_t * state) {
    // Skates are completely controlled by pod state, therefore we can just
    // switch over them

    switch (getPodMode()) {
      case Ready:
      case Pushing:
      case Coasting:
        setSkates(1);
        break;
      case Boot:
      case Emergency:
      case Shutdown:
      case Braking:
        setSkates(0);
        break;
      default:
        panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a skate decsion");
    }
}

/**
 * The Core Run Loop
 */
void * coreMain(void * arg) {

  // TODO: Implement pinReset();

  imuConnect();
  pod_state_t * state = getPodState();

  pod_mode_t mode;


  while ((mode = getPodMode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Read new information from sensors
    // --------------------------------------------
    if (imuRead(state) < 0) { DECLARE_EMERGENCY("IMU READ FAILED"); }
    if (heightRead(state) < 0) { DECLARE_EMERGENCY("HEIGHT READ FAILED"); }
    if (lateralRead(state) < 0) { DECLARE_EMERGENCY("LATERAL READ FAILED"); }


    // -------------------------------------------
    // SECTION: State Machine to determine actions
    // -------------------------------------------

    // General Checks (Going too fast, going too high)
    heightCheck(state);
    lateralCheck(state);

    // Mode Specific Checks
    switch (getPodMode()) {
        case Boot:
          bootChecks(state);
          break;
        case Ready:
          readyChecks(state);
          break;
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

    // -------------------------------------------
    // SECTION: Change the control surfaces
    // -------------------------------------------

    adjustSkates(state);
    adjustBrakes(state);

    // --------------------------------------------
    // Yield to other threads
    // --------------------------------------------
    usleep(CORE_THREAD_SLEEP);
  }

  return NULL;
}
