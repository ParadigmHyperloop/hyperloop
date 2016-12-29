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
bool any_emergency_brakes(pod_state_t *state) {
  // TODO: no transducers on ebrake lines
  return state->tmp_ebrakes;
}

/**
 * Determine if emergency brakes are engaged
 */
bool any_calipers(pod_state_t *state) {
  // TODO: no transducers on caliper lines
  return state->tmp_brakes;
}

/**
 * Determines if the pod is currently stationary accounting for error in
 * readings
 */
bool is_pod_stopped(pod_state_t *state) {
  return within(-A_ERR_X, get_value_f(&(state->accel_x)), A_ERR_X) &&
         within(-A_ERR_Y, get_value_f(&(state->accel_y)), A_ERR_Y) &&
         within(-A_ERR_Z, get_value_f(&(state->accel_z)), A_ERR_Z) &&
         within(-V_ERR_X, get_value_f(&(state->velocity_x)), V_ERR_X) &&
         within(-V_ERR_Y, get_value_f(&(state->velocity_y)), V_ERR_Y) &&
         within(-V_ERR_Z, get_value_f(&(state->velocity_z)), V_ERR_Z);
}

void setRelay(int pin, relay_state_t state) {
  switch (state) {
    case kRelayOn:
      digitalWrite(pin, 0);
      break;
    case kRelayOff:
      digitalWrite(pin, 1);
      break;
    default:
      DECLARE_EMERGENCY("UNKNOWN RELAY STATE");
  }
}

bool is_solenoid_open(pod_solenoid_t * solenoid) {
  switch (solenoid->type) {
    case kSolenoidNormallyOpen:
      return solenoid->value != 1;
      break;
    case kSolenoidNormallyClosed:
      return solenoid->value == 1;
      break;
    default:
      DECLARE_EMERGENCY("UNKNOWN SOLENOID TYPE");
  }
}

void set_solenoid(pod_solenoid_t *s, solenoid_state_t val) {
  switch (val) {
    case kSolenoidOpen:
      open_solenoid(s);
      break;
    case kSolenoidClosed:
      close_solenoid(s);
      break;
    default:
      DECLARE_EMERGENCY("UNKOWN SOLENOID STATE");
  }
}

void open_solenoid(pod_solenoid_t *s) {
  if (!is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOn : kRelayOff));
    s->value = ! s->value;
  }
}


void close_solenoid(pod_solenoid_t *s) {
  if (is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOff : kRelayOn));
    s->value = ! s->value;
  }
}
