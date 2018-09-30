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

#ifndef PARADIGM_TELEMETRY_H
#define PARADIGM_TELEMETRY_H

#include "pod.h"

// TODO: Circle Back to this, not sure why this struct isn't aligning correctly
typedef struct __attribute__((__packed__)) telemetry_packet {
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
    float pusher[N_PUSHER_DISTANCE];         // 4
    float levitation[N_LEVITATION_DISTANCE]; // 8

    // Pressures
    float hp_pressure;                                // 1
    float reg_pressure[N_REG_PRESSURE];               // 4
    float clamp_pressure[N_CLAMP_PRESSURE];           // 2
    float brake_tank_pressure[N_BRAKE_TANK_PRESSURE]; // 2

    // Thermocouples
    float hp_thermo;                          // 1
    float reg_thermo[N_REG_THERMO];           // 4
    float reg_surf_thermo[N_REG_SURF_THERMO]; // 4
    float power_thermo[N_POWER_THERMO];       // 4
    float clamp_thermo[N_CLAMP_PAD_THERMO];   // 2
    float frame_thermo;                       // 1

    // Batteries
    float voltages[N_BATTERIES]; // 3
    float currents[N_BATTERIES]; // 3

} telemetry_packet_t;

/**
 * Build a telemetry_packet_t using the given pod_t
 *
 * @param pod A pointer to the pod structure to build the telemetry packet from
 * @returns A telemetry_packet_t which can be directly dumpped into a UDP body
 */
telemetry_packet_t make_telemetry(pod_t *pod);

/**
 * Walk through all the fields in the given telemetry_packet_t and call the
 * given callback function with information about the field name, and it's
 * value.
 *
 * @param t A pointer to the telemetry packet to emit
 * @param outf A pointer to a function that will emit a telemetry value
 */
void emit_telemetry(telemetry_packet_t *t,
                    void (*outf)(char *key, size_t index, size_t total,
                                 float value));

void dump_telemetry_file(const char *filename);

#endif
