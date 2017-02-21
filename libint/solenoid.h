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
