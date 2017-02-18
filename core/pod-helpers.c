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

#include "pod-helpers.h"

/**
 * Determine if emergency brakes are engaged
 */
bool any_clamp_brakes(pod_t *pod) {
  // TODO: no transducers on clamp lines
  return pod->tmp_clamps;
}

/**
 * Determine if emergency brakes are engaged
 */
bool any_calipers(pod_t *pod) {
  // TODO: no transducers on caliper lines
  return pod->tmp_brakes;
}

float get_stopping_distance(pod_t *pod) {
  float v = get_value_f(&(pod->velocity_x));
  return (-(v * v) / (2.0 * NOMINAL_CLAMP_BRAKING_ACCEL));
}

float get_remaining_distance(pod_t *pod) {
  float x = get_value_f(&(pod->position_x));
  return (TUBE_LENGTH - STOP_MARGIN) - x;
}

float get_stopping_deccel(pod_t *pod) {
  float remaining_distance = get_remaining_distance(pod);
  float v = get_value_f(&(pod->velocity_x));
  return -(v * v) / (2.0 * remaining_distance);
}

/**
 * Determines if the pod is currently stationary accounting for error in
 * readings
 */
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
    float psia = get_sensor(&(pod->reg_pressure[i]));
    if (OUTSIDE(MIN_SAFE_PSIA, psia, MAX_SAFE_PSIA)) {
      return false;
    }
  }
  return WITHIN(MIN_SAFE_PSIA, get_sensor(&(pod->hp_pressure)), MAX_SAFE_PSIA);
}

bool is_lp_vented(pod_t *pod) {
  int i;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    float psia = get_sensor(&(pod->clamp_pressure[i]));
    if (OUTSIDE(MIN_SAFE_PSIA, psia, MAX_SAFE_PSIA)) {
      return false;
    }
  }
  return true;
}

sensor_t * get_sensor_by_name(pod_t *pod, char *name) {
  int i;
  sensor_t *s = NULL;
  for (i=0;i<sizeof(pod->sensors)/sizeof(pod->sensors[0]);i++) {
    if ((s = pod->sensors[i]) != NULL) {
      if (strcmp(s->name, name) == 0) {
        return s;
      }
    }
  }
  return NULL;
}

sensor_t * get_sensor_by_address(pod_t *pod, int mux, int input) {
  int i;
  sensor_t *s = NULL;
  for (i=0;i<sizeof(pod->sensors)/sizeof(pod->sensors[0]);i++) {
    if ((s = pod->sensors[i]) != NULL) {
      if (s->mux == mux && s->input == input) {
        return s;
      }
    }
  }
  return NULL;
}



