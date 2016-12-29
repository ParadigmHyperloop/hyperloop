#include "pod.h"

int set_mux(pod_mux_t mux, uint8_t select) {
  assert(N_MUX_SELECT_PINS == 4);
  int i;
  for (i = 0; i < N_MUX_SELECT_PINS; i++) {
    if (setPinValue(mux.select_pins[i], select & (0x01 << i)) < 0) {
      return -1;
    }
  }
  return 0;
}
