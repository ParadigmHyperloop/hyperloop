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

    // Distance sensors
    .pusher = {0},
    .levitation = {0},
    
    // Thermo
    .hp_thermo = 0.0,
    .reg_thermo = {0},
    .reg_surf_thermo = {0},
    .power_thermo = {0},
    .frame_thermo = 0.0,

    // batteries
    .voltages = {0},
    .currents = {0}
  };


  // Distance sensors
  for (i = 0; i < N_LEVITATION_DISTANCE; i++) {
    packet.pusher[i] = get_sensor(&(pod->pusher_plate_distance[i]));
  }

  for (i = 0; i < N_LEVITATION_DISTANCE; i++) {
    packet.levitation[i] = get_sensor(&(pod->levitation_distance[i]));
  }

  // Pressures
  packet.hp_pressure = get_sensor(&(pod->hp_pressure));

  for (i = 0; i < N_REG_PRESSURE; i++) {
    packet.reg_pressure[i] = get_sensor(&(pod->reg_pressure[i]));
  }

  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    packet.clamp_pressure[i] = get_sensor(&(pod->clamp_pressure[i]));
  }
  
  for (i = 0; i < N_CLAMP_PRESSURE; i++) {
    packet.brake_tank_pressure[i] = get_sensor(&(pod->brake_tank_pressure[i]));
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

#define emit_one(_key, _packet, _out) do { \
  _out(__XSTR__(_key), 0, 1, t->_key); \
} while (0)

#define emit_all(_key, _packet, _out) do { \
  for (size_t i = 0; i < sizeof(t->_key)/sizeof(*t->_key); i++) { \
    _out(__XSTR__(_key), i, sizeof(t->_key)/sizeof(*t->_key), t->_key[i]); \
  } \
} while (0)

void emit_telemetry(telemetry_packet_t *t, void (*outf)(char *key, size_t index, size_t total, float value)) {
  emit_one(version, t, outf);
  emit_one(size, t, outf);
  emit_one(state, t, outf);
  emit_one(solenoids, t, outf);
  emit_one(timestamp, t, outf);
  emit_one(position_x, t, outf);
  emit_one(position_y, t, outf);
  emit_one(position_z, t, outf);
  emit_one(velocity_x, t, outf);
  emit_one(velocity_y, t, outf);
  emit_one(velocity_z, t, outf);
  emit_one(acceleration_x, t, outf);
  emit_one(acceleration_y, t, outf);
  emit_one(acceleration_z, t, outf);
  
  emit_one(hp_pressure, t, outf);
  emit_all(reg_pressure, t, outf);
  emit_all(clamp_pressure, t, outf);
  emit_all(brake_tank_pressure, t, outf);

  emit_one(hp_thermo, t, outf);
  emit_all(reg_thermo, t, outf);
  emit_all(reg_surf_thermo, t, outf);
  emit_all(power_thermo, t, outf);
  emit_one(frame_thermo, t, outf);

  emit_all(voltages, t, outf);
  emit_all(currents, t, outf);
}

static bool emit_comma(bool open) {
  if (open) {
    printf(",");
  }
  return false;
}

static bool first = true;

static void emit_json(char *key, size_t index, size_t total, float value) {

  if (index == 0 && total == 1) {
    first = emit_comma(!first);
    printf("\"%s\":%f", key, value);
  } else if (total != 1) {
    first = emit_comma(!first);
    if (index == 0) {
      printf("\"%s\":[", key);
    }
    printf("%f", value);
    if (index == total - 1) {
      printf("]");
    }
  }
}

void dump_telemetry_file(const char *filename) {
  int fd = open(filename, O_RDONLY);

  if (fd < 0) {
    perror("Failed to open path: ");
    exit(1);
  }

  telemetry_packet_t *buf = malloc(sizeof(telemetry_packet_t));

  size_t count = 0;
  ssize_t rd;
  
  printf("[");
  while ((rd = read(fd, buf, sizeof(telemetry_packet_t))) == sizeof(telemetry_packet_t)) {
    if (count > 0) {
      printf(",");
    }
    printf("{");
    
    first = true;
    emit_telemetry(buf, emit_json);
    
    printf("}");
    count ++;
  }
  printf("]");
  
}
