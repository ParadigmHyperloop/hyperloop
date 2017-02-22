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
#include "libBBB.h"
#include <pthread.h>
#include <sys/queue.h>

#include <int.h>
#include <imu.h>
#include <log.h>

#include "pod/states.h"
// proprietary libimu header

#include "core.h"
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
uint64_t get_time_usec(void);

void get_timespec(struct timespec *t);

void timespec_add_us(struct timespec *t, long us);

int timespec_cmp(struct timespec *a, struct timespec *b);

int64_t timespec_to_nsec(struct timespec *t);

/**
 * Calibrate sensors based on currently read values (zero out)
 */
void pod_calibrate(void);

/**
 * Reset positional and sensor data to blank slate
 */
void pod_reset(void);

void pod_exit(int code);

int set_skate_target(int no, solenoid_state_t val, bool override);
int ensure_caliper_brakes(int no, solenoid_state_t val, bool override);
int ensure_clamp_brakes(int no, clamp_brake_state_t val, bool override);

int setBrakes(int no, int val, bool override);
int setEBrakes(int no, int val, bool override);

int self_tests(pod_t *pod);

void add_imu_data(imu_datagram_t *data, pod_t *s);
void setup_pins(pod_t *state);


// TODO: Circle Back to this, not sure why this struct isn't aligning correctly
#pragma pack(1)
typedef struct telemetry_packet {
  uint8_t version;
  uint16_t size;
  // state
  uint8_t state;
  // Solenoids
  uint32_t solenoids;
  uint64_t timestamp;
  // IMU
  float position_x;
  float position_y;
  float position_z;
  
  float velocity_x;
  float velocity_y;
  float velocity_z;
  
  float acceleration_x;
  float acceleration_y;
  float acceleration_z;
  
  // Distance sensors
  float corners[N_CORNER_DISTANCE];                // 4
  float wheels[N_WHEEL_DISTANCE];                  // 3
  float lateral[N_LATERAL_DISTANCE];               // 3
  
  // Pressures
  float hp_pressure;                               // 1
  float reg_pressure[N_REG_PRESSURE];              // 4
  float clamp_pressure[N_CLAMP_PRESSURE];          // 2
  float skate_pressure[N_SKATE_PRESSURE];          // 2
  float lateral_pressure[N_LAT_FILL_PRESSURE];     // 2
  
  // Thermocouples
  float hp_thermo;                                 // 1
  float reg_thermo[N_REG_THERMO];                  // 4
  float reg_surf_thermo[N_REG_SURF_THERMO];        // 4
  float power_thermo[N_POWER_THERMO];              // 4
  float clamp_thermo[N_CLAMP_PAD_THERMO];          // 2
  float frame_thermo;                              // 1
  
  // Batteries
  float voltages[N_BATTERIES];                     // 3
  float currents[N_BATTERIES];                     // 3
  
  // Photo
  float rpms[N_WHEEL_PHOTO];                       // 3
  uint32_t stripe_count;
} telemetry_packet_t;

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

#endif
