//
//  relay.h
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

#ifndef relay_h
#define relay_h

typedef enum relay_state {
  kRelayError,
  kRelayOff, // Relay is not actuated
  kRelayOn   // Relay is actuated
} relay_state_t;

void setRelay(int pin, relay_state_t state);

relay_state_t read_relay_state(int pin);

#endif /* relay_h */
