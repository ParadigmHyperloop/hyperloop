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
#include <imu.h>

int calcState(pod_value_t *a, pod_value_t *v, pod_value_t *x, float accel,
              double dt) {
  float acceleration = get_value_f(a);
  float velocity = get_value_f(v);
  float position = get_value_f(x);

  if (OUTSIDE(-10.0, accel, 10.0)) {
    warn("Value out of range for IMU (%f)", accel);
    return -1;
  }

  // Exponential Moving Average
  float new_accel =
      (1.0f - IMU_EMA_ALPHA) * acceleration + (IMU_EMA_ALPHA * accel);
  debug("RAW %f, old: %f, filtered %f, ema: %f", accel, acceleration, new_accel,
        IMU_EMA_ALPHA);
  // Calculate the new_velocity (oldv + (olda + newa) / 2)

  // float dv = calcDu(dt, acceleration, new_accel);
  float dv = (float)((dt * new_accel) / USEC_PER_SEC);
  float new_velocity = (velocity + dv);

  // float dx = calcDu(dt, velocity, new_velocity);
  float dx = (float)((dt * new_velocity) / USEC_PER_SEC);
  float new_position = (position + dx);

  // debug("dt: %lf us, dv: %f m/s, dx: %f m", dt, dv, dx);

  set_value_f(a, new_accel);
  set_value_f(v, new_velocity);
  set_value_f(x, new_position);

  return 0;
}

/**
 * Take a new IMU datagram and use it to update the x, y, and z positions
 * of the pod
 */
void add_imu_data(imu_datagram_t *data, pod_t *s) {

  static uint64_t last_imu_reading = 0;
  if (last_imu_reading == 0) {
    last_imu_reading = get_time_usec();
    return;
  }

  if (!imu_valid(data)) {
    warn("IMU INVALID! CRC:%X EXPECTED:%X; STAT:%X\n", data->crc,
         data->computed_crc, data->status);
    return;
  }

  uint64_t new_imu_reading = get_time_usec();

  uint64_t dt = new_imu_reading - last_imu_reading;

  if (dt == 0) {
    return;
  }

  last_imu_reading = new_imu_reading;

  float x = data->x + get_value_f(&(s->imu_calibration_x));
  float y = data->y + get_value_f(&(s->imu_calibration_y));
  float z = data->z + get_value_f(&(s->imu_calibration_z));

  calcState(&(s->accel_x), &(s->velocity_x), &(s->position_x), x, dt);
  calcState(&(s->accel_y), &(s->velocity_y), &(s->position_y), y, dt);
  calcState(&(s->accel_z), &(s->velocity_z), &(s->position_z), z, dt);
}
