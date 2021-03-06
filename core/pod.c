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

void pod_calibrate() {
  pod_t *pod = get_pod();

  set_value_f(&(pod->imu_calibration_x), -get_value_f(&(pod->accel_x)));
  set_value_f(&(pod->imu_calibration_y), -get_value_f(&(pod->accel_y)));
  set_value_f(&(pod->imu_calibration_z), -get_value_f(&(pod->accel_z)));

  set_value_f(&(pod->velocity_x), 0.0f);
  set_value_f(&(pod->velocity_y), 0.0f);
  set_value_f(&(pod->velocity_z), 0.0f);

  set_value_f(&(pod->position_x), 0.0f);
  set_value_f(&(pod->position_y), 0.0f);
  set_value_f(&(pod->position_z), 0.0f);
}

bool pod_reset() {
  pod_t *pod = get_pod();
  if (set_pod_mode(Shutdown, "Pod Reset Requested")) {
    pod->shutdown = WarmReboot;

    pod_shutdown(pod);
    return true;
  }
  return false;
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

  mask = 0x0000;

  mask |= ((is_solenoid_open(&pod->skate_solenoids[0]) & 0x1) << 0);
  mask |= ((is_solenoid_open(&pod->skate_solenoids[1]) & 0x1) << 1);
  mask |= ((is_solenoid_open(&pod->skate_solenoids[2]) & 0x1) << 2);
  mask |= ((is_solenoid_open(&pod->skate_solenoids[3]) & 0x1) << 3);

  mask |= ((is_solenoid_open(&pod->clamp_engage_solenoids[0]) & 0x1) << 4);
  mask |= ((is_solenoid_open(&pod->clamp_release_solenoids[0]) & 0x1) << 5);
  mask |= ((is_solenoid_open(&pod->clamp_engage_solenoids[1]) & 0x1) << 6);
  mask |= ((is_solenoid_open(&pod->clamp_release_solenoids[1]) & 0x1) << 7);

  mask |= ((is_solenoid_open(&pod->hp_fill_valve) & 0x1) << 8);
  mask |= ((is_solenoid_open(&pod->vent_solenoid) & 0x1) << 9);
  mask |= ((get_value(&pod->pusher_plate) & 0x1) << 10);

  return mask;
}

int status_dump(pod_t *pod, char *buf, size_t len) {
  int c = 0;
  int i = 0;
  char reason[MAX_STATE_REASON_MSG];

  pthread_rwlock_rdlock(&(pod->mode_mutex));
  strncpy(reason, pod->state_reason, MAX_STATE_REASON_MSG - 1);
  pthread_rwlock_unlock(&(pod->mode_mutex));

  c +=
      snprintf(&buf[c], len, "mode: %s\nreason: %s\n"
                             "acl m/s/s: x: %f, y: %f, z: %f\n"
                             "vel m/s  : x: %f, y: %f, z: %f\n"
                             "pos m    : x: %f, y: %f, z: %f\n",
               pod_mode_names[get_pod_mode()], reason,
               get_value_f(&(pod->accel_x)), get_value_f(&(pod->accel_y)),
               get_value_f(&(pod->accel_z)), get_value_f(&(pod->velocity_x)),
               get_value_f(&(pod->velocity_y)), get_value_f(&(pod->velocity_z)),
               get_value_f(&(pod->position_x)), get_value_f(&(pod->position_y)),
               get_value_f(&(pod->position_z)));

  c += snprintf(&buf[c], len - c, "Pusher Plate: \t%s\n",
                (get_value(&(pod->pusher_plate)) ? "ACTIVE" : "INACTIVE"));

  for (i = 0; i < N_SKATE_SOLENOIDS; i++) {
    c += snprintf(
        &buf[c], len - c, "Skate %d:\t%s\n", i,
        (is_solenoid_open(&(pod->skate_solenoids[i])) ? "open" : "closed"));
  }

  for (i = 0; i < N_CLAMP_ENGAGE_SOLENOIDS; i++) {
    c += snprintf(&buf[c], len - c, "Clamp Eng %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->clamp_engage_solenoids[i]))
                       ? "open"
                       : "closed"));
  }

  for (i = 0; i < N_CLAMP_RELEASE_SOLENOIDS; i++) {
    c += snprintf(&buf[c], len - c, "Clamp Rel %d:\t%s\n", i,
                  (is_solenoid_open(&(pod->clamp_release_solenoids[i]))
                       ? "open"
                       : "closed"));
  }

  c += snprintf(&buf[c], len - c, "HP Fill:\t%s\n",
                (is_solenoid_open(&(pod->hp_fill_valve)) ? "open" : "closed"));

  c += snprintf(&buf[c], len - c, "LP Vent:\t%s\n",
                (is_solenoid_open(&(pod->vent_solenoid)) ? "open" : "closed"));

  sensor_t *s = NULL;
  size_t j;
  for (j = 0; j < sizeof(pod->sensors) / sizeof(pod->sensors[0]); j++) {
    s = pod->sensors[j];
    if (s != NULL) {
      c += snprintf(&buf[c], len - c, "%s: \t%f (%p)\n", s->name, get_sensor(s),
                    (void *)s);
    }
  }

  return c;
}

void log_dump(pod_t *pod) {
#ifdef POD_DEBUG
// note("Logging System -> Dumping");
// char s[8096];

// Load up `s` with a textual status dump
// status_dump(pod, s, sizeof(s) / sizeof(s[0]));

// printf("%s", s);
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
