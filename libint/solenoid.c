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
