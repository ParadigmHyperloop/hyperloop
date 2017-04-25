/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#ifndef OPENLOOP_POD_H
#define OPENLOOP_POD_H

#include "cdefs.h"
#include "config.h"
#include <libBBB.h>
#include <pthread.h>
#include <sys/queue.h>
#include <hw.h>
#include <imu.h>
#include <log.h>
#include <stdatomic.h>
#include "realtime.h"
#include "states.h"
#include "telemetry.h"
#include "commander.h"
#include "core.h"
#include "panic.h"
#include "pod-helpers.h"
#include "ring_buffer.h"

/**
 * Sets the target flow through the skates on a scale of 0% to 100%
 */
int set_skate_target(int no, mpye_value_t val, bool override);

/**
 * Sets the caliper brake state
 */
int ensure_caliper_brakes(int no, solenoid_state_t val, bool override);

/**
 * Sets the clamp brake state
 */
int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override);

/**
 * Gets a bitmask representing the state of all the relays on the SSR board(s)
 */
relay_mask_t get_relay_mask(pod_t *pod);

/**
 * Performs a software self test on the pod's systems
 */
int self_tests(pod_t *pod);

/**
 * Computes the pod's new position, velocity, and acceleration vectors given
 * the IMU datagram
 */
void add_imu_data(imu_datagram_t *data, pod_t *s);

/**
 * Sends the given message to all logging destinations
 */
int pod_log(char *fmt, ...);

/**
 * Dump entire pod_t to the network logging buffer
 */
void log_dump(pod_t *pod);

/**
 * Create a human understandable text description of the current pod status
 *
 * @param pod A pod with data that you want a report of
 * @param buf The buffer to put the report in
 * @param len The length of buf
 *
 * @return The length of the report in bytes, or -1 on failure
 */
int status_dump(pod_t *pod, char *buf, size_t len);

/**
 * Initiates a halt of all threads
 */
int pod_shutdown(pod_t *pod);

/**
 * Starts all threads for the pod
 */
int pod_start(pod_t *pod);

/**
 * Initializes the GPIO pins on the BBB for use with the pod
 */
void setup_pins(pod_t *state);

/**
 * Calibrate sensors based on currently read values (zero out)
 */
void pod_calibrate(void);

/**
 * Reset positional and sensor data to blank slate
 */
void pod_reset(void);

/**
 * Shuts down the pod safely and performs any required cleanup actions
 */
void pod_exit(int code);

#endif
