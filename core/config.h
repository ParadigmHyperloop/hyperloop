#ifndef _OPENLOOP_POD_CONFIG_
#define _OPENLOOP_POD_CONFIG_

// --------------------------
// PINS - List of pin numbers
// --------------------------
#define EBRAKE_PINS { 0, 1, 2 }

// --------------------------
// Device Constants
// --------------------------
#define N_SKATE_SOLONOIDS 6
#define N_SKATE_THERMOCOUPLES N_SKATE_SOLONOIDS

#define N_EBRAKE_SOLONOIDS 2
#define N_EBRAKE_PRESSURES N_EBRAKE_SOLONOIDS
#define N_EBRAKE_THERMOCOUPLES N_EBRAKE_SOLONOIDS

#define N_WHEEL_SOLONOIDS 3
#define N_WHEEL_PRESSURES N_WHEEL_SOLONOIDS
#define N_WHEEL_THERMOCOUPLES N_WHEEL_SOLONOIDS

#define N_LATERAL_SENSORS 4

// Taken from interface.c
#define COND 2
#define COUNT 4
#define PUSH_ACC 16
#define TRACK_LENGTH 100 // Meters
#define ACC_LENGTH 250
#define POD_MASS 750 // Kg
#define SIM_TIME 65
#define LOOP_DURATION 1000;

// Error Thresholds
#define A_ERR_X 1
#define A_ERR_Y 1
#define A_ERR_Z 1
#define V_ERR_X 1
#define V_ERR_Y 1
#define V_ERR_Z 1

// Signals
#define POD_SIGPANIC SIGUSR2

// -------------------------
// Subsystem Identifiers
// -------------------------

// The Main Thread
#define POD_MAIN_SUBSYSTEM 1

// The Core Processing Loop
#define POD_CORE_SUBSYSTEM 2

// The Command Server
#define POD_COMMAND_SUBSYSTEM 4

// The Logging Client
#define POD_LOGGING_SUBSYSTEM 8


// ----------------------
// Thread Sleep Intervals
// --------------------------------------------------------------------------
// Each thread is a loop, how long should the thread sleep for each iteration
// --------------------------------------------------------------------------

#define IMU_THREAD_SLEEP 5000
#define BRAKING_THREAD_SLEEP 5000
#define DISTANCE_THREAD_SLEEP 5000
#define KALMAN_THREAD_SLEEP 5000
#define LATERAL_THREAD_SLEEP 5000
#define LOGGING_THREAD_SLEEP 500000 // Half Second
#define PHOTOELECTRIC_THREAD_SLEEP 5000

// --------------
// Debug Printing
// --------------
#define output(prefix_, fmt_, ...) podLog((prefix_ "{" __FILE__ ":" __XSTR__(__LINE__) "} " fmt_ "\n"), ##__VA_ARGS__)
#define debug(fmt_, ...) output("[DEBUG] ", fmt_, ##__VA_ARGS__)
#define warn(fmt_, ...) output("[WARN] ", fmt_, ##__VA_ARGS__)
#define error(fmt_, ...) output("[ERROR] ", fmt_, ##__VA_ARGS__)
#define info(fmt_, ...) output("[INFO] ", fmt_, ##__VA_ARGS__)
#define fatal(fmt_, ...) output("[FATAL] ", fmt_, ##__VA_ARGS__)
#define panic(subsystem, notes, ...) podInterruptPanic(subsystem, __FILE__, __LINE__, notes, ##__VA_ARGS__)

// ------------------
// Primary Braking Thresholds
// ------------------

/// 0.8 G = 0.8 * 9.8 m/s/s * 1000 mm / m
#define PRIMARY_BRAKING_ACCEL_X_MIN -5880 // -0.6 G => mm/s/s
#define PRIMARY_BRAKING_ACCEL_X_NOM -7840 // -0.8 G => mm/s/s
#define PRIMARY_BRAKING_ACCEL_X_MAX -24500 // -2.5 G => mm/s/s

/// TODO: Need Real Values
/// NOMINAL: 100N /// REVIEW: Guess
#define PRIMARY_BRAKING_ENGAGED_MIN_F 80
#define PRIMARY_BRAKING_ENGAGED_NOM_F 100
#define PRIMARY_BRAKING_ENGAGED_MAX_F 150

// ------------------
// Emergency Braking Thresholds
// ------------------
/// NOMINAL: 1.2 G = 1.2 * 9.8 m/s/s * 1000 mm / m
#define EBRAKE_BRAKING_ACCEL_X_MIN -7840 // -0.8 G => mm/s/s
#define EBRAKE_BRAKING_ACCEL_X_NOM -11760 // -1.2 G => mm/s/s
#define EBRAKE_BRAKING_ACCEL_X_MAX -49000 // -5.0 G => mm/s/s

/// TODO: Need Real Values
/// NOMINAL: 1000N /// REVIEW: Guess
#define EBRAKE_ENGAGED_MIN_F 800
#define EBRAKE_ENGAGED_NOM_F 1000
#define EBRAKE_ENGAGED_MAX_F 1500

// ---------------------
// Logging Configuration
// ---------------------
#define LOG_FILE_PATH "./hyperloop-core.log"
#define LOG_FILE_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define MAX_PACKET_SIZE 1024
#define LOG_SVR_NAME "pod-server.openloop.com"
#define LOG_SVR_PORT 7778
#define MAX_LOG_LINE 4096

// ---------------
// Command Control
// ---------------
#define POD_CLI_VERSION "0.0.1-alpha"
#define POD_COPY_YEAR "2016"
#define CMD_SVR_PORT 7779

#define POD_ETOOMANYCLIENTS_TXT "Too Many Clients are connected"


// static buffer that is written to by each of the command functions
#define CMD_OUT_BUF 4096

#define MAX_CMD_CLIENTS 16

#endif
