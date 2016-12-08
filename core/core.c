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

int imufd;

int lateralRead(pod_state_t *state);
int skateRead(pod_state_t *state);
int brakingRead(pod_state_t *state);

float maximumSafeForwardVelocity = 25.0;    // TODO: CHANGE ME! ARBITRARY!
float standardDistanceBeforeBraking = 75.0; // TODO: CHANGE ME! ARBITRARY!
float maximumSafeDistanceBeforeBraking = 125.0;

/**
 * Checks to be performed when the pod's state is Boot
 */
void boot_state_checks(pod_state_t *state) {
  if (get_value(&(state->ready)) == 1) {
    // TODO: Other Pre-flight Checks that are not Human checked
    set_pod_mode(Ready, "Pod's Ready bit has been set");
  } else {
    info("Pod state is Boot, waiting for operator...");
  }
}

/**
 * Checks to be performed when the pod's state is Boot
 */
void ready_state_checks(pod_state_t *state) {
  if (get_value_f(&(state->accel_x)) > PUSHING_MIN_ACCEL) {
    set_pod_mode(Pushing, "Detecting Positive Acceleration");
  }
}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergency_state_checks(pod_state_t *state) {
  if (is_pod_stopped(state) && any_emergency_brakes(state) &&
      any_calipers(state)) {
    set_pod_mode(Shutdown, "Pod has been determined to be in a safe state");
  }
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushing_state_checks(pod_state_t *state) {
  if (get_value_f(&(state->position_x)) > maximumSafeDistanceBeforeBraking) {
    set_pod_mode(Emergency, "Pod Position is > max travel before braking");
  } else if (get_value_f(&(state->velocity_x)) > maximumSafeForwardVelocity) {
    set_pod_mode(Emergency, "Pod is going too fast");
  } else if (get_value_f(&(state->accel_x)) <= COASTING_MIN_ACCEL_TRIGGER) {
    set_pod_mode(Coasting, "Pod has negative acceleration in the X dir");
  }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coasting_state_checks(pod_state_t *state) {
  if (get_value_f(&(state->position_x)) > maximumSafeDistanceBeforeBraking ||
      get_value_f(&(state->velocity_x)) > maximumSafeForwardVelocity) {
    set_pod_mode(Emergency, "Pod has travelled too far");
  } else if (get_value_f(&(state->position_x)) >
             standardDistanceBeforeBraking) {
    set_pod_mode(Braking, "Pod has entered braking range of travel");
  }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void braking_state_checks(pod_state_t *state) {
  // TODO: This is an issue, Engineers need the look at this
  //       Do Not Ship without this baking algorithm reviewed
  if (PRIMARY_BRAKING_ACCEL_X_MAX > get_value_f(&(state->accel_x))) {
    set_pod_mode(Emergency, "Pod decelleration is too high");
  } else if (PRIMARY_BRAKING_ACCEL_X_MIN < get_value_f(&(state->accel_x))) {
    float ax = get_value_f(&(state->accel_x));
    float vx = get_value_f(&(state->velocity_x));

    if (is_pod_stopped(state)) {
      set_pod_mode(Shutdown, "Pod has stopped");
    } else if (ax > -vx) { // TODO: this calculation is BS.
      set_pod_mode(Emergency, "Pod decelleration is too low");
    }
  }
}

void skate_sensor_checks(pod_state_t *state) {
  // TODO: Make these checks bounded by min and max values
  bool ok = (get_value(&(state->skate_rear_left_z)) > 0) &&
            (get_value(&(state->skate_rear_right_z)) > 0) &&
            (get_value(&(state->skate_front_left_z)) > 0) &&
            (get_value(&(state->skate_front_right_z)) > 0);

  if (!ok) {
    set_pod_mode(Emergency, "A height sensor is returning 0");
  }
}

void lp_package_checks(pod_state_t *state) {
  int i;
  for (i = 0; i < N_LP_REGULATOR_THERMOCOUPLES; i++) {
    int32_t temp = get_value(&(state->lp_reg_thermocouples[i]));
    if (temp < MIN_REGULATOR_THERMOCOUPLE_TEMP) {
      set_pod_mode(Emergency, "Thermocouple %d for skates is too low");
    }
  }
}

void lateral_sensor_checks(pod_state_t *state) {

  int errors = 0;
  if (outside(LATERAL_MIN, get_value(&(state->lateral_front_left)),
              LATERAL_MAX)) {
    errors |= 0x1;
  }
  if (outside(LATERAL_MIN, get_value(&(state->lateral_front_right)),
              LATERAL_MAX)) {
    errors |= 0x2;
  }
  if (outside(LATERAL_MIN, get_value(&(state->lateral_rear_left)),
              LATERAL_MAX)) {
    errors |= 0x4;
  }
  if (outside(LATERAL_MIN, get_value(&(state->lateral_rear_right)),
              LATERAL_MAX)) {
    errors |= 0x8;
  }

  if (errors) {
    error("lateral error mask: %X", errors);
    set_pod_mode(Emergency, "lateral sensor(s) is out of bounds");
  }
}

int set_skate_target(int no, int val, bool override) {
  // TODO: Implement Me
  pod_state_t *state = get_pod_state();
  if (is_surface_overriden(SKATE_OVERRIDE_ALL) && !override &&
      state->tmp_skates != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_skates = val;
  return 0;
}

int set_caliper_brakes(int no, int val, bool override) {
  // TODO: Implement Me
  pod_state_t *state = get_pod_state();
  uint64_t skate_override[] = SKATE_OVERRIDE_LIST;
  if (is_surface_overriden(skate_override[no]) && !override &&
      state->tmp_brakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_brakes = val;
  return 0;
}

int set_emergency_brakes(int no, int val, bool override) {
  // TODO: Implement actually and also implement locking
  pod_state_t *state = get_pod_state();
  uint64_t ebrake_override[] = EBRAKE_OVERRIDE_LIST;

  if (is_surface_overriden(ebrake_override[no]) && !override &&
      state->tmp_ebrakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  state->tmp_ebrakes = val;
  return 0;
}

void adjust_brakes(pod_state_t *state) {
  int i;
  switch (get_pod_mode()) {
  case Ready:
  case Pushing:
  case Coasting:
    for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
      set_caliper_brakes(i, 0, false);
    }
    break;
  case Boot:
  case Shutdown:
  case Braking:
    for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
      set_caliper_brakes(i, 1, false);
    }
    break;
  case Emergency:
    if (get_value(&(state->accel_x)) <= A_ERR_X) {
      for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
        set_caliper_brakes(i, 1, false);
      }
      for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
        set_emergency_brakes(i, 0, false);
      }
    } else {
      error("==== Emergency Emergency Emergency ====");
      error("State is Emergency but not applying any brakes because accel x is "
            "> 0.0");
      error("==== Emergency Emergency Emergency ====");
    }
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a brake decsion");
  }
}

void adjust_skates(pod_state_t *state) {
  // Skates are completely controlled by pod state, therefore we can just
  // switch over them
  int i;
  switch (get_pod_mode()) {
  case Ready:
  case Pushing:
  case Coasting:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      set_skate_target(i, 1, false);
    }
    break;
  case Boot:
  case Emergency:
  case Shutdown:
  case Braking:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      set_skate_target(i, 1, false);
    }
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a skate decsion");
  }
}

