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

#include "pod.h"

extern char *pod_mode_names[N_POD_STATES];

// Returns the current RealTime Clock value in microseconds
uint64_t get_time_usec() {
  struct timespec tc;
  clock_gettime(CLOCK_REALTIME, &tc);

  return (tc.tv_sec * USEC_PER_SEC) + (tc.tv_nsec / NSEC_PER_USEC);
}
void get_timespec(struct timespec *t) {
  int rc = clock_gettime(CLOCK_REALTIME, t);
  if (rc < 0) {
    DECLARE_EMERGENCY("Clock Failure: %d", rc);
  }
}

void timespec_add_us(struct timespec *t, long us) {
  t->tv_nsec += us * 1000;
  if (t->tv_nsec > 1000000000) {
    t->tv_nsec = t->tv_nsec - 1000000000; // + ms*1000000;
    t->tv_sec += 1;
  }
}

int timespec_cmp(struct timespec *a, struct timespec *b) {
  if (a->tv_sec > b->tv_sec)
    return 1;
  else if (a->tv_sec < b->tv_sec)
    return -1;
  else if (a->tv_sec == b->tv_sec) {
    if (a->tv_nsec > b->tv_nsec)
      return 1;
    else if (a->tv_nsec == b->tv_nsec)
      return 0;
    else
      return -1;
  }

  return -1;
}

int64_t timespec_to_nsec(struct timespec *t) {
  if (t->tv_sec >= (INT64_MAX - 1) / (long)NSEC_PER_SEC) {
    return -1;
  }

  return (t->tv_sec * NSEC_PER_SEC) + t->tv_nsec;
}

void pod_calibrate() {
  pod_t *pod = get_pod();

  set_value_f(&(pod->imu_calibration_x), get_value_f(&(pod->accel_x)));
  set_value_f(&(pod->imu_calibration_y), get_value_f(&(pod->accel_y)));
  set_value_f(&(pod->imu_calibration_z), get_value_f(&(pod->accel_z)));
}

void pod_reset() {
  pod_t *pod = get_pod();

  set_value_f(&(pod->accel_x), 0.0);
  set_value_f(&(pod->accel_y), 0.0);
  set_value_f(&(pod->accel_z), 0.0);
  set_value_f(&(pod->velocity_x), 0.0);
  set_value_f(&(pod->velocity_z), 0.0);
  set_value_f(&(pod->velocity_y), 0.0);
  set_value_f(&(pod->position_x), 0.0);
  set_value_f(&(pod->position_y), 0.0);
  set_value_f(&(pod->position_z), 0.0);
}


relay_mask_t get_relay_mask(pod_t *pod) {
  relay_mask_t mask = 0x0000;
  
  /* Ordered like pod->relays
   HP Fill Line
   LP Fill A
   Clamp Engage A
   Clamp Release A
   Skates Front
   Skates Rear
   Caliper Mid
   Lateral Control Fill A
   LP Vent
   LP Fill B
   Clamp Engage B
   Clamp Release B
   Skates Mid
   Caliper Front
   Caliper Back
   Lateral Control Fill B
   */
  
  int i;
  for (i = 0; i < N_RELAY_CHANNELS; i++) {
    if (is_solenoid_open(pod->relays[i])) {
      mask |= (0x01 << i);
    }
  }
  return mask;
}

telemetry_packet_t make_telemetry(pod_t *pod) {
  int i;
  
  telemetry_packet_t packet = {
    .version = TELEMETRY_PACKET_VERSION,
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
    .stripe_count = (uint16_t)get_value_f(&(pod->stripe_count))
  };
  
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

int status_dump(pod_t *pod, char *buf, size_t len) {
  int c = 0;
  int i = 0;
  
  c += snprintf(&buf[c], len, "mode: %s\n"
                "acl m/s/s: x: %f, y: %f, z: %f\n"
                "vel m/s  : x: %f, y: %f, z: %f\n"
                "pos m    : x: %f, y: %f, z: %f\n",
                pod_mode_names[get_pod_mode()], get_value_f(&(pod->accel_x)),
                get_value_f(&(pod->accel_y)), get_value_f(&(pod->accel_z)), get_value_f(&(pod->velocity_x)),
                get_value_f(&(pod->velocity_y)), get_value_f(&(pod->velocity_z)), get_value_f(&(pod->position_x)),
                get_value_f(&(pod->position_y)), get_value_f(&(pod->position_z)));
  
  c += snprintf(&buf[c], len - c, "Pusher Plate: \t%s\n",
                (get_value(&(pod->pusher_plate)) ? "ACTIVE" : "INACTIVE"));
  
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    c += snprintf(
                  &buf[c], len - c, "Skate %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->skate_solonoids[i])) ? "open" : "closed"));
  }
  
  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    c += snprintf(
                  &buf[c], len - c, "Caliper %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->wheel_solonoids[i])) ? "open" : "closed"));
  }
  
  for (i = 0; i < N_CLAMP_ENGAGE_SOLONOIDS; i++) {
    c += snprintf(&buf[c], len - c, "Clamp Eng %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->clamp_engage_solonoids[i]))
                   ? "open"
                   : "closed"));
  }
  
  for (i = 0; i < N_CLAMP_RELEASE_SOLONOIDS; i++) {
    c += snprintf(&buf[c], len - c, "Clamp Rel %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->clamp_release_solonoids[i]))
                   ? "open"
                   : "closed"));
  }
  
  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    c += snprintf(
                  &buf[c], len - c, "LP Fill %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->lp_fill_valve[i])) ? "open" : "closed"));
  }
  
  c += snprintf(&buf[c], len - c, "HP Fill:\t%s\n",
                (is_solenoid_open(&(pod->hp_fill_valve)) ? "open" : "closed"));
  
  c += snprintf(&buf[c], len - c, "LP Vent:\t%s\n",
                (is_solenoid_open(&(pod->vent_solenoid)) ? "open" : "closed"));
  
  sensor_t *s = NULL;
  size_t j;
  for (j=0;j<sizeof(pod->sensors)/sizeof(pod->sensors[0]);j++) {
    s = pod->sensors[j];
    if (s != NULL) {
      c += snprintf(&buf[c], len - c, "%s: \t%f\n", s->name, get_sensor(s));
    }
  }
  
  return c;
}



void log_dump(pod_t *pod) {
#ifdef POD_DEBUG
  note("Logging System -> Dumping");
  char s[8096];
  
  // Load up `s` with a textual status dump
  status_dump(pod, s, sizeof(s)/sizeof(s[0]));
  
  printf("%s", s);
#endif
  
  
  // Telemetry streaming
  static uint64_t last_packet = 0;
  
  if (get_time_usec() - last_packet > PACKET_INTERVAL) {
    last_packet = get_time_usec();
    telemetry_packet_t packet = make_telemetry(pod);
    log_t l = {.type = Packet, .data = {0}, .sz = sizeof(packet)};
    
    memcpy(l.data, &packet, l.sz);

    log_enqueue(&l);
  }
  
}
