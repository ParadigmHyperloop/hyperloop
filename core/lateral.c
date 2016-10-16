#include "pod.h"
/**
 * TODO: Do Not Need to do anything with this anymore
 */

uint32_t readLateralSensor(uint8_t sensorno) {
  //CHANGE ME! Read in value assume units in mm
  return 6;
}

int lateralRead(pod_state_t * state) {
    debug("Updateing lateral sensor readings");

    setPodField(&(state->lateral_front_left), readLateralSensor(0));
    setPodField(&(state->lateral_front_right), readLateralSensor(1));
    setPodField(&(state->lateral_rear_left), readLateralSensor(2));
    setPodField(&(state->lateral_rear_right), readLateralSensor(3));

    return 0;
}
