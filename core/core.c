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

#include "pod.h"
#ifdef HAS_PRU
#include "pru.h"
#endif

void common_checks(pod_t *pod) {

  // Watchdog Timer
  if (pod->launch_time > 0) {
    uint64_t now = get_time_usec();
    if (now - pod->launch_time >= WATCHDOG_TIMER) {
      if (!(get_pod_mode() == Braking || get_pod_mode() == Vent ||
            get_pod_mode() == Retrieval)) {
        set_pod_mode(Emergency, "Watchdog Timer Expired");
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

void post_state_checks(pod_t *pod) {
  if (pod_safe_checklist(pod) && pod->last_ping > 0) {
    set_pod_mode(Boot, "System POST checklist passed");
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
  if (core_pod_checklist(pod) && get_value(&(pod->pusher_plate)) == 0) {
    set_pod_mode(Standby, "Standing By");
  }
}

void standby_state_checks(pod_t *pod) {
  if (core_pod_checklist(pod) && get_value(&(pod->pusher_plate)) == 1) {
    set_pod_mode(Load, "Moving/Loading");
  }
}

void armed_state_checks(pod_t *pod) {
  if (core_pod_checklist(pod) && get_value(&(pod->pusher_plate)) == 1) {
    set_pod_mode(Pushing, "Pusher Plate depressed for at least 0.1s");
  }
}

/**
 * Checks to be performed when the pod's state is Emergency
 */
void emergency_state_checks(pod_t *pod) {
  if (is_pod_stopped(pod) && any_clamp_brakes(pod)) {
    if (time_in_state() > EMERGENCY_HOLD) {
      set_pod_mode(Vent, "Pod has been determined to be ready for venting");
    }
  }
}

/**
 * Checks to be performed when the pod's state is Pushing
 */
void pushing_state_checks(pod_t *pod) {
  if (get_value_f(&(pod->accel_x)) <= COASTING_MIN_ACCEL_TRIGGER) {
    set_pod_mode(Coasting, "Pod has negative acceleration in the X dir");
  }

//  if (get_value_f(&(pod->position_x)) > START_BRAKING) {
//    set_pod_mode(Braking, "Pod has entered braking range of travel");
//  }

  if (pod->launch_time == 0) {
    pod->launch_time = get_time_usec();
  }
}

/**
 * Checks to be performed when the pod's state is Coasting
 */
void coasting_state_checks(__unused pod_t *pod) {
//  if (get_value_f(&(pod->position_x)) > START_BRAKING) {
//    set_pod_mode(Braking, "Pod has entered braking range of travel");
//  }
}

/**
 * Checks to be performed when the pod's state is Braking
 */
void braking_state_checks(pod_t *pod) {
  
  if (time_in_state() > BRAKING_WAIT) {
    if (get_value_f(&(pod->accel_x)) < PRIMARY_BRAKING_ACCEL_X_MIN) {
      debug("Suboptimal Braking");
      set_pod_mode(Emergency, "Pod decelleration is too low");
    }
  }
  
  if (time_in_state() > BRAKING_TIMEOUT) {
    set_pod_mode(Vent, "Braking Time Expired");
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

void skate_sensor_checks(__unused pod_t *pod) {
  // PASS
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

void lateral_sensor_checks(__unused pod_t *pod) {
 // PASS
}

int set_skate_target(int no, mpye_value_t val, bool override) {
  pod_t *pod = get_pod();
  if (is_surface_overriden(SKATE_OVERRIDE_ALL) && !override) {
    warn("Skates are in override mode!");
    return -1;
  }

  if (val > 0) {
    set_solenoid(&(pod->skate_solonoids[no]), kSolenoidOpen);
  } else {
    set_solenoid(&(pod->skate_solonoids[no]), kSolenoidClosed);
  }

  set_mpye(&(pod->mpye[no]), val);

  return 0;
}

int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override) {
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
  switch (get_pod_mode()) {
  case POST:
  case Boot:
  case HPFill:
  case Load:
  case Standby:
  case Armed:
  case Vent:
  case Retrieval:
  case Pushing:
  case Coasting:
  case Shutdown:
    for (int i = 0; i < N_CLAMP_SOLONOIDS; i++) {
      ensure_clamp_brakes(i, kClampBrakeReleased, true);
    }
    break;
  case Braking:
    for (int i = 0; i < N_CLAMP_SOLONOIDS; i++) {
      if (get_stopping_deccel(pod) > get_value_f(&(pod->accel_x))) {
        ensure_clamp_brakes(i, kClampBrakeClosed, false);
      } else {
        ensure_clamp_brakes(i, kClampBrakeEngaged, false);
      }
    }
  case Emergency:
    for (int i = 0; i < N_CLAMP_SOLONOIDS; i++) {
      ensure_clamp_brakes(i, kClampBrakeEngaged, false);
    }
    break;
  default:
    panic(POD_CORE_SUBSYSTEM, "Pod Mode unknown, cannot make a skate decsion");
  }
}

void adjust_skates(__unused pod_t *pod) {
  int i;
  switch (get_pod_mode()) {
  case POST:
  case Boot:
  case HPFill:
  case Load:
  case Standby:
  case Armed:
  case Vent:
  case Retrieval:
  case Emergency:
  case Shutdown:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      set_skate_target(i, 0, false);
    }
    break;
  case Pushing:
  case Coasting:
  case Braking:
    for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
      // TODO Implement PID for Skates
      set_skate_target(i, 100, false);
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
//
//static void setup(void) {
//  pod_t *pod = get_pod();
//  
//  debug("=== Begin Setup ===");
//  for (int i = 0; i < N_MPYES; i++) {
//    mpye_t *m = &(pod->mpye[i]);
//    set_ssr(m->bus->fd, m->address, m->channel, 0);
//  }
//  
//  for (int i = 0; i < N_SKATE_SOLONOIDS; i++) {
//    solenoid_t *s = &(pod->skate_solonoids[i]);
//    set_ssr(s->bus->fd, s->address, s->channel, 0);
//  }
//  
//  for (int i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
//    solenoid_t *s = &(pod->clamp_engage_solonoids[i]);
//    set_ssr(s->bus->fd, s->address, s->channel, 0);
//  }
//
//  
//  for (int i = 0; i < N_CLAMP_RELEASE_SOLONOIDS; i++) {
//    solenoid_t *s = &(pod->clamp_release_solonoids[i]);
//    set_ssr(s->bus->fd, s->address, s->channel, 0);
//  }
//
//  solenoid_t *s;
//  s = &(pod->hp_fill_valve);
//  set_ssr(s->bus->fd, s->address, s->channel, 0);
//  
//  s = &(pod->vent_solenoid);
//  set_ssr(s->bus->fd, s->address, s->channel, 0);
//  debug("=== End Setup ===");
//}
//
//
//static void functional_check(void) {
//  pod_t *pod = get_pod();
//  debug("=== Begin Functional Check ===");
//  
//  for (int iterations = 0; iterations < 10; iterations++) {
//    for (uint8_t j = 0; j <= 100; j++) {
//      for (int i = 0; i < N_MPYES; i++) {
//        mpye_t *m = &(pod->mpye[i]);
//        set_mpye(m, j);
//      }
//    }
//    
//    for (uint8_t j = 100; j >= 0; j--) {
//      for (int i = 0; i < N_MPYES; i++) {
//        mpye_t *m = &(pod->mpye[i]);
//        set_mpye(m, j);
//      }
//    }
//  }
//
//  sleep(5);
//  
//  for (int iterations = 0; iterations < 2; iterations++) {
//    for (int i = 0; i < N_SKATE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->skate_solonoids[i]);
//      open_solenoid(s);
//      sleep(1);
//    }
//    
//    for (int i = 0; i < N_SKATE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->skate_solonoids[i]);
//      close_solenoid(s);
//      sleep(1);
//    }
//  }
//  
//  sleep(5);
//
//  for (int iterations = 0; iterations < 2; iterations++) {
//    for (int i = 0; i < N_SKATE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->skate_solonoids[i]);
//      open_solenoid(s);
//      sleep(1);
//    }
//    
//    for (int i = 0; i < N_SKATE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->skate_solonoids[i]);
//      close_solenoid(s);
//      sleep(1);
//    }
//  }
//
//  sleep(5);
//  
//  for (int iterations = 0; iterations < 2; iterations++) {
//    for (int i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->clamp_engage_solonoids[i]);
//      open_solenoid(s);
//      sleep(1);
//    }
//    
//    for (int i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->clamp_engage_solonoids[i]);
//      close_solenoid(s);
//      sleep(1);
//    }
//  }
//
//  sleep(5);
//  
//  for (int iterations = 0; iterations < 2; iterations++) {
//    for (int i = 0; i < N_CLAMP_RELEASE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->clamp_release_solonoids[i]);
//      open_solenoid(s);
//      sleep(1);
//    }
//    for (int i = 0; i < N_CLAMP_RELEASE_SOLONOIDS; i++) {
//      solenoid_t *s = &(pod->clamp_release_solonoids[i]);
//      close_solenoid(s);
//      sleep(1);
//    }
//  }
//
//  
//  solenoid_t *s;
//  // HP Fill Valve
//  s = &(pod->hp_fill_valve);
//  open_solenoid(s);
//  sleep(13);
//  close_solenoid(s);
//  sleep(13);
//  
//  // Vent Solenoid
//  s = &(pod->vent_solenoid);
//  open_solenoid(s);
//  sleep(1);
//  close_solenoid(s);
//  set_ssr(s->bus->fd, s->address, s->channel, 0);
//  
//  debug("=== End Setup ===");
//}
//

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
  
  /// ADC Init
  adc_t adc[2];
  int spi_fd = open_spi();
  init_adc(&adc[0], spi_fd, "ADC6", 6);
  init_adc(&adc[1], spi_fd, "ADC7", 7);
  
  struct timespec next, now;
  get_timespec(&next);

//  setup();
//
//  if (pod->func_test) {
//    functional_check();
//  }

  while ((mode = get_pod_mode()) != Shutdown) {
    // --------------------------------------------
    // SECTION: Realtime Deadline Miss Detection
    // --------------------------------------------

    clock_gettime(CLOCK_REALTIME, &now);
    timespec_add_us(&next, CORE_PERIOD_USEC);
    if (timespec_cmp(&now, &next) > 0) {
      fprintf(stderr, "Deadline miss for core thread\n");
      fprintf(stderr, "now: %ld.%ldns next: %ld.%ldns ()\n",
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

    for (int a = 6; a < 8; a++) {
      int rc = set_gpio_for_adc(&adc[a - 6]);
      if (rc < 0) {
        set_pod_mode(Emergency, "Motherboard Demux Communication Failure");
      }

      for (uint8_t channel = 0; channel < 16; channel++) {
        sensor_t *s = get_sensor_by_address(pod, a, channel);
        if (s != NULL) {
          

          int value = read_adc(&adc[a - 6], channel);
          if (value < 0) {
            set_pod_mode(Emergency, "Motherboard Communication Failure");
            continue;
          }
          
          double voltage = value * 0.0012207;

          debug("Sensor %s: ADC%d Channel %02d: %d (%lf Volts)", s->name, a, channel, value, voltage);

          queue_sensor(s, value);
          update_sensor(s);
        } else {
          debug("No Sensor for ADC%d Channel %d", a, channel);
        }
      }
    }

    // Pusher Plate


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

    #pragma mark core-telemetry

    // -------------------------------------------
    // SECTION: Telemetry dump
    // -------------------------------------------
    log_dump(pod);

    #pragma mark core-heartbeat-check
    // --------------------------------------------
    // Heartbeat handling
    // --------------------------------------------
//    if (get_time_usec() - pod->last_ping > HEARTBEAT_TIMEOUT_USEC &&
//        pod->last_ping > 0 && get_pod_mode() != Emergency) {
//      set_pod_mode(Emergency, "Heartbeat timeout");
//    }

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
