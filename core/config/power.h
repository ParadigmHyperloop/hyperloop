
#ifndef _OPENLOOP_POD_CONFIG_POWER_
#define _OPENLOOP_POD_CONFIG_POWER_

// --------------------------
// Power Board
// --------------------------

#define KILL_PIN 86

#define KILL_PIN_KILL_VALUE 0
#define KILL_PIN_OK_VALUE 1

#define PB_PIN 87
#define PB_ON_VAL 1
#define PB_OFF_VAL 0

#define N_POWER_THERMO 4
#define POWER_THERMO_MUX THERMO_MUX_1
#define POWER_THERMO_INPUTS                                                    \
  { 6, 7, 8, 9 }

// --------------------------
// Batteries
// --------------------------

#define N_BATTERIES 3
#define BATTERY_I2C_ADDRESSES                                                  \
  { 0x0b, 0x0b, 0x0b }

// --------------------------
// Frame Thermocouple
// --------------------------
#define N_FRAME_THERMO 1
#define FRAME_THERMO_MUX THERMO_MUX_1
#define FRAME_THERMO_INPUTS                                                    \
  { 10 }

#endif
