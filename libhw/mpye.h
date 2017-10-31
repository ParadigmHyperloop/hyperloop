//
//  mpye.h
//  pod
//
//  Created by Eddie Hurtig on 4/15/17.
//
//

#ifndef PARADIGM_MPYE_H
#define PARADIGM_MPYE_H

#include "hw.h"

#define MPYE_A_SETPOINT 900
#define MPYE_CLOSED_SETPOINT 2048
#define MPYE_B_SETPOINT 3000

typedef unsigned int mpye_value_t;

typedef struct mpye {
  // The I2C address of the controller managing this MPYE
  unsigned char address;
  // The PWM pin used to control this solenoid
  unsigned char channel;
  // The Human Readable name of the solenoid
  char name[HW_MAX_NAME];
  // The current value of this mpye (0 for closed, +100 full A, -100 full B)
  unsigned int value;
  // The current value of this mpye (0 for closed, +100 full A, -100 full B)
  bool queued;
  // Prevent this mpye from changing state without an explicit unlock
  bool locked;
  // A mutex for multithreaded access to this struct
  pthread_mutex_t mutex;
  // Scheduler
  bus_t *bus;
} mpye_t;

int mpye_init(mpye_t *m, char *name, bus_t *bus, unsigned char address, unsigned char channel);

/**
 * Sets the desired solenoid state
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void set_mpye(mpye_t *m, mpye_value_t val);

/**
 * Determines if a mpye is in a locked out state
 */
bool is_mpye_locked(mpye_t *m);

/**
 * Sets an inhibit in place to prevent the given solenoid from changing
 * state
 *
 * @note should only be called by a human initiated action (such as a control
 * point command)
 */
bool lock_mpye(mpye_t *m);

/**
 * Removes an inhibit in place to prevent the given mpye from changing
 * state
 *
 * @note should only be called by a human initiated action (such as a control
 * point command)
 */
bool unlock_mpye(mpye_t *m);

#endif /* PARADIGM_MPYE_H */
