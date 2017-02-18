#include "pod.h"
#include <imu.h>
#include <math.h>

void integrate_imu(float timestep, const float acc[3], float vel[3], float pos[3],
    const float rotvel[3], float quat[4])
{
  float dv2[16];
  float dv3[16];
  int k;
  float dv4[4];
  int i0;
  float y[4];
  float b_y;
  float b_quat;
  float normQuat;
  float dv5[9];
  float dv6[3];
  float d0;
  static const signed char iv0[3] = { 0, 0, 1 };

  /*  numerical integration of quaternions */

  // dv2 is the omega matrix for quaternion update
  // q = q + .5*timestep*omega*q
  dv2[0] = 0.0;
  dv2[4] = rotvel[2];
  dv2[8] = -rotvel[1];
  dv2[12] = rotvel[0];
  dv2[1] = -rotvel[2];
  dv2[5] = 0.0;
  dv2[9] = rotvel[0];
  dv2[13] = rotvel[1];
  dv2[2] = rotvel[1];
  dv2[6] = -rotvel[0];
  dv2[10] = 0.0;
  dv2[14] = rotvel[2];
  dv2[3] = -rotvel[0];
  dv2[7] = -rotvel[1];
  dv2[11] = -rotvel[2];
  dv2[15] = 0.0;
  for (k = 0; k < 4; k++) {
    for (i0 = 0; i0 < 4; i0++) {
      dv3[i0 + (k << 2)] = 0.5 * dv2[i0 + (k << 2)];
    }
  }

  for (k = 0; k < 4; k++) {
    dv4[k] = 0.0;
    for (i0 = 0; i0 < 4; i0++) {
      dv4[k] += dv3[k + (i0 << 2)] * quat[i0];
    }
  }

  /*  normalize - the quaternion vector must have magniturde 1 */
  for (k = 0; k < 4; k++) {
    b_quat = quat[k] + timestep * dv4[k];
    y[k] = b_quat * b_quat;
    quat[k] = b_quat;
  }

  b_y = y[0];
  for (k = 0; k < 3; k++) {
    b_y += y[k + 1];
  }

  normQuat = sqrt(b_y);
  for (k = 0; k < 4; k++) {
    quat[k] /= normQuat;
  }

  b_y = timestep * 9.8;
  dv5[0] = (1.0 - 2.0 * (quat[2] * quat[2])) - 2.0 * (quat[3] * quat[3]);
  dv5[3] = 2.0 * (quat[1] * quat[2] - quat[0] * quat[3]);
  dv5[6] = 2.0 * (quat[1] * quat[3] + quat[0] * quat[2]);
  dv5[1] = 2.0 * (quat[1] * quat[2] + quat[0] * quat[3]);
  dv5[4] = (1.0 - 2.0 * (quat[1] * quat[1])) - 2.0 * (quat[3] * quat[3]);
  dv5[7] = 2.0 * (quat[2] * quat[3] - quat[0] * quat[1]);
  dv5[2] = 2.0 * (quat[1] * quat[3] - quat[0] * quat[2]);
  dv5[5] = 2.0 * (quat[2] * quat[3] + quat[0] * quat[1]);
  dv5[8] = (1.0 - 2.0 * (quat[1] * quat[1])) - 2.0 * (quat[2] * quat[2]);
  for (k = 0; k < 3; k++) {
    d0 = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      d0 += dv5[k + 3 * i0] * acc[i0];
    }

    dv6[k] = d0 - (float)iv0[k];
    vel[k] += b_y * dv6[k];
    pos[k] += timestep * vel[k];
  }
}

void sensor_fusion(imu_datagram_t *data, pod_t *s) {
  if (!imu_valid(data)) {
    warn("IMU NOT VALID: %X != %X; STAT: %X\n", data->computed_crc, data->crc,
           data->status);
    return;
  }

  float ax = data->x + get_value_f(&(s->imu_calibration_x));
  float ay = data->y + get_value_f(&(s->imu_calibration_y));
  float az = data->z + get_value_f(&(s->imu_calibration_z));

  if (OUTSIDE(-10.0, ax, 10.0)) {
    warn("Unacceptable IMU Accelleration %f in X", ax);
    return;
  }

  if (OUTSIDE(-10.0, ay, 10.0)) {
    warn("Unacceptable IMU Accelleration %f in Y", ay);
    return;
  }

  if (OUTSIDE(-10.0, az, 10.0)) {
    warn("Unacceptable IMU Accelleration %f in Z", az);
    return;
  }

  // unpack state data types into arrays that work better with calculations
  float acc[3] = {ax,ay,az};
  float vel[3] = {
    get_value_f(&(s->velocity_x)),
    get_value_f(&(s->velocity_y)),
    get_value_f(&(s->velocity_z))
  };
  float pos[3] = {
    get_value_f(&(s->position_x)),
    get_value_f(&(s->position_y)),
    get_value_f(&(s->position_z))
  };

  float rotvel[3] = {data->wx, data->wy, data->wz};
  float quat[4] = {
    get_value_f(&(s->quaternion_real)),
    get_value_f(&(s->quaternion_i)),
    get_value_f(&(s->quaternion_j)),
    get_value_f(&(s->quaternion_k))
  };

  static uint64_t last_time = 0;

  if (last_time == 0) {
    last_time = get_time();
    return;
  }

  uint64_t now = get_time();
  if (now-last_time <= 0) {
    warn("Called Within 1us of last");
    return;
  }

  //TODO: fix the magic timestep number to something global
  integrate_imu(.001, acc, vel, pos, rotvel, quat);

  last_time = now;

  // because we don't have the actual buffer of PE sensors yet
  // and we don't even use the sensor pack data we read in
  // and ideally everything I write will be replaced by (?Kalman?)
  // I'll just put pseudocode down

  // iterate through the photoelectric sensors recorded since the last Kalman ran
  	// easy mode - if we see obvious strip, snap to nearest if in ~5 meters
  	// detect whether there's a strip by low pass filtering the data
  	// and comparing to a moving avg/median
  	// hard mode - do whatever Kalman does with that shit


  // pack it all back up into neat little boxes
  set_value_f(&(s->accel_x), acc[0]);
  set_value_f(&(s->accel_y), acc[1]);
  set_value_f(&(s->accel_z), acc[2]);

  set_value_f(&(s->velocity_x), vel[0]);
  set_value_f(&(s->velocity_y), vel[1]);
  set_value_f(&(s->velocity_z), vel[2]);

  set_value_f(&(s->position_x), pos[0]);
  set_value_f(&(s->position_y), pos[1]);
  set_value_f(&(s->position_z), pos[2]);

  set_value_f(&(s->rotvel_x), rotvel[0]);
  set_value_f(&(s->rotvel_y), rotvel[1]);
  set_value_f(&(s->rotvel_z), rotvel[2]);

  set_value_f(&(s->quaternion_real), quat[0]);
  set_value_f(&(s->quaternion_i), quat[1]);
  set_value_f(&(s->quaternion_j), quat[2]);
  set_value_f(&(s->quaternion_k), quat[3]);
}
