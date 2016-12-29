
#ifndef _OPENLOOP_POD_CONFIG_INVENTORY_
#define _OPENLOOP_POD_CONFIG_INVENTORY_

// --------------------------
// Relay
// --------------------------
#define N_RELAY_CHANNELS 16
#define RELAY_I2C_ADDRESS 5

// --------------------------
// HP Package
// --------------------------
#define N_HP_TRANSDUCERS 1
#define HP_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_HP_THERMOCOUPLES 1
#define HP_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_HP_RELIEF 1
#define HP_RELIEF_PINS { 15 }

// --------------------------
// LP Package
// --------------------------
#define N_LP_TRANSDUCERS 4
#define LP_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_LP_THERMOCOUPLES 4
#define LP_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_LP_REGULATOR_THERMOCOUPLES 4
#define LP_REGULATOR_THERMOCOUPLES_BASE (PRU_BASE + 0)

// --------------------------
// Skates
// --------------------------

#define N_SKATE_SOLONOIDS 6
#define SKATE_SOLENOIDS { 10, 9, 8, 78, 76, 74 }

#define N_SKATE_TRANSDUCERS 6
#define SKATE_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_MPYES 6
#define MPYE_I2C_DAC_ADDR 14

// --------------------------
// Emergency Brakes
// --------------------------

#define N_EBRAKE_SOLONOIDS 2
#define EBRAKE_SOLONOIDS { 72, 70 }

#define N_EBRAKE_PAD_THERMOCOUPLES 2
#define EBRAKE_PAD_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_EBRAKE_LINE_THERMOCOUPLES 2
#define EBRAKE_LINE_THERMOCOUPLES_BASE (PRU_BASE + 0)

// --------------------------
// Wheels
// --------------------------

#define N_WHEEL_SOLONOIDS 3
#define WHEEL_SOLONOIDS { 11, 81, 80 }

#define N_WHEEL_THERMOCOUPLES 3
#define WHEEL_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_WHEEL_PHOTODIODES 3
#define WHEEL_PHOTODIODES_BASE (PRU_BASE + 0)

// --------------------------
// Lateral
// --------------------------

#define N_LATERAL_SENSORS 6
#define LATERAL_SENSORS_BASE (PRU_BASE + 0)

// --------------------------
// OMRONS
// --------------------------

#define N_WHEEL_OMRONS 3
#define WHEEL_OMRONS_BASE (PRU_BASE + 0)

#define N_SKATE_OMRONS 4
#define SKATE_OMRONS_BASE (PRU_BASE + 0)

// --------------------------
// Shell
// --------------------------

#define N_SHELL_TRANSDUCERS 3
#define SHELL_TRANSDUCERS_BASE (PRU_BASE + 0)

#define N_SHELL_THERMOCOUPLES 4
#define SHELL_THERMOCOUPLES_BASE (PRU_BASE + 0)

#define N_SHELL_PHOTODIODES 6
#define SHELL_PHOTODIODES_BASE (PRU_BASE + 0)

// --------------------------
// Power Board
// --------------------------

#define N_POWER_THERMOCOUPLES 4
#define POWER_THERMOCOUPLES_BASE (PRU_BASE + 0)


// --------------------------
// Batteries
// --------------------------

#define N_BATTERIES 4
#define BATTERY_I2C_ADDRESSES { 6, 7, 8, 9 }


// --------------------------
// HP Fill
// --------------------------

#define HP_FILL_SOLENOID 79

// --------------------------
// LP FILL
// --------------------------

#define N_LP_FILL_SOLENOIDS 2
#define LP_FILL { 77, 75 }

// --------------------------
// Releif solenoid
// --------------------------

#define RELEIF_SOLENOID 73

// --------------------------
// MUX
// --------------------------

#define N_MUXES 4
#define N_MUX_SELECT_PINS 4
#define MUX_SELECT_PINS { 45, 44, 117, 125 }


#endif
