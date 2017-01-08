#ifndef _OPENLOOP_POD_CONFIG_AIRSUPPLY_
#define _OPENLOOP_POD_CONFIG_AIRSUPPLY_

// --------------------------
// Configuration Constants
// --------------------------
#define MIN_SAFE_PSIA -10
#define MAX_SAFE_PSIA 20

#define NOMINAL_HP_PSIA 1770
#define NOMINAL_MINI_TANK_PSIA 145

#define _SURF_THERMO_ERR 5
#define _LINE_THERMO_ERR 5
#define _HP_THERMO_ERR 5

// Presure Transducer Configs
#define _HP_PRESSURE_ERR 20
#define _LP_PRESSURE_ERR 5

// --------------------------
// HP Package
// --------------------------
#define N_HP_PRESSURE 1
#define HP_PRESSURE_MUX PRESSURE_MUX
#define HP_PRESSURE_INPUT 10
#define HP_PRESSURE_ERR _HP_PRESSURE_ERR

#define N_HP_THERMO 1
#define HP_THERMO_MUX THERMO_MUX_0
#define HP_THERMO_INPUT 4
#define HP_THERMO_ERR _HP_THERMO_ERR

// --------------------------
// LP Package
// --------------------------
#define LP_PRESSURE_ERR 5

#define N_REG_PRESSURE 4
#define REG_PRESSURE_MUX PRESSURE_MUX
#define REG_PRESSURE_INPUTS { 0, 1, 2, 3 }
#define REG_PRESSURE_ERR LP_PRESSURE_ERR

#define N_REG_THERMO 4
#define REG_THERMO_MUX THERMO_MUX_0
#define REG_THERMO_INPUTS { 0, 1, 2, 3 }
#define REG_THERMO_ERR 10

#define N_REG_SURF_THERMO 4
#define REG_SURF_THERMO_MUX THERMO_MUX_1
#define REG_SURF_THERMO_INPUTS { 0, 1, 2, 3 }
#define REG_SURF_THERMO_ERR SURF_THERMO_ERR

// --------------------------
// Skates
// --------------------------

// Front, Mid, Rear
#define N_SKATE_SOLONOIDS 3
#define SKATE_SOLENOIDS                                                        \
  { 76, 79, 74 }

// Mid Left and Mid Right
#define N_SKATE_PRESSURE 2
#define SKATE_PRESSURE_MUX PRESSURE_MUX
#define SKATE_PRESSURE_INPUTS { 8, 9 }
#define SKATE_PRESSURE_ERR LP_PRESSURE_ERR


#define N_MPYES 6
#define MPYE_I2C_DAC_ADDR { 0, 1, 2, 3, 4, 5 }

// --------------------------
// Clamp Brakes
// --------------------------

#define N_CLAMP_ENGAGE_SOLONOIDS 2
#define CLAMP_ENGAGE_SOLONOIDS                                                       \
  { 8, 75 }

#define N_CLAMP_RELEASE_SOLONOIDS 2
#define CLAMP_RELEASE_SOLONOIDS                                                       \
  { 78, 77 }

// Which solenoid (index) to use for primary braking
#define PRIMARY_BRAKING_CLAMP 1

#define N_CLAMP_PAD_THERMO 2
#define CLAMP_PAD_THERMO_MUX THERMO_MUX_1
#define CLAMP_PAD_THERMO_INPUTS { 4, 5 }

#define N_CLAMP_PRESSURE 2
#define CLAMP_PRESSURE_MUX PRESSURE_MUX
#define CLAMP_PRESSURE_INPUTS { 4, 5 }
#define CLAMP_PRESSURE_ERR _LP_PRESSURE_ERR

#define NOMINAL_CLAMP_BRAKING_ACCEL -8.134 // m/s^2 (-0.83 G)

// --------------------------
// Wheels
// --------------------------

#define N_WHEEL_SOLONOIDS 3
#define WHEEL_SOLONOIDS                                                        \
  { 80, 72, 81 }

// --------------------------
// HP Fill
// --------------------------

#define HP_FILL_SOLENOID 10

// --------------------------
// LP FILL
// --------------------------

#define N_LP_FILL_SOLENOIDS 2
#define LP_FILL_SOLENOIDS                                                      \
  { 9, 73 }

// --------------------------
// LP FILL
// --------------------------

#define N_LAT_FILL_SOLENOIDS 2
#define LAT_FILL_SOLENOIDS                                                     \
  { 70, 11 }

#define N_LAT_FILL_PRESSURE 2
#define LAT_FILL_PRESSURE_MUX PRESSURE_MUX
#define LAT_FILL_PRESSURE_INPUTS { 6, 7 }

// --------------------------
// Releif solenoid
// --------------------------

#define VENT_SOLENOID 71

#endif
