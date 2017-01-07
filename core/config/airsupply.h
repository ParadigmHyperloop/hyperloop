#ifndef _OPENLOOP_POD_CONFIG_AIRSUPPLY_
#define _OPENLOOP_POD_CONFIG_AIRSUPPLY_

// --------------------------
// Configuration Constants
// --------------------------
#define MIN_SAFE_PSIA -10
#define MAX_SAFE_PSIA 20

#define NOMINAL_HP_PSIA 1770
#define NOMINAL_MINI_TANK_PSIA 145

// --------------------------
// HP Package
// --------------------------
#define N_HP_TRANSDUCERS 1
#define HP_TRANSDUCERS_BASE (PRU_BASE + 0)
#define HP_TRANSDUCERS_ERR 10

#define N_HP_THERMOCOUPLES 1
#define HP_THERMOCOUPLES_BASE (PRU_BASE + 0)

// --------------------------
// LP Package
// --------------------------
#define N_LP_REGULATOR_TRANSDUCERS 4
#define LP_REGULATOR_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_LP_THERMOCOUPLES 4
#define LP_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_LP_REGULATOR_THERMOCOUPLES 4
#define LP_REGULATOR_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define LP_TRANS_ERR 1 // PSI
// --------------------------
// Skates
// --------------------------

#define N_SKATE_SOLONOIDS 6
#define SKATE_SOLENOIDS                                                        \
  { 10, 9, 8, 78, 76, 74 }

#define N_SKATE_TRANSDUCERS 6
#define SKATE_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_MPYES 6
#define MPYE_I2C_DAC_ADDR 14

// --------------------------
// Emergency Brakes
// --------------------------

#define N_EBRAKE_SOLONOIDS 2
#define EBRAKE_SOLONOIDS                                                       \
  { 72, 70 }

// Which solenoid (index) to use for primary braking
#define PRIMARY_BRAKING_CLAMP 1

#define N_EBRAKE_PAD_THERMOCOUPLES 2
#define EBRAKE_PAD_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_EBRAKE_LINE_THERMOCOUPLES 2
#define EBRAKE_LINE_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_EBRAKE_TRANSDUCERS 2
#define EBRAKE_TRANSDUCERS_BASE (PRU_BASE + 0)

#define CLAMP_BRAKING_ACCEL -8.134 // m/s^2 (-0.83 G)

// --------------------------
// Wheels
// --------------------------

#define N_WHEEL_SOLONOIDS 3
#define WHEEL_SOLONOIDS                                                        \
  { 11, 81, 80 }

#define N_WHEEL_THERMOCOUPLES 3
#define WHEEL_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_WHEEL_PHOTODIODES 3
#define WHEEL_PHOTODIODES_BASE (PRU_BASE + 0)

// --------------------------
// HP Fill
// --------------------------

#define HP_FILL_SOLENOID 79

// --------------------------
// LP FILL
// --------------------------

#define N_LP_FILL_SOLENOIDS 2
#define LP_FILL_SOLENOIDS                                                      \
  { 77, 75 }

// --------------------------
// LP FILL
// --------------------------

#define N_LAT_FILL_SOLENOIDS 2
#define LAT_FILL_SOLENOIDS                                                     \
  { 77, 75 }

#define N_LAT_FILL_TRANSDUCERS 2
#define LAT_FILL_TRANSDUCERS_BASE (PRU_BASE + 0)

// --------------------------
// Releif solenoid
// --------------------------

#define VENT_SOLENOID 73

#endif
