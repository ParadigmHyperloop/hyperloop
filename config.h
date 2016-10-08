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

#define output(prefix_, fmt_, ...) printf((prefix_ fmt_ "\n"), ##__VA_ARGS__)
#define debug(fmt_, ...) output("[DEBUG] ", fmt_, ##__VA_ARGS__)
#define warn(fmt_, ...) output("[WARN] ", fmt_, ##__VA_ARGS__)

#endif
