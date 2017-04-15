/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#include "pod-helpers.h"
#include "pod.h"
#ifdef HAS_PRU
#include "pru.h"
#endif

void common_checks(pod_t *pod) {

  // Watchdog Timer
  if (pod->launch_time > 0) {
    uint64_t now = get_time_usec();
    if (now - pod->launch_time >= WATCHDOG_TIMER) {
      if (!is_pod_stopped(pod)) {
        if (!(get_pod_mode() == Braking || get_pod_mode() == Vent ||
              get_pod_mode() == Retrieval)) {
          set_pod_mode(Emergency, "Watchdog Timer Expired");
        }
      }
    }
  }
}
/**
 * Checks to be performed when the pod's state is Boot
 */
void boot_state_checks(__unused pod_t *pod) {
  // Waiting for lp fill command to transition to LP Fill State
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
    return set_pod_mode(HPFill, "Control Point Initiated HP Fill");
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
    float psia = get_sensor(&(pod->clamp_pressure[i]));

    if (psia < NOMINAL_MINI_TANK_PSIA + CLAMP_PRESSURE_ERR) {
      open_solenoid(&(pod->lp_fill_valve[i]));
    } else {
      close_solenoid(&(pod->lp_fill_valve[i]));
      lock_solenoid(&(pod->lp_fill_valve[i]));
    }
  }
}

void hp_fill_state_checks(pod_t *pod) {
  float psia = get_sensor(&(pod->hp_pressure));

  if (psia >= NOMINAL_HP_PSIA - HP_PRESSURE_ERR) {
    close_solenoid(&(pod->hp_fill_valve));
    lock_solenoid(&(pod->hp_fill_valve));
    set_pod_mode(Load, "HP System is full and locked");
  }
}

void load_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }

  if (get_value(&(pod->pusher_plate)) == 0) {
    set_pod_mode(Standby, "Standing By");
  }
}

void standby_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }
  if (get_value(&(pod->pusher_plate)) == 1) {
    set_pod_mode(Load, "Moving/Loading");
  }
}

