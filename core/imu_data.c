#include <imu.h>
#include "pod.h"

int calcState(pod_value_t *a, pod_value_t *v, pod_value_t *x, float accel,
              double dt) {
  float acceleration = getPodField_f(a);
  float velocity = getPodField_f(v);
  float position = getPodField_f(x);

  // Exponential Moving Average
  float new_accel =
      (1.0 - IMU_EMA_ALPHA) * acceleration + IMU_EMA_ALPHA * accel;

  // Calculate the new_velocity (oldv + (olda + newa) / 2)

  // float dv = calcDu(dt, acceleration, new_accel);
  float dv = ((dt * new_accel) / USEC_PER_SEC);
  float new_velocity = (velocity + dv);

  // float dx = calcDu(dt, velocity, new_velocity);
  float dx = ((dt * new_velocity) / USEC_PER_SEC);
  float new_position = (position + dx);

  // debug("dt: %lf us, dv: %f m/s, dx: %f m", dt, dv, dx);

  setPodField_f(a, new_accel);
  setPodField_f(v, new_velocity);
  setPodField_f(x, new_position);

  return 0;
}

/**
 * Take a new IMU datagram and use it to update the x, y, and z positions
 * of the pod
 */
void add_imu_data(imu_datagram_t *data, pod_state_t *s) {
  static uint64_t last_imu_reading = 0;
  if (last_imu_reading == 0) {
    last_imu_reading = getTime();
    return;
  }

  uint64_t new_imu_reading = getTime();
  uint64_t dt = new_imu_reading - last_imu_reading;

  if (dt == 0) {
    return;
  }

  calcState(&(s->accel_x), &(s->velocity_x), &(s->position_x), data->x, dt);
  calcState(&(s->accel_y), &(s->velocity_y), &(s->position_y), data->y, dt);
  calcState(&(s->accel_z), &(s->velocity_z), &(s->position_z), data->z, dt);
}
