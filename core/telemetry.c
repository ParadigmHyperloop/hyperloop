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

#include "telemetry.h"

telemetry_packet_t make_telemetry(pod_t *pod) {
  int i;
  // NOTE: This is mildly dangerous if a field is never initialized in this func
  telemetry_packet_t packet = {.version = TELEMETRY_PACKET_VERSION,
    .size = (uint16_t)sizeof(telemetry_packet_t),
    .state = get_pod_mode(),
    // Solenoids
    .solenoids = get_relay_mask(pod),
    .timestamp = get_time_usec(),
    // IMU
    .position_x = get_value_f(&(pod->position_x)),
    .position_y = get_value_f(&(pod->position_y)),
    .position_z = get_value_f(&(pod->position_z)),
    .velocity_x = get_value_f(&(pod->velocity_x)),
    .velocity_y = get_value_f(&(pod->velocity_y)),
    .velocity_z = get_value_f(&(pod->velocity_z)),
    .acceleration_x = get_value_f(&(pod->accel_x)),
    .acceleration_y = get_value_f(&(pod->accel_y)),
    .acceleration_z = get_value_f(&(pod->accel_z)),
    // PSI
    .hp_pressure = 0.0,
    .reg_pressure = {0},
    .clamp_pressure = {0},
    .skate_pressure = {0},
    .lateral_pressure = {0},
    
    // Distance sensors
    .corners = {0},
    .wheels = {0},
    .lateral = {0},
    
    // Thermo
    .hp_thermo = 0.0,
    .reg_thermo = {0},
    .reg_surf_thermo = {0},
    .power_thermo = {0},
    .frame_thermo = 0.0,
    
    // batteries
    .voltages = {0},
    .currents = {0},
    // Photo
    .rpms = {0},
    .stripe_count =
    (uint16_t)get_value_f(&(pod->stripe_count))};
  
  // Distance sensors
  for (i = 0; i < N_CORNER_DISTANCE; i++) {
    packet.corners[i] = get_sensor(&(pod->corner_distance[i]));
  }
  
  for (i = 0; i < N_WHEEL_DISTANCE; i++) {
    packet.wheels[i] = get_sensor(&(pod->wheel_distance[i]));
  }
  
  for (i = 0; i < N_LATERAL_DISTANCE; i++) {
    packet.lateral[i] = get_sensor(&(pod->lateral_distance[i]));
  }
  
  // Photo
  for (i = 0; i < N_WHEEL_PHOTO; i++) {
    packet.rpms[i] = get_value_f(&(pod->rpms[i]));
  }
  
  // Pressures
  packet.hp_pressure = get_sensor(&(pod->hp_pressure));
  
  for (i = 0; i < N_REG_PRESSURE; i++) {
    packet.reg_pressure[i] = get_sensor(&(pod->reg_pressure[i]));
  }
  
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    packet.clamp_pressure[i] = get_sensor(&(pod->clamp_pressure[i]));
  }
  
  for (i = 0; i < N_SKATE_PRESSURE; i++) {
    packet.skate_pressure[i] = get_sensor(&(pod->skate_pressure[i]));
  }
  
  for (i = 0; i < N_LAT_FILL_PRESSURE; i++) {
    packet.lateral_pressure[i] = get_sensor(&(pod->lateral_pressure[i]));
  }
  
  // Temperatures
  packet.hp_thermo = get_sensor(&(pod->hp_thermo));
  packet.frame_thermo = get_sensor(&(pod->frame_thermo));
  
  for (i = 0; i < N_REG_THERMO; i++) {
    packet.reg_thermo[i] = get_sensor(&(pod->reg_thermo[i]));
  }
  
  for (i = 0; i < N_REG_SURF_THERMO; i++) {
    packet.reg_surf_thermo[i] = get_sensor(&(pod->reg_surf_thermo[i]));
  }
  
  for (i = 0; i < N_POWER_THERMO; i++) {
    packet.power_thermo[i] = get_sensor(&(pod->power_thermo[i]));
  }
  
  for (i = 0; i < N_CLAMP_PAD_THERMO; i++) {
    packet.clamp_thermo[i] = get_sensor(&(pod->clamp_thermo[i]));
  }
  
  // Batteries
  for (i = 0; i < N_BATTERIES; i++) {
    packet.voltages[i] = get_sensor(&(pod->battery[i].voltage));
  }
  
  for (i = 0; i < N_BATTERIES; i++) {
    packet.currents[i] = get_sensor(&(pod->battery[i].current));
  }
  
  return packet;
}
