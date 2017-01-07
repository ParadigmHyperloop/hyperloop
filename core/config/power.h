
#ifndef _OPENLOOP_POD_CONFIG_POWER_
#define _OPENLOOP_POD_CONFIG_POWER_

// --------------------------
// Power Board
// --------------------------

#define N_POWER_THERMOCOUPLES 4
#define POWER_THERMOCOUPLES_BASE (PRU_BASE + 0)

// --------------------------
// Batteries
// --------------------------

#define N_BATTERIES 4
#define BATTERY_I2C_ADDRESSES                                                  \
  { 6, 7, 8, 9 }

#endif
