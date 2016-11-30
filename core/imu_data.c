#include <imu.h>
#include "pod.h"

int calcState(pod_value_t *a, pod_value_t *v, pod_value_t *x, float accel,
              double dt) {
  float acceleration = get_value_f(a);
  float velocity = get_value_f(v);
  float position = get_value_f(x);

  // Exponential Moving Average
  float new_accel =
      (1.0 - IMU_EMA_ALPHA) * acceleration + (IMU_EMA_ALPHA * accel);

  // Calculate the new_velocity (oldv + (olda + newa) / 2)

  // float dv = calcDu(dt, acceleration, new_accel);
  float dv = ((dt * new_accel) / USEC_PER_SEC);
  float new_velocity = (velocity + dv);

  // float dx = calcDu(dt, velocity, new_velocity);
  float dx = ((dt * new_velocity) / USEC_PER_SEC);
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
void add_imu_data(imu_datagram_t *data, pod_state_t *s) {
  if (!imu_valid(data)) {
    printf("NOT VALID: %X == %X; STAT: %X\n", data->computed_crc, data->crc, data->status);
    if (data->x > 0.0) {
      exit(101);
    }
    return;
  }

  static uint64_t last_imu_reading = 0;
  if (last_imu_reading == 0) {
    last_imu_reading = get_time();
    return;
  }

  uint64_t new_imu_reading = get_time();

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
