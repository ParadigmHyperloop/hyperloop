#include "../pod.h"

void pod_panic(int subsystem, char *file, int line, char *notes, ...) {
  static char msg[MAX_LOG_LINE];
  va_list arg;
  va_start(arg, notes);
  vsnprintf(&msg[0], MAX_LOG_LINE, notes, arg);
  va_end(arg);

  fprintf(stderr, "[PANIC] %s:%d -> %s\n", file, line, msg);

  kill(getpid(), POD_SIGPANIC);
  // Alternate
  // exit(POD_EX_PANIC);
}

void set_caution(pod_warning_t caution, pod_state_t *state) {
  state->cautions |= caution;
}

void set_warn(pod_warning_t warning, pod_state_t *state) {
  state->warnings |= warning;
}

void clear_caution(pod_warning_t caution, pod_state_t *state) {
  state->cautions &= ~(caution);
}

void clear_warn(pod_warning_t warning, pod_state_t *state) {
  state->warnings &= ~(warning);
}

bool has_caution(pod_caution_t caution, pod_state_t *state) {
  return ((state->cautions & caution) != PodCautionNone);
}

bool has_warning(pod_warning_t warning, pod_state_t *state) {
  return ((state->warnings & warning) != PodWarningNone);
}
