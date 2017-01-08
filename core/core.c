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
#include "pru.h"

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

bool core_pod_checklist(pod_t *pod) {
  // TODO: is_battery_power_ok()  // Voltage > 28, current > 0.2 A @james
  // TODO: is_rpm_ok()            // less than 6000 @akeating
  // TODO: is_imu_ok()            // temp (-40°C to +75°C) VERIFIED
  // TODO: is_velocity_too_fast() // 95 m/s (roughly 215 mph) @akeating

  // TODO: is_reg_temp_ok()       // 0 -> 50 @akeating
  // TODO: is_clamp_temp_ok()    // 0 -> 100something @akeating
  // TODO: is_battery_temp_ok()   // 0 -> 60something @james
  // TODO: is_caliper_temp_ok()   // 0 -> 100something @akeating
  // TODO: is_frame_temp_ok()     // 0 -> 40 C @edhurtig

  // TODO: is_frame_pressure_ok() // 0 -> 20 PSIA VERIFIED
  // TODO: is_hp_pressure_ok()    // 0 -> 1770 PSI... @akeating
  // TODO: is_lp_pressure_ok()    // 0 -> 150 PSI... @akeating
  int i;
  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    if (is_solenoid_open(&(pod->lp_fill_valve[i]))) {
      return false;
    }
  }

  if (is_solenoid_open(&(pod->hp_fill_valve))) {
    return false;
  }

  return true;
}

/**
 * Is the pod safe. Used to inhibit transitions to various different states
 */
bool pod_safe_checklist(pod_t *pod) {
  return core_pod_checklist(pod) && is_pod_stopped(pod) && is_pod_vented(pod);
}

/**
 * Is the pod safe to proceed to an HP Fill
 */
bool pod_hp_safe_checklist(pod_t *pod) {
  return core_pod_checklist(pod) && is_pod_stopped(pod) && is_hp_vented(pod);
}

/**
 * Attempt to transition the the LP Fill state
 */
bool start_lp_fill() {
  if (pod_safe_checklist(get_pod())) {
    return set_pod_mode(LPFill, "Control Point Initiated LP Fill");
  }
  return false;
}

/**
 * Attempt to transition to the HP FIll State
 */
bool start_hp_fill() {
  if (pod_hp_safe_checklist(get_pod())) {
    return set_pod_mode(HPFill, "Control Point Initiated LP Fill");
  }
  return false;
}

void post_state_checks(pod_t *pod) {
  if (pod_safe_checklist(pod) && pod->last_ping > 0) {
    set_pod_mode(Boot, "System POST checklist passed");
  }
}

void lp_fill_state_checks(pod_t *pod) {
  int i;

  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    float psia = get_sensor(&(pod->clamp_transducers[i]));

    if (psia < NOMINAL_MINI_TANK_PSIA + CLAMP_PRESSURE_ERR) {
      open_solenoid(&(pod->lp_fill_valve[i]));
    } else {
      close_solenoid(&(pod->lp_fill_valve[i]));
      lock_solenoid(&(pod->lp_fill_valve[i]));
    }
  }
}

void hp_fill_state_checks(pod_t *pod) {
  float psia = get_sensor(&(pod->hp_transducer));

  if (psia < NOMINAL_HP_PSIA + HP_PRESSURE_ERR) {
    open_solenoid(&(pod->hp_fill_valve));
  } else {
    close_solenoid(&(pod->hp_fill_valve));
    lock_solenoid(&(pod->hp_fill_valve));
  }
}

void load_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }
}

void standby_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }
}

