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

#ifndef OPENLOOP_POD_LOG_H
#define OPENLOOP_POD_LOG_H
#include "../pod.h"
#include "../pod-helpers.h"

#ifndef PACKET_INTERVAL
#define PACKET_INTERVAL (USEC_PER_SEC / 100) // 1000 Packets per second
#endif

#define TELEMETRY_PACKET_VERSION 2

typedef enum {
  Message = 1,
  Telemetry_float = 2,
  Telemetry_int32 = 3,
  Packet = 4
} log_type_t;

typedef struct {
  char name[64];
  float value;
} log_float_data_t;

typedef struct {
  char name[64];
  int32_t value;
} log_int32_data_t;

typedef uint16_t relay_mask_t;

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

typedef struct log {
  log_type_t type;
  union {
    char message[MAX_LOG_LINE];
    log_float_data_t float_data;
    log_int32_data_t int32_data;
    telemetry_packet_t packet;
  } v;
  STAILQ_ENTRY(log) entries;
} log_t;

/**
 * Sends the given message to all logging destinations
 */
int pod_log(char *fmt, ...);

/**
 * Dump entire pod_t to the network logging buffer
 */
void log_dump(pod_t *pod);

/**
 * Enqueue a telemetry packet for network transmission of the current state
 */
int log_enqueue(log_t *l);


int status_dump(pod_t *pod, char *buf, size_t len);

#endif
