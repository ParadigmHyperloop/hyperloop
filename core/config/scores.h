
#ifndef _OPENLOOP_POD_CONFIG_SCORES_
#define _OPENLOOP_POD_CONFIG_SCORES_

// Must have at least 50 read failures, each good read brings back down 1.
// A 10% fail rate is marked acceptable and will not be caught
#define IMU_SCORE_MAX 500
#define IMU_SCORE_STEP_UP 10
#define IMU_SCORE_STEP_DOWN 1

#define SKATE_SCORE_MAX 500
#define SKATE_SCORE_STEP_UP 10
#define SKATE_SCORE_STEP_DOWN 1

#define LATERAL_SCORE_MAX 500
#define LATERAL_SCORE_STEP_UP 10
#define LATERAL_SCORE_STEP_DOWN 1

#define ITERATION_TIME_ALPHA 0.01

#endif
