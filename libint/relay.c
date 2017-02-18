//
//  relay.c
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

#include "relay.h"
#include "pins.h"

void setRelay(int pin, relay_state_t state) {
  switch (state) {
    case kRelayOn:
      setPinValue(pin, 1);
      break;
    case kRelayOff:
      setPinValue(pin, 0);
      break;
    case kRelayError:
      // TOOD: Handle
      break;
  }
}


relay_state_t read_relay_state(int pin) {
  switch (getPinValue(pin)) {
    case 0:
      return kRelayOff;
      break;
    case 1:
      return kRelayOn;
      break;
    default:
      return kRelayError;
  }
}

