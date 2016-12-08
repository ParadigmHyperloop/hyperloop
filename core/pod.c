/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#include "pod.h"



// returns the time in microseconds
uint64_t get_time() {
  struct timeval currentTime;

  assert(gettimeofday(&currentTime, NULL) == 0);

  return (currentTime.tv_sec * 1000000ULL) + currentTime.tv_usec;
}


void pod_calibrate() {
  pod_state_t *state = get_pod_state();

  set_value_f(&(state->imu_calibration_x), get_value_f(&(state->accel_x)));
  set_value_f(&(state->imu_calibration_y), get_value_f(&(state->accel_y)));
  set_value_f(&(state->imu_calibration_z), get_value_f(&(state->accel_z)));
}

void pod_reset() {
  pod_state_t *state = get_pod_state();

  set_value_f(&(state->accel_x), 0.0);
  set_value_f(&(state->accel_y), 0.0);
  set_value_f(&(state->accel_z), 0.0);
  set_value_f(&(state->velocity_x), 0.0);
  set_value_f(&(state->velocity_z), 0.0);
  set_value_f(&(state->velocity_y), 0.0);
  set_value_f(&(state->position_x), 0.0);
  set_value_f(&(state->position_y), 0.0);
  set_value_f(&(state->position_z), 0.0);
}

int pru_read_pack(sensor_pack_t *pack) {
    return -1;
}
