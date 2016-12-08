#include "../pod.h"

// Log Priority telemetry
log_t logbuf_data[LOG_BUF_SIZE];
ring_buf_t logbuf;

extern char *pod_mode_names[N_POD_STATES];

int logTelemetry_f(char *name, float f) {
  log_t l = {.type = Telemetry_float,
             .v = {.float_data = {.name = {0}, .value = f}}};
  snprintf(&l.v.float_data.name[0], 64, "%s", name);
  return logEnqueue(&l);
}

int logTelemetry(char *name, int32_t i) {
  log_t l = {.type = Telemetry_int32,
             .v = {.int32_data = {.name = {0}, .value = i}}};
  snprintf(&l.v.int32_data.name[0], 64, "%s", name);
  return logEnqueue(&l);
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

  return logEnqueue(&l);
}

void logDump(pod_state_t *state) {
  note("Logging System -> Dumping");

  note("mode: %s, ready: %d", pod_mode_names[get_pod_mode()],
       get_value(&(state->ready)));

  note("acl m/s/s: x: %f, y: %f, z: %f", get_value_f(&(state->accel_x)),
       get_value_f(&(state->accel_y)), get_value_f(&(state->accel_z)));

  note("vel m/s  : x: %f, y: %f, z: %f", get_value_f(&(state->velocity_x)),
       get_value_f(&(state->velocity_y)), get_value_f(&(state->velocity_z)));

  note("pos m    : x: %f, y: %f, z: %f", get_value_f(&(state->position_x)),
       get_value_f(&(state->position_y)), get_value_f(&(state->position_z)));

  note("skates   : %d", state->tmp_skates);
  note("brakes   : %d", state->tmp_brakes);

  // Send Telemetry
  logTelemetry_f("accel_x", get_value_f(&(state->accel_x)));
  logTelemetry_f("accel_y", get_value_f(&(state->accel_y)));
  logTelemetry_f("accel_z", get_value_f(&(state->accel_z)));

  logTelemetry_f("velocity_x", get_value_f(&(state->velocity_x)));
  logTelemetry_f("velocity_y", get_value_f(&(state->velocity_y)));
  logTelemetry_f("velocity_z", get_value_f(&(state->velocity_z)));

  logTelemetry_f("position_x", get_value_f(&(state->position_x)));
  logTelemetry_f("position_y", get_value_f(&(state->position_y)));
  logTelemetry_f("position_z", get_value_f(&(state->position_z)));

  logTelemetry("skates", state->tmp_skates);
  logTelemetry("brakes", state->tmp_brakes);
}


int logEnqueue(log_t *l) {
  if (!logbuf.initialized) {
    ring_buf_init(&logbuf, &logbuf_data, LOG_BUF_SIZE, sizeof(log_t));
  }

  ring_buf_append(l, &logbuf);
  return 0;
}
