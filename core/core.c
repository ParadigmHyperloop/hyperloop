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

int lateralRead(pod_t *pod);
int skateRead(pod_t *pod);
int brakingRead(pod_t *pod);

float maximumSafeForwardVelocity = 25.0;    // TODO: CHANGE ME! ARBITRARY!
float standardDistanceBeforeBraking = 75.0; // TODO: CHANGE ME! ARBITRARY!
float maximumSafeDistanceBeforeBraking = 125.0;

/**
 * Checks to be performed when the pod's state is Boot
 */
void boot_state_checks(pod_t *pod) {
  if (get_value(&(pod->ready)) == 1) {
    // TODO: Other Pre-flight Checks that are not Human checked
    set_pod_mode(LPFill, "Pod's Ready bit has been set");
  } else {
    info("Pod state is Boot, waiting for operator...");
  }
}

void post_state_checks(pod_t *pod) {}

void lp_fill_state_checks(pod_t *pod) {}

void hp_fill_state_checks(pod_t *pod) {}

void load_state_checks(pod_t *pod) {}

void standby_state_checks(pod_t *pod) {}

void armed_state_checks(pod_t *pod) {}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergency_state_checks(pod_t *pod) {
  if (is_pod_stopped(pod) && any_emergency_brakes(pod) && any_calipers(pod)) {
    set_pod_mode(Vent, "Pod has been determined to be ready for venting");
  }
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushing_state_checks(pod_t *pod) {
  if (get_value_f(&(pod->position_x)) > maximumSafeDistanceBeforeBraking) {
    set_pod_mode(Emergency, "Pod Position is > max travel before braking");
  } else if (get_value_f(&(pod->velocity_x)) > maximumSafeForwardVelocity) {
    set_pod_mode(Emergency, "Pod is going too fast");
  } else if (get_value_f(&(pod->accel_x)) <= COASTING_MIN_ACCEL_TRIGGER) {
    set_pod_mode(Coasting, "Pod has negative acceleration in the X dir");
  }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coasting_state_checks(pod_t *pod) {
  if (get_value_f(&(pod->position_x)) > maximumSafeDistanceBeforeBraking ||
      get_value_f(&(pod->velocity_x)) > maximumSafeForwardVelocity) {
    set_pod_mode(Emergency, "Pod has travelled too far");
  } else if (get_value_f(&(pod->position_x)) > standardDistanceBeforeBraking) {
    set_pod_mode(Braking, "Pod has entered braking range of travel");
  }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void braking_state_checks(pod_t *pod) {
  // TODO: This is an issue, Engineers need the look at this
  //       Do Not Ship without this baking algorithm reviewed
  if (PRIMARY_BRAKING_ACCEL_X_MAX > get_value_f(&(pod->accel_x))) {
    set_pod_mode(Emergency, "Pod decelleration is too high");
  } else if (PRIMARY_BRAKING_ACCEL_X_MIN < get_value_f(&(pod->accel_x))) {
    float ax = get_value_f(&(pod->accel_x));
    float vx = get_value_f(&(pod->velocity_x));

    if (is_pod_stopped(pod)) {
      set_pod_mode(Vent, "Pod has stopped");
    } else if (ax > -vx) { // TODO: this calculation is BS.
      set_pod_mode(Emergency, "Pod decelleration is too low");
    }
  }
}

void vent_state_checks(pod_t *pod) {}

void retrieval_state_checks(pod_t *pod) {}

void skate_sensor_checks(pod_t *pod) {
  // TODO: Make these checks bounded by min and max values
  bool ok = (get_value(&(pod->skate_rear_left_z)) > 0) &&
            (get_value(&(pod->skate_rear_right_z)) > 0) &&
            (get_value(&(pod->skate_front_left_z)) > 0) &&
            (get_value(&(pod->skate_front_right_z)) > 0);

  if (!ok) {
    set_pod_mode(Emergency, "A height sensor is returning 0");
  }
}

void lp_package_checks(pod_t *pod) {
  int i;
  for (i = 0; i < N_LP_REGULATOR_THERMOCOUPLES; i++) {
    int32_t temp = get_value(&(pod->lp_reg_thermocouples[i]));
    if (temp < MIN_REGULATOR_THERMOCOUPLE_TEMP) {
      set_pod_mode(Emergency, "Thermocouple %d for skates is too low");
    }
  }
}

void lateral_sensor_checks(pod_t *pod) {

  int errors = 0;
  if (outside(LATERAL_MIN, get_value(&(pod->lateral_front_left)),
              LATERAL_MAX)) {
    errors |= 0x1;
  }
  if (outside(LATERAL_MIN, get_value(&(pod->lateral_front_right)),
              LATERAL_MAX)) {
    errors |= 0x2;
  }
  if (outside(LATERAL_MIN, get_value(&(pod->lateral_rear_left)), LATERAL_MAX)) {
    errors |= 0x4;
  }
  if (outside(LATERAL_MIN, get_value(&(pod->lateral_rear_right)),
              LATERAL_MAX)) {
    errors |= 0x8;
  }

  if (errors) {
    error("lateral error mask: %X", errors);
    set_pod_mode(Emergency, "lateral sensor(s) is out of bounds");
  }
}

int set_skate_target(int no, solenoid_state_t val, bool override) {
  // TODO: Implement Me
  pod_t *pod = get_pod();
  if (is_surface_overriden(SKATE_OVERRIDE_ALL) && !override &&
      pod->tmp_skates != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  pod->tmp_skates = val;

  set_solenoid(&(pod->skate_solonoids[no]), val);
  return 0;
}

int set_caliper_brakes(int no, solenoid_state_t val, bool override) {
  // TODO: Implement Me
  pod_t *pod = get_pod();
  uint64_t skate_override[] = SKATE_OVERRIDE_LIST;
  if (is_surface_overriden(skate_override[no]) && !override &&
      pod->tmp_brakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  set_solenoid(&(pod->wheel_solonoids[no]), val);
  pod->tmp_brakes = val;
  return 0;
}

int set_emergency_brakes(int no, solenoid_state_t val, bool override) {
  // TODO: Implement actually and also implement locking
  pod_t *pod = get_pod();
  uint64_t ebrake_override[] = EBRAKE_OVERRIDE_LIST;

  if (is_surface_overriden(ebrake_override[no]) && !override &&
      pod->tmp_ebrakes != val) {
    warn("Skates are in override mode!");
    return -1;
  }

  set_solenoid(&(pod->ebrake_solonoids[no]), val);
  pod->tmp_ebrakes = val;
  return 0;
}

void adjust_brakes(pod_t *pod) {
  int i;
  switch (get_pod_mode()) {
  case POST:
  case Boot:
  case LPFill:
  case HPFill:
  case Load:
  case Standby:
  case Armed:
  case Pushing:
  case Coasting:
  case Vent:
  case Retrieval:
  case Shutdown:
    for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
      set_caliper_brakes(i, kSolenoidClosed, false);
    }
    for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
      set_emergency_brakes(i, kSolenoidClosed, false);
    }
    break;
  case Braking:
    set_emergency_brakes(PRIMARY_BRAKING_CLAMP, kSolenoidOpen, false);
    break;
  case Emergency:
    if (get_value(&(pod->accel_x)) <= A_ERR_X) {
      for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
        set_emergency_brakes(i, kSolenoidOpen, false);
      }
      // TODO: If both ebrakes applied but not optimal decel, apply calipers
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

void adjust_skates(pod_t *pod) {
  // Skates are completely controlled by pod state, therefore we can just
  // switch over them
  int i;
  switch (get_pod_mode()) {
  case POST:
  case Boot:
  case LPFill:
  case HPFill:
  case Load:
  case Standby:
  case Armed:
  case Vent:
  case Retrieval:
  case Emergency:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      set_skate_target(i, kSolenoidClosed, false);
    }
    break;
  case Pushing:
  case Coasting:
  case Braking:
  case Shutdown:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      set_skate_target(i, kSolenoidOpen, false);
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

  pod_t *pod = get_pod();

  size_t imu_score = 0, skate_score = 0;
  pod_mode_t mode;
  imu_datagram_t imu_data;
  while ((mode = get_pod_mode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Read new information from sensors
    // --------------------------------------------

    if (imu_read(pod->imu, &imu_data) < 0 && imu_score < IMU_SCORE_MAX) {
      imu_score += IMU_SCORE_STEP_UP;
      if (imu_score > IMU_SCORE_MAX) {
        DECLARE_EMERGENCY("IMU FAILED");
      }
    } else if (imu_score > 0) {
      imu_score -= IMU_SCORE_STEP_DOWN;
    }

    add_imu_data(&imu_data, pod);

    if (skateRead(pod) < 0 && skate_score < SKATE_SCORE_MAX) {
      DECLARE_EMERGENCY("SKATE READ FAILED");
    }

    if (lateralRead(pod) < 0) {
      DECLARE_EMERGENCY("LATERAL READ FAILED");
    }

    // -------------------------------------------
    // SECTION: State Machine to determine actions
    // -------------------------------------------

    // General Checks (Going too fast, going too high)
    skate_sensor_checks(pod);
    lateral_sensor_checks(pod);

    if (get_value_f(&(pod->velocity_x)) < -V_ERR_X && pod->calibrated) {
      set_pod_mode(Emergency, "Pod rolling backward");
    }

    // Mode Specific Checks
    switch (get_pod_mode()) {
    case POST:
      post_state_checks(pod);
      break;
    case Boot:
      boot_state_checks(pod);
      break;
    case LPFill:
      lp_fill_state_checks(pod);
      break;
    case HPFill:
      hp_fill_state_checks(pod);
      break;
    case Load:
      load_state_checks(pod);
      break;
    case Standby:
      standby_state_checks(pod);
      break;
    case Armed:
      armed_state_checks(pod);
      break;
    case Pushing:
      pushing_state_checks(pod);
      break;
    case Coasting:
      coasting_state_checks(pod);
      break;
    case Braking:
      braking_state_checks(pod);
      break;
    case Vent:
      vent_state_checks(pod);
      break;
    case Retrieval:
      retrieval_state_checks(pod);
      break;
    case Emergency:
      emergency_state_checks(pod);
    case Shutdown:
      warn("pod in shutdown mode, but still running");
    default:
      panic(POD_CORE_SUBSYSTEM, "Pod in unknown state!");
      break;
    }

    // -------------------------------------------
    // SECTION: Change the control surfaces
    // -------------------------------------------

    // Handle Skates
    adjust_skates(pod);

    // Handle Wheel AND Ebrakes
    adjust_brakes(pod);

    // -------------------------------------------
    // SECTION: Telemetry collection
    // -------------------------------------------
    logDump(pod);

    // --------------------------------------------
    // Yield to other threads
    // --------------------------------------------
    usleep(CORE_THREAD_SLEEP);
  }

  imu_disconnect(pod->imu);

  return NULL;
}
