//
//  solenoid.h
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

#ifndef solenoid_h
#define solenoid_h

#include <stdbool.h>
#include "relay.h"

#ifndef MAX_NAME
#define MAX_NAME 64
#endif

typedef enum solenoid_state {
  kSolenoidError,
  kSolenoidOpen,
  kSolenoidClosed
} solenoid_state_t;

typedef enum solenoid_type {
  kSolenoidNormallyOpen,
  kSolenoidNormallyClosed
} solenoid_type_t;

typedef struct solenoid {
  // The GPIO pin used to control this solenoid
  int gpio;
  // The Human Readable name of the solenoid
  char name[MAX_NAME];
  // The current value of this solenoid (0 for default position, 1 for active)
  int value;
  // Prevent this solenoid from changing state without an explicit unlock
  bool locked;
  // The logic type of the solenoid (Normally Open or Normally Closed)
  solenoid_type_t type;
} solenoid_t;

/**
 * Sets the desired solenoid state
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void set_solenoid(solenoid_t *s, solenoid_state_t val);
/**
 * Opens the solenoid if it is not already set to be open
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
bool open_solenoid(solenoid_t *s);

/**
 * Closes the given solenoid if it is not already set to closed
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
bool close_solenoid(solenoid_t *s);

/**
 * Inhibits the controller from changing the solenoid state.
 *
 * @note unlock_solenoid, the function to reverse this one, should only be
 * called by a human initiated action (such as a control point command)
 */
void lock_solenoid(solenoid_t *s);

/**
 * Removes an inhibit in place to prevent the given solenoid from changing
 * state
 *
 * @note should only be called by a human initiated action (such as a control
 * point command)
 */
void unlock_solenoid(solenoid_t *s);

/**
 * Determines if a solenoid is in it's open state
 */
bool is_solenoid_open(solenoid_t *s);

/**
 * Determines if a solenoid is in it's closed state
 */
bool is_solenoid_closed(solenoid_t *s);

/**
 * Determines if a solenoid is in a locked out state
 */
bool is_solenoid_locked(solenoid_t *s);

#endif /* solenoid_h */
