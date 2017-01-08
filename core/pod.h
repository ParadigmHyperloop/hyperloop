/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#ifndef OPENLOOP_POD_H
#define OPENLOOP_POD_H

#include "cdefs.h"
#include "libBBB.h"
#include "config.h"
#include "core.h"
#include <pthread.h>
#include <sys/queue.h>

// proprietary libimu header
#include <imu.h>
#include "pod/states.h"
#include "pod/log.h"
#include "pod/emergency.h"
#include "ring_buffer.h"
/**
 * Get the current time of the pod in microseconds
 *
 * TODO: Make this function return nanosecond precision
 * TODO: Make this function return a uint64_t
 * TODO: Make this function actually specify the timezone/base of the timestamp
 *
 * @return The current timestamp in microseconds
 */
uint64_t get_time(void);

/**
 * Calibrate sensors based on currently read values (zero out)
 */
void pod_calibrate(void);

/**
 * Reset positional and sensor data to blank slate
 */
void pod_reset(void);

int pru_read_pack(sensor_pack_t *pack);

void pod_exit(int code);

int set_skate_target(int no, solenoid_state_t val, bool override);
int ensure_caliper_brakes(int no, solenoid_state_t val, bool override);
int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override);

int setBrakes(int no, int val, bool override);
int setEBrakes(int no, int val, bool override);

int self_tests(pod_t *pod);

void add_imu_data(imu_datagram_t *data, pod_t *s);
void setup_pins(pod_t *state);
#endif
