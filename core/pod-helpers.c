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
bool emergencyBrakesEngaged(pod_state_t *state) {
  int i;
  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    if (!within(PRIMARY_BRAKING_ENGAGED_MIN_F,
                getPodField(&(state->ebrake_pressures[i])),
                PRIMARY_BRAKING_ENGAGED_MAX_F)) {
      return false;
    }
  }

  return true;
}

/**
 * Determine if emergency brakes are engaged
 */
bool primaryBrakesEngaged(pod_state_t *state) {
  int i;
  for (i = 0; i < N_EBRAKE_PRESSURES; i++) {
    if (!within(EBRAKE_ENGAGED_MIN_F,
                getPodField(&(state->ebrake_pressures[i])),
                EBRAKE_ENGAGED_MAX_F)) {
      return false;
    }
  }

  return true;
}

/**
 * Determines if the pod is currently stationary accounting for error in
 * readings
 */
bool podIsStopped(pod_state_t *state) {
  return within(-A_ERR_X, getPodField_f(&(state->accel_x)), A_ERR_X) &&
         within(-A_ERR_Y, getPodField_f(&(state->accel_y)), A_ERR_Y) &&
         within(-A_ERR_Z, getPodField_f(&(state->accel_z)), A_ERR_Z) &&
         within(-V_ERR_X, getPodField_f(&(state->velocity_x)), V_ERR_X) &&
         within(-V_ERR_Y, getPodField_f(&(state->velocity_y)), V_ERR_Y) &&
         within(-V_ERR_Z, getPodField_f(&(state->velocity_z)), V_ERR_Z);
}
