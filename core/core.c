/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#include "pod.h"
#include "pod-helpers.h"

int imuRead(pod_state_t *state);
int lateralRead(pod_state_t *state);
int skateRead(pod_state_t *state);
int brakingRead(pod_state_t *state);

float maximumSafeForwardVelocity = 25.0;    // TODO: CHANGE ME! ARBITRARY!
float standardDistanceBeforeBraking = 75.0; // TODO: CHANGE ME! ARBITRARY!
float maximumSafeDistanceBeforeBraking = 125.0;

/**
 * Checks to be performed when the pod's state is Boot
 */
void bootChecks(pod_state_t *state) {
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
void readyChecks(pod_state_t *state) {
  if (getPodField_f(&(state->accel_x)) > PUSHING_MIN_ACCEL) {
    setPodMode(Pushing, "Detecting Positive Acceleration");
  }
}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergencyChecks(pod_state_t *state) {
  if (podIsStopped(state) && emergencyBrakesEngaged(state) &&
      primaryBrakesEngaged(state)) {
    setPodMode(Shutdown, "Pod has been determined to be in a safe state");
  }
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushingChecks(pod_state_t *state) {
  if (getPodField_f(&(state->position_x)) > maximumSafeDistanceBeforeBraking) {
    setPodMode(Emergency, "Pod Position is > max travel before braking");
  } else if (getPodField_f(&(state->velocity_x)) > maximumSafeForwardVelocity) {
    setPodMode(Emergency, "Pod is going too fast");
  } else if (getPodField_f(&(state->accel_x)) <= COASTING_MIN_ACCEL_TRIGGER) {
    setPodMode(Coasting, "Pod has negative acceleration in the X dir");
  }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coastingChecks(pod_state_t *state) {
  if (getPodField_f(&(state->position_x)) > maximumSafeDistanceBeforeBraking ||
      getPodField_f(&(state->velocity_x)) > maximumSafeForwardVelocity) {
    setPodMode(Emergency, "Pod has travelled too far");
  } else if (getPodField_f(&(state->position_x)) >
             standardDistanceBeforeBraking) {
    setPodMode(Braking, "Pod has entered braking range of travel");
  }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void brakingChecks(pod_state_t *state) {
  // TODO: This is an issue, Engineers need the look at this
  //       Do Not Ship without this baking algorithm reviewed
  if (PRIMARY_BRAKING_ACCEL_X_MAX > getPodField_f(&(state->accel_x))) {
    setPodMode(Emergency, "Pod decelleration is too high");
  } else if (PRIMARY_BRAKING_ACCEL_X_MIN < getPodField_f(&(state->accel_x))) {
    float ax = getPodField_f(&(state->accel_x));
    float vx = getPodField_f(&(state->velocity_x));

    if (podIsStopped(state)) {
      setPodMode(Shutdown, "Pod has stopped");
    } else if (ax > -vx) { // TODO: this calculation is BS.
      setPodMode(Emergency, "Pod decelleration is too low");
    }
  }
}

void skateCheck(pod_state_t *state) {
  // TODO: Make these checks bounded by min and max values
  bool ok = (getPodField(&(state->skate_rear_left_z)) > 0) &&
            (getPodField(&(state->skate_rear_right_z)) > 0) &&
            (getPodField(&(state->skate_front_left_z)) > 0) &&
            (getPodField(&(state->skate_front_right_z)) > 0);

  if (!ok) {
    setPodMode(Emergency, "A height sensor is returning 0");
  }

  int i;
  for (i = 0; i < N_SKATE_THERMOCOUPLES; i++) {
    int32_t temp = getPodField(&(state->skate_thermocouples[i]));

    if (temp < MIN_REGULATOR_THERMOCOUPLE_TEMP) {
      setPodMode(Emergency, "Thermocouple %d for skates is too low");
    }
  }
}

void lateralCheck(pod_state_t *state) {

  int errors = 0;
  if (outside(LATERAL_MIN, getPodField(&(state->lateral_front_left)),
              LATERAL_MAX)) {
    errors |= 0x1;
  }
  if (outside(LATERAL_MIN, getPodField(&(state->lateral_front_right)),
              LATERAL_MAX)) {
    errors |= 0x2;
  }
  if (outside(LATERAL_MIN, getPodField(&(state->lateral_rear_left)),
              LATERAL_MAX)) {
    errors |= 0x4;
  }
  if (outside(LATERAL_MIN, getPodField(&(state->lateral_rear_right)),
              LATERAL_MAX)) {
    errors |= 0x8;
  }

  if (errors) {
    error("lateral error mask: %X", errors);
    setPodMode(Emergency, "lateral sensor(s) is out of bounds");
  }
}

int setSkates(int no, int val, bool override) {
  // TODO: Implement Me
  pod_state_t * state = getPodState();
  if (isManual(SKATE_OVERRIDE_ALL) && !override && state->tmp_skates != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_skates = val;
  return 0;
}

int setBrakes(int no, int val, bool override) {
  // TODO: Implement Me
  pod_state_t * state = getPodState();
  uint64_t skate_override[] = SKATE_OVERRIDE_LIST;
  if (isManual(skate_override[no]) && !override && state->tmp_brakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_brakes = val;
  return 0;
}

int setEBrakes(int no, int val, bool override) {
  // TODO: Implement actually and also implement locking
  pod_state_t * state = getPodState();
  uint64_t ebrake_override[] = EBRAKE_OVERRIDE_LIST;

  if (isManual(ebrake_override[no]) && !override && state->tmp_ebrakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_ebrakes = val;
  return 0;
}

void adjustBrakes(pod_state_t *state) {
  int i;
  switch (getPodMode()) {
  case Ready:
  case Pushing:
  case Coasting:
    for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
      setBrakes(i, 0, false);
    }
    break;
  case Boot:
  case Shutdown:
  case Braking:
    for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
      setBrakes(i, 1, false);
    }
    break;
  case Emergency:
    if (getPodField(&(state->accel_x)) <= A_ERR_X) {
      for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
        setBrakes(i, 1, false);
      }
      for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
        setEBrakes(i, 0, false);
      }
    } else {
      error("==== Emergency Emergency Emergency ====");
      error("State is Emergency but not applying any brakes because accel x is > 0.0");
      error("==== Emergency Emergency Emergency ====");
    }
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a brake decsion");
  }
}

void adjustSkates(pod_state_t *state) {
  // Skates are completely controlled by pod state, therefore we can just
  // switch over them
  int i;
  switch (getPodMode()) {
  case Ready:
  case Pushing:
  case Coasting:
    for (i=0; i<N_SKATE_SOLONOIDS; i++) {
      setSkates(i, 1, false);
    }
    break;
  case Boot:
  case Emergency:
  case Shutdown:
  case Braking:
    for (i=0; i<N_SKATE_SOLONOIDS; i++) {
      setSkates(i, 1, false);
    }
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a skate decsion");
  }
}

/**
 * The Core Run Loop
 */
void *coreMain(void *arg) {

  // TODO: Implement pinReset();

  imuConnect();
  pod_state_t *state = getPodState();

  pod_mode_t mode;

  while ((mode = getPodMode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Read new information from sensors
    // --------------------------------------------
    if (imuRead(state) < 0) {
      DECLARE_EMERGENCY("IMU READ FAILED");
    }
    if (skateRead(state) < 0) {
      DECLARE_EMERGENCY("SKATE READ FAILED");
    }
    if (lateralRead(state) < 0) {
      DECLARE_EMERGENCY("LATERAL READ FAILED");
    }

    // -------------------------------------------
    // SECTION: State Machine to determine actions
    // -------------------------------------------

    // General Checks (Going too fast, going too high)
    skateCheck(state);
    lateralCheck(state);

    if (getPodField_f(&(state->velocity_x)) < -V_ERR_X) {
      setPodMode(Emergency, "Pod rolling backward");
    }

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

    // Handle Skates
    adjustSkates(state);

    // Handle Wheel AND Ebrakes
    adjustBrakes(state);

    // -------------------------------------------
    // SECTION: Telemetry collection
    // -------------------------------------------
    logDump(state);

    // --------------------------------------------
    // Yield to other threads
    // --------------------------------------------
    usleep(CORE_THREAD_SLEEP);
  }

  return NULL;
}
