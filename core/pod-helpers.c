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

bool start_hp_fill() {
  debug("start_hp_fill has been called, performing pre-transition checks");
  return set_pod_mode(HPFill, "Control Point Initiated HP Fill");
  //  if (pod_hp_safe_checklist(get_pod())) {
  //
  //  }
  return false;
}

bool start_standby() {
  debug("start_standby has been called, performing pre-transition checks");
  return set_pod_mode(Standby, "Control Point Initiated Standby");
}

bool any_clamp_brakes(__unused pod_t *pod) {
  for (int i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
    if (is_solenoid_open(&pod->clamp_engage_solonoids[i])) {
      return true;
    }
  }
  return false;
}

float get_stopping_distance(pod_t *pod) {
  float v = get_value_f(&(pod->velocity_x));
  return (-(v * v) / (2.0f * NOMINAL_CLAMP_BRAKING_ACCEL));
}

float get_remaining_distance(pod_t *pod) {
  float x = get_value_f(&(pod->position_x));
  return (TUBE_LENGTH - STOP_MARGIN) - x;
}

float get_stopping_deccel(pod_t *pod) {
  float remaining_distance = get_remaining_distance(pod);
  float v = get_value_f(&(pod->velocity_x));
  return -(v * v) / (2.0f * remaining_distance);
}

bool is_pod_stopped(pod_t *pod) {
  return WITHIN(-A_ERR_X, get_value_f(&(pod->accel_x)), A_ERR_X) &&
         WITHIN(-A_ERR_Y, get_value_f(&(pod->accel_y)), A_ERR_Y) &&
         WITHIN(-A_ERR_Z, get_value_f(&(pod->accel_z)), A_ERR_Z) &&
         WITHIN(-V_ERR_X, get_value_f(&(pod->velocity_x)), V_ERR_X) &&
         WITHIN(-V_ERR_Y, get_value_f(&(pod->velocity_y)), V_ERR_Y) &&
         WITHIN(-V_ERR_Z, get_value_f(&(pod->velocity_z)), V_ERR_Z);
}

bool is_pod_vented(pod_t *pod) {
  return is_hp_vented(pod) && is_lp_vented(pod);
}

bool is_hp_vented(pod_t *pod) {
  int i;
  for (i = 0; i < N_REG_PRESSURE; i++) {
    float psi = get_sensor(&(pod->reg_pressure[i]));
    if (OUTSIDE(MIN_SAFE_PSI, psi, MAX_SAFE_PSI)) {
      warn("Sensor %s is not reading safe PSI", pod->reg_pressure[i].name);
      return false;
    }
  }
  return WITHIN(MIN_SAFE_PSI, get_sensor(&(pod->hp_pressure)), MAX_SAFE_PSI);
}

bool is_lp_vented(pod_t *pod) {
  int i;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    float psi = get_sensor(&(pod->clamp_pressure[i]));
    if (OUTSIDE(MIN_SAFE_PSI, psi, MAX_SAFE_PSI)) {
      return false;
    }
  }
  return true;
}

sensor_t *get_sensor_by_name(pod_t *pod, char *name) {
  size_t i;
  sensor_t *s = NULL;
  for (i = 0; i < sizeof(pod->sensors) / sizeof(pod->sensors[0]); i++) {
    if ((s = pod->sensors[i]) != NULL) {
      if (strcmp(s->name, name) == 0) {
        return s;
      }
    }
  }
  return NULL;
}

sensor_t *get_sensor_by_address(pod_t *pod, int adc_num, int input) {
  size_t i;
  sensor_t *s = NULL;
  for (i = 0; i < sizeof(pod->sensors) / sizeof(pod->sensors[0]); i++) {
    if ((s = pod->sensors[i]) != NULL) {
      if (s->adc_num == adc_num && s->input == input) {
        return s;
      }
    }
  }
  return NULL;
}

bool is_pusher_present(pod_t *pod) {
  int pp_active_count = 0;
  flight_profile_t *profile = &pod->flight_profile;

  for (int pp = 0; pp < N_PUSHER_DISTANCE; pp++) {
    float distance = get_sensor(&pod->pusher_plate_distance[pp]);

    if (distance < get_pusher_distance_min(profile)) {
      pp_active_count++;
    }
  }
  if (pp_active_count >= 2) {
    pod->last_pusher_seen = get_time_usec();
    return true;
  } else if (pod->last_pusher_seen + get_pusher_timeout(profile) >
             get_time_usec()) {
    return true;
  }
  return false;
}
