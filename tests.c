#include "pod.h"

void testIMUAccelerationComputations() {

}

int main() {
  initializePodState();
  pod_state_t * state = getPodState();

  testIMUAccelerationComputations();
}