/**
 * The Core Run Loop
 */
void *core_main(void *arg) {

  // TODO: Implement pinReset();
  pod_state_t *state = get_pod_state();

  imufd = imu_connect(IMU_DEVICE);

  if (imufd < 0) {
    return NULL;
  }
  size_t imu_score = 0, skate_score = 0;
  pod_mode_t mode;
  imu_datagram_t imu_data;
  while ((mode = get_pod_mode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Read new information from sensors
    // --------------------------------------------

    if (imu_read(imufd, &imu_data) < 0 && imu_score < IMU_SCORE_MAX) {
      imu_score += IMU_SCORE_STEP_UP;
      if (imu_score > IMU_SCORE_MAX) {
        DECLARE_EMERGENCY("IMU FAILED");
      }
    } else if (imu_score > 0) {
      imu_score -= IMU_SCORE_STEP_DOWN;
    }

    add_imu_data(&imu_data, state);

    if (skateRead(state) < 0 && skate_score < SKATE_SCORE_MAX) {
      DECLARE_EMERGENCY("SKATE READ FAILED");
    }

    if (lateralRead(state) < 0) {
      DECLARE_EMERGENCY("LATERAL READ FAILED");
    }

    // -------------------------------------------
    // SECTION: State Machine to determine actions
    // -------------------------------------------

    // General Checks (Going too fast, going too high)
    skate_sensor_checks(state);
    lateral_sensor_checks(state);

    if (get_value_f(&(state->velocity_x)) < -V_ERR_X && state->calibrated) {
      set_pod_mode(Emergency, "Pod rolling backward");
    }

    // Mode Specific Checks
    switch (get_pod_mode()) {
    case Boot:
      boot_state_checks(state);
      break;
    case Ready:
      ready_state_checks(state);
      break;
    case Pushing:
      pushing_state_checks(state);
      break;
    case Coasting:
      coasting_state_checks(state);
      break;
    case Braking:
      braking_state_checks(state);
      break;
    case Emergency:
      emergency_state_checks(state);
    default:
      break;
    }

    // -------------------------------------------
    // SECTION: Change the control surfaces
    // -------------------------------------------

    // Handle Skates
    adjust_skates(state);

    // Handle Wheel AND Ebrakes
    adjust_brakes(state);

    // -------------------------------------------
    // SECTION: Telemetry collection
    // -------------------------------------------
    logDump(state);

    // --------------------------------------------
    // Yield to other threads
    // --------------------------------------------
    usleep(CORE_THREAD_SLEEP);
  }

  imu_disconnect(imufd);

  return NULL;
}
