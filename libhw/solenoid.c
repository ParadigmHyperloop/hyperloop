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

int ssr_board_init(__unused bus_t * b, __unused int address) {
//  bus_enqueue(b, ^(__unused bus_t *bus) {
//    
//    
//    // Paste SSR Board Bringup Here
//    
//    printf("%d\n", address);
//  });
  return 0;
}


int solenoid_init(solenoid_t *s, char *name, bus_t *bus, unsigned char address, unsigned char channel, solenoid_type_t type) {
  strncpy(s->name, name, MAX_NAME);
  s->bus = bus;
  s->address = address;
  s->channel = channel;
  s->type = type;
  
  // TODO: Read in from IC
  switch (s->type) {
    case kSolenoidNormallyClosed:
      s->state = kSolenoidClosed;
      break;
    case kSolenoidNormallyOpen:
      s->state = kSolenoidOpen;
      break;
  }

  s->locked = false;
  return 0;
}

bool is_solenoid_open(solenoid_t *solenoid) {
  return solenoid->state == kSolenoidOpen;
}

bool is_solenoid_closed(solenoid_t *solenoid) {
  return solenoid->state == kSolenoidClosed;
}


bool is_solenoid_opening(solenoid_t *solenoid) {
  return solenoid->state == kSolenoidOpening;
}


bool is_solenoid_closing(solenoid_t *solenoid) {
  return solenoid->state == kSolenoidClosing;
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
    default:
      abort();
  }
}

solenoid_state_t read_solenoid_state(__unused const solenoid_t *solenoid) {
  abort();
  return kSolenoidError;
}

bool open_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return is_solenoid_open(s);
  }
  
  if (is_solenoid_opening(s)) {
    return true;
  }

  if (!is_solenoid_open(s)) {
    int value;
    switch (s->type) {
      case kSolenoidNormallyOpen:
        value = 0;
        break;
      case kSolenoidNormallyClosed:
        value = 4095;
        break;
      default:
        abort();
    }
    
    printf("Opening Solenoid %s (Addr: %d Ch: %d Val: %d)\n", s->name, s->address, s->channel, value);


    set_ssr(s->bus->fd, s->address, s->channel, value);
    s->state = kSolenoidOpen;
  }
  return true;
}

bool close_solenoid(solenoid_t *s) {
  if (is_solenoid_locked(s)) {
    return is_solenoid_closed(s) || is_solenoid_closing(s);
  }
  
  if (is_solenoid_closing(s)) {
    return true;
  }

  if (is_solenoid_open(s)) {
    int value;
    switch (s->type) {
      case kSolenoidNormallyOpen:
        value = 4095;
        break;
      case kSolenoidNormallyClosed:
        value = 0;
        break;
      default:
        abort();
    }
    
    printf("Closing Solenoid %s (Addr: %d Ch: %d Val: %d)\n", s->name, s->address, s->channel, value);

    
    set_ssr(s->bus->fd, s->address, s->channel, value);
    s->state = kSolenoidClosed;
  }
  return true;
}

bool is_solenoid_locked(solenoid_t *s) { return s->locked; }

void lock_solenoid(solenoid_t *s) { s->locked = true; }

void unlock_solenoid(solenoid_t *s) { s->locked = false; }
