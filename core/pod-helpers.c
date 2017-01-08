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
  return within(-A_ERR_X, get_value_f(&(pod->accel_x)), A_ERR_X) &&
         within(-A_ERR_Y, get_value_f(&(pod->accel_y)), A_ERR_Y) &&
         within(-A_ERR_Z, get_value_f(&(pod->accel_z)), A_ERR_Z) &&
         within(-V_ERR_X, get_value_f(&(pod->velocity_x)), V_ERR_X) &&
         within(-V_ERR_Y, get_value_f(&(pod->velocity_y)), V_ERR_Y) &&
         within(-V_ERR_Z, get_value_f(&(pod->velocity_z)), V_ERR_Z);
}

bool is_pod_vented(pod_t *pod) {
  return is_hp_vented(pod) && is_lp_vented(pod);
}

bool is_hp_vented(pod_t *pod) {
  int i;
  for (i = 0; i < N_REG_PRESSURE; i++) {
    float psia = get_sensor(&(pod->lp_reg_transducers[i]));
    if (outside(MIN_SAFE_PSIA, psia, MAX_SAFE_PSIA)) {
      return false;
    }
  }
  return within(MIN_SAFE_PSIA, get_sensor(&(pod->hp_transducer)),
                MAX_SAFE_PSIA);
}

bool is_lp_vented(pod_t *pod) {
  int i;
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    float psia = get_sensor(&(pod->clamp_transducers[i]));
    if (outside(MIN_SAFE_PSIA, psia, MAX_SAFE_PSIA)) {
      return false;
    }
  }
  return true;
}

void setRelay(int pin, relay_state_t state) {
  switch (state) {
  case kRelayOn:
    setPinValue(pin, 0);
    break;
  case kRelayOff:
    setPinValue(pin, 1);
    break;
  default:
    DECLARE_EMERGENCY("UNKNOWN RELAY STATE");
  }
}

bool is_solenoid_open(solenoid_t *solenoid) {
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

bool is_solenoid_closed(solenoid_t *solenoid) {
  return !is_solenoid_open(solenoid);
}

void set_solenoid(solenoid_t *s, solenoid_state_t val) {
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

void open_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return;
  }

  if (!is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOn : kRelayOff));
    s->value = !s->value;
  }
}

void close_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return;
  }

  if (is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOff : kRelayOn));
    s->value = !s->value;
  }
}

bool is_solenoid_locked(solenoid_t *s) { return s->locked; }

void lock_solenoid(solenoid_t *s) { s->locked = true; }

void unlock_solenoid(solenoid_t *s) { s->locked = false; }
