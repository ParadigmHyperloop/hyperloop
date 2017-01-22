#include "../pod.h"
#include "../pod-helpers.h"

// Log Priority telemetry
log_t logbuf_data[LOG_BUF_SIZE];
ring_buf_t logbuf;

extern char *pod_mode_names[N_POD_STATES];

int logTelemetry_f(char *name, float f) {
  log_t l = {.type = Telemetry_float,
             .v = {.float_data = {.name = {0}, .value = f}}};
  snprintf(&l.v.float_data.name[0], 64, "%s", name);
  return 0; // logEnqueue(&l);
}

int logTelemetry(char *name, int32_t i) {
  log_t l = {.type = Telemetry_int32,
             .v = {.int32_data = {.name = {0}, .value = i}}};
  snprintf(&l.v.int32_data.name[0], 64, "%s", name);
  return 0; // logEnqueue(&l);
}

int podLog(char *fmt, ...) {
  va_list arg;
  log_t l = {.type = Message, .v = {{0}}};

  char *msg = l.v.message;
  /* Write the error message */
  va_start(arg, fmt);
  vsnprintf(msg, MAX_LOG_LINE, fmt, arg);
  va_end(arg);

  printf("%s", msg);
  fflush(stdout);

  static FILE *log_file = NULL;

  if (log_file == NULL) {
    log_file = fopen(LOG_FILE_PATH, "a+");

    if (!log_file) {
      fprintf(stderr, "Failed to Open Log File: " LOG_FILE_PATH);
    }
  } else {
    fprintf(log_file, "%s", msg);
    fflush(log_file);
    fsync(fileno(log_file));
  }

  return 0; // logEnqueue(&l);
}

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
solenoid_mask_t get_solenoid_mask(pod_t *pod) {
  solenoid_mask_t mask = 0x0000;

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

  telemetry_packet_t packet = {.version = 1,
                               .length = sizeof(telemetry_packet_t),
                               .timestamp = get_time(),
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

                               // Distance sensors
                               .corners = {0},
                               .wheels = {0},
                               .lateral = {0},

                               // Photo
                               .rpms = {0},
                               .stripe_count =
                                   get_value_f(&(pod->stripe_count)),

                               // Solenoids
                               .solenoids = get_solenoid_mask(pod),

                               // state
                               .state = get_pod_mode()};

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
    packet.power_thermo[i] = get_sensor(&(pod->battery[i].temperature));
  }

  return packet;
}

void logDump(pod_t *pod) {
  note("Logging System -> Dumping");

  note("mode: %s, ready: %d", pod_mode_names[get_pod_mode()],
       get_value(&(pod->ready)));

  note("acl m/s/s: x: %f, y: %f, z: %f", get_value_f(&(pod->accel_x)),
       get_value_f(&(pod->accel_y)), get_value_f(&(pod->accel_z)));

  note("vel m/s  : x: %f, y: %f, z: %f", get_value_f(&(pod->velocity_x)),
       get_value_f(&(pod->velocity_y)), get_value_f(&(pod->velocity_z)));

  note("pos m    : x: %f, y: %f, z: %f", get_value_f(&(pod->position_x)),
       get_value_f(&(pod->position_y)), get_value_f(&(pod->position_z)));

#ifndef PACKET_INTERVAL
#define PACKET_INTERVAL USEC_PER_SEC / 10 // 0.1 seconds in usec
#endif

  // Telemetry streaming
  static uint64_t last_packet = 0;

  if (last_packet == 0) {
    last_packet = get_time();
  }

  if (get_time() - last_packet > PACKET_INTERVAL) {
    telemetry_packet_t packet = make_telemetry(pod);
    log_t l = {.type = Packet, .v = {.packet = packet}};
    logEnqueue(&l);
  }
}

int logEnqueue(log_t *l) {
  if (!logbuf.initialized) {
    ring_buf_init(&logbuf, &logbuf_data, LOG_BUF_SIZE, sizeof(log_t));
  }

  ring_buf_append(l, &logbuf);
  return 0;
}