void armed_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }
}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergency_state_checks(pod_t *pod) {
  if (is_pod_stopped(pod) && any_clamp_brakes(pod) && any_calipers(pod)) {
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
  int i;
  for (i=0;i<N_CORNER_DISTANCE;i++) {
    if (outside(HEIGHT_MIN, get_sensor(&(pod->corner_distance[i])), HEIGHT_MAX)) {
      set_pod_mode(Emergency, "Height sensor %d out of range", i);
    }
  }
}

void lp_package_checks(pod_t *pod) {
  int i;
  for (i = 0; i < N_REG_THERMO; i++) {
    float temp = get_sensor(&(pod->lp_reg_thermocouples[i]));
    if (temp < REG_THERMO_MIN) {
      set_pod_mode(Emergency, "Thermocouple %d for skates is too low");
    }
  }
}

void lateral_sensor_checks(pod_t *pod) {

  int errors = 0;
  int i;
  for (i=0;i<N_LATERAL_DISTANCE;i++) {
    if (outside(LATERAL_MIN, get_sensor(&(pod->lateral_distance[i])),
                LATERAL_MAX)) {
      errors |= (0x1 << i);
    }
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

int ensure_caliper_brakes(int no, solenoid_state_t val, bool override) {
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

int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override) {
  // TODO: Implement actually and also implement locking
  pod_t *pod = get_pod();
  uint64_t clamp_override[] = CLAMP_OVERRIDE_LIST;

  if (is_surface_overriden(clamp_override[no]) && !override) {
    warn("Clamps are in override mode!");
    return -1;
  }

  switch (val) {
  case kClampBrakeClosed:
    set_solenoid(&(pod->clamp_engage_solonoids[no]), kSolenoidClosed);
    set_solenoid(&(pod->clamp_release_solonoids[no]), kSolenoidClosed);
    break;
  case kClampBrakeEngaged:
    set_solenoid(&(pod->clamp_engage_solonoids[no]), kSolenoidOpen);
    set_solenoid(&(pod->clamp_release_solonoids[no]), kSolenoidClosed);
    break;
  case kClampBrakeReleased:
    set_solenoid(&(pod->clamp_engage_solonoids[no]), kSolenoidClosed);
    set_solenoid(&(pod->clamp_release_solonoids[no]), kSolenoidOpen);
    break;
  default:
    DECLARE_EMERGENCY("Invalid clamp_brake_state_t");
  }

  // TODO: Remove
  pod->tmp_clamps = val;
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
      ensure_caliper_brakes(i, kSolenoidClosed, false);
    }
    for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
      ensure_clamp_brakes(i, kClampBrakeReleased, false);
    }
    break;
  case Braking:
    ensure_clamp_brakes(PRIMARY_BRAKING_CLAMP, kClampBrakeEngaged, false);
    break;
  case Emergency:
    if (get_value(&(pod->accel_x)) <= A_ERR_X) {
      for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
        ensure_clamp_brakes(i, kClampBrakeEngaged, false);
      }
      // TODO: If both clamps applied but not optimal decel, apply calipers
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

  static double iteration_time = 0;
  static uint64_t last = 0;

  pod_t *pod = get_pod();

  size_t imu_score = 0;
  pod_mode_t mode;
  imu_datagram_t imu_data;
  sensor_pack_t pack;
  memset(&pack, 0, sizeof(sensor_pack_t));

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
    // ADC_READ

    // ------------
    // If ADC buffer ready
    // ------------
    // else do nothing
    // ------------
    //  Done with ADC Read
    // ------------

    memset(&pack, 0, sizeof(sensor_pack_t));
    pru_read(&pack);

    // -------------------------------------------
    // SECTION: State Machine to determine actions
    // -------------------------------------------

    // General Checks (Going too fast, going too high)

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
      break;
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
    // Heartbeat handling
    // --------------------------------------------
    if (get_time() - pod->last_ping > HEARTBEAT_TIMEOUT * USEC_PER_SEC &&
        pod->last_ping > 0) {
      set_pod_mode(Emergency, "Heartbeat timeout");
    }

    // --------------------------------------------
    // Yield to other threads
    // --------------------------------------------
    usleep(CORE_THREAD_SLEEP);
    usleep(1 * USEC_PER_SEC);
    // -------------------------------------------------------
    // Compute how long it is taking for the main loop to run
    // -------------------------------------------------------
    uint64_t now = get_time();

    if (last == 0) {
      last = now;
    } else {
      if (iteration_time == 0) {
        iteration_time = (double)((now - last));
      } else {
        iteration_time = 0.99 * iteration_time + 0.01 * (double)((now - last));
      }
      last = now;
      set_value_f(&(pod->core_speed),
                  1.0 / (iteration_time / (double)USEC_PER_SEC));
    }
  }

  return NULL;
}