void armed_state_checks(pod_t *pod) {
  if (!core_pod_checklist(pod)) {
    set_pod_mode(Emergency, "Core Checklist Failed");
  }

  if (get_value(&(pod->pusher_plate)) == 1) {
    set_pod_mode(Pushing, "Pusher Plate depressed for at least 0.1s");
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
  if (get_value_f(&(pod->accel_x)) <= COASTING_MIN_ACCEL_TRIGGER) {
    set_pod_mode(Coasting, "Pod has negative acceleration in the X dir");
  }

  if (get_value_f(&(pod->position_x)) > START_BRAKING) {
    set_pod_mode(Braking, "Pod has entered braking range of travel");
  }

  if (pod->launch_time == 0) {
    pod->launch_time = get_time_usec();
  }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coasting_state_checks(pod_t *pod) {
  if (get_value_f(&(pod->position_x)) > START_BRAKING) {
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

void vent_state_checks(pod_t *pod) {
  if (pod_safe_checklist(pod)) {
    set_pod_mode(Retrieval, "Pod Determined to be safe");
  } else {
    open_solenoid(&(pod->vent_solenoid));
  }
}

void retrieval_state_checks(__unused pod_t *pod) {}

void skate_sensor_checks(pod_t *pod) {
  int i;
  for (i = 0; i < N_CORNER_DISTANCE; i++) {
    if (OUTSIDE(HEIGHT_MIN, get_sensor(&(pod->corner_distance[i])),
                HEIGHT_MAX)) {
      set_pod_mode(Emergency, "Height sensor %d out of range", i);
    }
  }
}

void lp_package_checks(pod_t *pod) {
  int i;
  for (i = 0; i < N_REG_THERMO; i++) {
    float temp = get_sensor(&(pod->reg_thermo[i]));
    if (temp < REG_THERMO_MIN) {
      set_pod_mode(Emergency, "Thermocouple %d for skates is too low");
    }
  }
}

void lateral_sensor_checks(pod_t *pod) {
  int errors = 0;
  int i;
  for (i = 0; i < N_LATERAL_DISTANCE; i++) {
    if (OUTSIDE(LATERAL_MIN, get_sensor(&(pod->lateral_distance[i])),
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
  if (is_surface_overriden(SKATE_OVERRIDE_ALL) && !override) {
    warn("Skates are in override mode!");
    return -1;
  }

  set_solenoid(&(pod->skate_solonoids[no]), val);
  return 0;
}

int ensure_caliper_brakes(int no, solenoid_state_t val, bool override) {
  // TODO: Implement Me
  pod_t *pod = get_pod();
  uint64_t skate_override[] = SKATE_OVERRIDE_LIST;
  if (is_surface_overriden(skate_override[no]) && !override) {
    warn("Skates are in override mode!");
    return -1;
  }

  set_solenoid(&(pod->wheel_solonoids[no]), val);
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

  return 0;
}

void adjust_brakes(__unused pod_t *pod) {
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

void adjust_skates(__unused pod_t *pod) {
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

void adjust_vent(pod_t *pod) {
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
  case Braking:
    close_solenoid(&(pod->vent_solenoid));
    break;
  case Vent:
  case Retrieval:
  case Emergency:
  case Shutdown:
    open_solenoid(&(pod->vent_solenoid));
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a skate decsion");
  }
}

void adjust_hp_fill(pod_t *pod) {
  switch (get_pod_mode()) {
  case POST:
  case Boot:
  case LPFill:
  case Load:
  case Standby:
  case Armed:
  case Pushing:
  case Coasting:
  case Braking:
  case Vent:
  case Retrieval:
  case Emergency:
  case Shutdown:
    close_solenoid(&(pod->hp_fill_valve));
    break;
  case HPFill:
    open_solenoid(&(pod->hp_fill_valve));
    break;
  default:
    panic(POD_CORE_SUBSYSTEM,
          "Pod Mode unknown, cannot make a hp fill decsion");
  }
}

/**
 * The Core Run Loop
 */
void *core_main(__unused void *arg) {

  double iteration_time = 0;
  uint64_t usec_last = 0;

  info("Core Control Thread Started");
  pod_t *pod = get_pod();

  size_t imu_score = 0;
  pod_mode_t mode;
  imu_datagram_t imu_data;

  struct timespec next, now;
  get_timespec(&next);

  while ((mode = get_pod_mode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Realtime Deadline Miss Detection
    // --------------------------------------------

    clock_gettime(CLOCK_REALTIME, &now);
    timespec_add_us(&next, CORE_PERIOD_USEC);
    if (timespec_cmp(&now, &next) > 0) {
      fprintf(stderr, "Deadline miss for core thread\n");
      fprintf(stderr, "now: %ld sec %ld nsec next: %ld sec %ldnsec \n",
              now.tv_sec, now.tv_nsec, next.tv_sec, next.tv_nsec);
    }

    // --------------------------------------------
    // SECTION: Read new information from sensors
    // --------------------------------------------

    if (pod->imu > -1) {
      if (imu_read(pod->imu, &imu_data) <= 0 && imu_score < IMU_SCORE_MAX) {
        warn("BAD IMU READ");
        imu_score += IMU_SCORE_STEP_UP;
        if (imu_score > IMU_SCORE_MAX) {
          DECLARE_EMERGENCY("IMU FAILED");
        }
      } else if (imu_score > 0) {
        imu_score -= IMU_SCORE_STEP_DOWN;
      }
      add_imu_data(&imu_data, pod);
    }

#ifdef HAS_PRU
    pru_read(pod);
#endif

    // Pusher Plate D-Bounce
    if (pod->pusher_plate_override != 1) {
      if (get_value(&(pod->pusher_plate_raw)) == 1) {
        if (get_time_usec() - pod->last_pusher_plate_low > 0.1 * USEC_PER_SEC) {
          set_value(&(pod->pusher_plate_raw), 1);
        }
      } else {
        pod->last_pusher_plate_low = get_time_usec();
        set_value(&(pod->pusher_plate), 0);
      }
    }

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
      break;
    case Shutdown:
      warn("pod in shutdown mode, but still running");
      break;
    default:
      panic(POD_CORE_SUBSYSTEM, "Pod in unknown state!");
      break;
    }

    common_checks(pod);
    // -------------------------------------------
    // SECTION: Change the control surfaces
    // -------------------------------------------

    // Handle Skates
    adjust_skates(pod);

    // Handle Wheel AND Ebrakes
    adjust_brakes(pod);

    // Handle Vent
    adjust_vent(pod);

    // Set hp fill
    adjust_hp_fill(pod);

    // -------------------------------------------
    // SECTION: Telemetry collection
    // -------------------------------------------
    log_dump(pod);

    // --------------------------------------------
    // Heartbeat handling
    // --------------------------------------------
    if (get_time_usec() - pod->last_ping > HEARTBEAT_TIMEOUT * USEC_PER_SEC &&
        pod->last_ping > 0) {
      set_pod_mode(Emergency, "Heartbeat timeout");
    }

// --------------------------------------------
// Yield to other threads
// --------------------------------------------
#if _POSIX_C_SOURCE >= 200112L
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
#else
    struct timespec scratch;
    int64_t next_ns = timespec_to_nsec(&next);
    get_timespec(&scratch);
    int64_t scratch_ns = timespec_to_nsec(&scratch);

    assert(next_ns > -1);
    assert(scratch_ns > -1);
    if (next_ns > scratch_ns) {
      usleep((uint32_t)((float)(next_ns - scratch_ns) * 0.9f /
                        (float)NSEC_PER_USEC));
    }
#endif
    // -------------------------------------------------------
    // Compute how long it is taking for the main loop to run
    // -------------------------------------------------------
    uint64_t usec_now = get_time_usec();

    if (usec_last == 0) {
      usec_last = usec_now;
    } else {
      if (iteration_time == 0) {
        iteration_time = (double)((usec_now - usec_last));
      } else {
        iteration_time =
            (1.0 - ITERATION_TIME_ALPHA) * iteration_time +
            ITERATION_TIME_ALPHA * (double)((usec_now - usec_last));
      }
      usec_last = usec_now;
      set_value_f(&(pod->core_speed),
                  1.0f / ((float)iteration_time / (float)USEC_PER_SEC));
    }
  }

  warn("Core Control Thread Exiting");

  return NULL;
}
