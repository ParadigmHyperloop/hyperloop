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
// MUX
// --------------------------
#define N_MUXES 7
#define N_MUX_INPUTS 16
#define THERMO_MUX_0 0
#define THERMO_MUX_1 1
#define PRESSURE_MUX 2
#define DISTANCE_MUX 3
#define PHOTO_MUX 4
#define SPARE_MUX_0 5
#define SPARE_MUX_1 6
#endif
