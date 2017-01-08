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

#define N_SHELL_PRESSURE 3
#define N_SHELL_THERMO 4

// --------------------------
// MUX
// --------------------------

#define N_MUXES 4
#define N_MUX_SELECT_PINS 4
#define MUX_SELECT_PINS                                                        \
  { 45, 44, 117, 125 }

#define THERMO_MUX_0 0
#define THERMO_MUX_1 1
#define PRESSURE_MUX 2
#define DISTANCE_MUX 3
#define PHOTO_MUX 4
#define SPARE_MUX_0 5
#define SPARE_MUX_1 6
#endif
