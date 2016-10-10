#ifndef _OPENLOOP_POD_CONFIG_
#define _OPENLOOP_POD_CONFIG_

#define N_SKATE_SOLONOIDS 6
#define N_EBRAKE_SOLONOIDS 2
#define N_WHEEL_SOLONOIDS 3
#define N_LATERAL_SOLONOIDS 4

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
#define output(prefix_, fmt_, ...) podLog((prefix_ fmt_ "\n"), ##__VA_ARGS__)
#define debug(fmt_, ...) output("[DEBUG] ", fmt_, ##__VA_ARGS__)
#define warn(fmt_, ...) output("[WARN] ", fmt_, ##__VA_ARGS__)

// ------------------
// Braking Thresholds
// ------------------
#define PRIMARY_BRAKING_ACCEL_X_MIN -280
#define PRIMARY_BRAKING_ACCEL_X_MAX -40


// ---------------------
// Logging Configuration
// ---------------------
#define LOG_FILE_PATH "./hyperloop-core.log"
#define LOG_FILE_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define MAX_PACKET_SIZE 1024
#define LOG_SVR_NAME "pod-server.openloop.com"
#define LOG_SVR_PORT 7778

// ---------------
// Command Control
// ---------------
#define CMD_SVR_PORT 7779
#define CMD_EBRAKE "ebrake"

#define CMD_PING "ping"
#define CMD_PING_RES "PONG"

#define CMD_UNKNOWN_RES "UNKNOWN COMMAND"

#define MAX_CMD_CLIENTS 16

#endif
