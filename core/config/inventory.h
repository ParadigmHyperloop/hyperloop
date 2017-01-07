#ifndef _OPENLOOP_POD_CONFIG_INVENTORY_
#define _OPENLOOP_POD_CONFIG_INVENTORY_

#include "airsupply.h"
#include "power.h"
#include "photo.h"
#include "overrides.h"

/// MISC Sensors and other constants.
#define TUBE_LENGTH 1600
#define STOP_MARGIN 200
// --------------------------
// Relay
// --------------------------
#define N_RELAY_CHANNELS 16

// --------------------------
// Frame
// --------------------------

#define N_SHELL_TRANSDUCERS 3
#define N_SHELL_THERMOCOUPLES 4

// --------------------------
// MUX
// --------------------------

#define N_MUXES 4
#define N_MUX_SELECT_PINS 4
#define MUX_SELECT_PINS                                                        \
  { 45, 44, 117, 125 }

#endif
