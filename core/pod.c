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

int pru_read_pack(sensor_pack_t *pack) { return -1; }
