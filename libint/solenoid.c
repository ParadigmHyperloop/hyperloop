//
//  solenoid.c
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

#include "solenoid.h"

bool is_solenoid_open(solenoid_t *solenoid) {
  switch (solenoid->type) {
    case kSolenoidNormallyOpen:
      return solenoid->value != 1;
      break;
    case kSolenoidNormallyClosed:
      return solenoid->value == 1;
      break;
  }
}

bool is_solenoid_closed(solenoid_t *solenoid) {
  switch (solenoid->type) {
    case kSolenoidNormallyOpen:
      return solenoid->value == 1;
      break;
    case kSolenoidNormallyClosed:
      return solenoid->value != 1;
      break;
  }
}

void set_solenoid(solenoid_t *s, solenoid_state_t val) {
  switch (val) {
    case kSolenoidOpen:
      open_solenoid(s);
      break;
    case kSolenoidClosed:
      close_solenoid(s);
      break;
    case kSolenoidError:
      // TOOD: Handle
      break;
  }
}

solenoid_state_t read_solenoid_state(solenoid_t *solenoid) {
  relay_state_t r = read_relay_state(solenoid->gpio);
  switch (r) {
    case kRelayOn:
      return (solenoid->type == kSolenoidNormallyClosed ? kSolenoidOpen : kSolenoidClosed);
      break;
    case kRelayOff:
      return (solenoid->type == kSolenoidNormallyClosed ? kSolenoidClosed : kSolenoidOpen);
      break;
    default:
      // TODO: Handle
      return kSolenoidError;
  }
}

bool open_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return is_solenoid_open(s);
  }
  
  if (!is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOff : kRelayOn));
    s->value = (s->value == 0 ? 1 : 0);
  }
  return true;
}

bool close_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return is_solenoid_closed(s);
  }
  
  if (is_solenoid_open(s)) {
    // TODO: Prove
    setRelay(s->gpio, (s->value ? kRelayOff : kRelayOn));
    s->value = (s->value == 0 ? 1 : 0);
  }
  return true;
}

bool is_solenoid_locked(solenoid_t *s) { return s->locked; }

void lock_solenoid(solenoid_t *s) { s->locked = true; }

void unlock_solenoid(solenoid_t *s) { s->locked = false; }
