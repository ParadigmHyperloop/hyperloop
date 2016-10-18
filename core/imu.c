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
#include <termios.h>

int imuFd = -1;

// All units are assumed to be in meters
typedef struct {
  // header
  uint32_t hd;
  // linear acceleration X
  float x;
  // linear acceleration Y
  float y;
  // linear acceleration Z
  float z;
  // angular acceleration X
  float wx;
  // angular acceleration Y
  float wy;
  // angular acceleration Z
  float wz;
  // sequence byte
  uint8_t sequence;
  // status byte
  uint8_t status;
  // temperature byte
  float temperature;
} imu_datagram_t;

// example datagram, MSB is always printed first
// uint8_t example[32] = {
//   0xFE, 0x81, 0xFF, 0x55, // Message Header, exactly these bytes
//   0x01, 0x23, 0x45, 0x65, // X rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Y rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z rotational (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z linear (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Y linear  (4 Bytes to form a float)
//   0x01, 0x23, 0x45, 0x65, // Z linear  (4 Bytes to form a float)
//   0xEE, // Status (each bit wx, wy, wz, reserved, ax, ay, az, reserved)
//   0xFF, // sequence number (0-127 wraps)
//   0x00, 0x01 // temperature bits (UInt16)
// };

// TODO: This is really a job for ring_buf_t
unsigned char imubuf[IMU_MESSAGE_SIZE] = {0};
int imubufc = 0;

/**
 * Determines if we have a valid header read in
 *
 * Returns -1 if read is failing (applicable in O_NONBLOCK)
 * Returns 0 if we are not at a header
 * Returns 1 if we are at a header
 */
int atHeader() {
  int i;
  bool success = false;
  // TODO: Remove these asserts and log errors instead, Definitely when
  // O_NONBLOCK is used
  while (imubufc < 32) {

    // Try a non blocking read
    if (read(imuFd, &imubuf[imubufc], 1) != 1) {
      return -1;
    }
    imubufc++;
  }

  if (imubuf[0] == 0xFE) {
    unsigned const char ideal[3] = {0x81, 0xFF, 0x55};

    success = true;
    for (i = 1; i < 4; i++) {
      printf("%x\n", imubuf[i]);
      if (imubuf[i] != ideal[i]) {
        success = false;
      }
    }
  }

  if (!success) {
    // remove the first byte and shuffle everyone back once
    for (i = 0; i < 3; i++) {
      imubuf[i] = imubuf[i + 1];
    }
    imubufc--;
  }

  return (success ? 1 : 0);
}

extern pod_state_t __state;

int readIMUDatagram(uint64_t t, imu_datagram_t *gram) {
#ifdef TESTING
// Lets have a chat together... this testing simulator... is terrible...
// It does not follow the laws of physics to the nail, but it is something
// that excersises the system with minimal impact on the __state.
// So... I recognize that this is terrible. I will fix it into a proper sim
// when there is time
// Generates NOISE
#define NOISE (float)(rand() - RAND_MAX / 2) / (RAND_MAX * 100.0)
  static float base_ax = 0.0; // NOTE: preserved accross calls
  static uint64_t pushing = 0;
  static uint64_t last_time = 0;
  static uint64_t coasting = 0;
  static bool moving = false;

  if (last_time == 0) {
    last_time = t;
  }

  float vx = __state.velocity_x.value.fl;
  // If ready, start pushing at some random time
  if (getPodMode() == Ready) {
    if (rand() % 7 == 0) {
      base_ax = 2.0;
      pushing = t; // Turn on the pusher for future calls
    }
  } else if (pushing > 0) {
    moving = true;
    base_ax = 2.0;

    uint64_t q =
        t - pushing; // The time period after pusher has started to back off
    debug("[SIM] PUSHER IS ON => usecs: %llu\n", q);
    if (q > 5 * 1000 * 1000) { // After 5 seconds of pushing
      base_ax = 6.0 - ((float)q / (1000.0 * 1000.0));
      if (q > 6 * 1000 * 1000) {
        pushing = 0;  // Turn off the pusher after 6 seconds of pushing
        coasting = t; // Turn off the pusher after 6 seconds of pushing
      }
    }
  } else if (coasting) { // or braking
    base_ax = -1.0;
  }

  float ax = base_ax;
  if (__state.tmp_brakes == 1 && moving) {
    debug("Brakes applied");
    ax -= 7.0;

    // predict velocity on sim end to simulate smooth transition to vx=0.0

    if (vx < -ax) {
      if (vx > 0.0) {
        double rough_dt = t - last_time;
        ax = (0.0 - vx) / rough_dt;
      }
    }
    if (vx < 0) {
      error("[SIM] NEGATIVE VELOCITY");
    }
  }

  ax += NOISE;
  debug("[SIM] Giving => x: %f, y: **, z: **, wx: **, wy: **, wz: **,\n", ax);
  *gram = (imu_datagram_t){
      .x = ax, .y = NOISE, .z = NOISE, .wx = NOISE, .wy = NOISE, .wz = NOISE};
  return 0;
#undef NOISE
#else

  retries = 0;
  bool at_header = false;
  while ((at_header = atHeader()) == 0 && retries < 32)
    retries++;

  if (at_header < 1) {
    error("Could not find an IMU data header: %d", at_header);
    return -1;
  } else {
    assert(imubufc == 32);
  }

  // Massive Bit Shifting Operation.
  // See the example imu_datagram_t in the comment at the top of this file
  *gram = {.hd = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3],
           .wx = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7],
           .wy = (buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11],
           .wz = (buf[12] << 24) | (buf[13] << 16) | (buf[14] << 8) | buf[15],
           .x = (buf[16] << 24) | (buf[17] << 16) | (buf[18] << 8) | buf[19],
           .y = (buf[20] << 24) | (buf[21] << 16) | (buf[22] << 8) | buf[23],
           .z = (buf[24] << 24) | (buf[25] << 16) | (buf[26] << 8) | buf[27],
           .status = buf[28],
           .sequence = buf[29],
           .temperature = buf[30] | (buf[31] << 8)};
  return 0;
#endif
}

// Connect the serial device for the IMU
int imuConnect() {
  imuFd = open(IMU_DEVICE, O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR);

  if (imuFd < 0) {
    return -1;
  }

  struct termios pts;
  tcgetattr(imuFd, &pts);
  /*make sure pts is all blank and no residual values set
  safer than modifying current settings*/
  pts.c_lflag = 0; /*implies non-canoical mode*/
  pts.c_iflag = 0;
  pts.c_oflag = 0;
  pts.c_cflag = 0;

  // Baud Rate (This is super high by default)
  pts.c_cflag |= 921600;

  // 8 Data Bits
  pts.c_cflag |= CS8;

  pts.c_cflag |= CLOCAL;
  pts.c_cflag |= CREAD;

  // Set the terminal device atts
  tcsetattr(imuFd, TCSANOW, &pts);

  return imuFd;
}

int calcState(pod_value_t *a, pod_value_t *v, pod_value_t *x, float new_accel,
              double dt) {

  float acceleration = getPodField_f(a);
  float velocity = getPodField_f(v);
  float position = getPodField_f(x);

  // Calculate the new_velocity (oldv + (olda + newa) / 2)

  // float dv = calcDu(dt, acceleration, new_accel);
  float dv = ((dt * ((acceleration + new_accel) / 2)) / 1000000.0);
  float new_velocity = (velocity + dv);

  // float dx = calcDu(dt, velocity, new_velocity);
  float dx = ((dt * ((velocity + new_velocity) / 2)) / 1000000.0);
  float new_position = (position + dx);

  debug("dt: %lf us, dv: %f m/s, dx: %f m\n", dt, dv, dx);

  setPodField_f(a, new_accel);
  setPodField_f(v, new_velocity);
  setPodField_f(x, new_position);

  return 0;
}

/**
 * Reads data from the IMU, computes the new Acceleration, Velocity, and
 * Position state values
 */
int imuRead(pod_state_t *p) {
  // TODO: Clean up all this time tracking code

  static uint64_t start_time = 0;
  static uint64_t lastCheckTime = 0;

  // Intiializes local static varaibles
  if (start_time == 0) {
    lastCheckTime = getTime();
    start_time = getTime();
    usleep(1);
  }

  imu_datagram_t data;

  // TODO: This time tracking is giving me a headache
  uint64_t currentCheckTime = getTime(); // Same as above, assume milliseconds

  // Read from the IMU also provide the time offset scince start for simulator
  if (readIMUDatagram(currentCheckTime - start_time, &data) < 0) {
    return -1;
  }

  double dt = (double)(currentCheckTime - lastCheckTime);
  lastCheckTime = currentCheckTime;

  if (dt <= 0.0) {
    error("time scince last check <= 0.0, this should not happen");
    return -1;
  }

  calcState(&(p->accel_x), &(p->velocity_x), &(p->position_x), data.x, dt);
  calcState(&(p->accel_y), &(p->velocity_y), &(p->position_y), data.y, dt);
  calcState(&(p->accel_z), &(p->velocity_z), &(p->position_z), data.z, dt);

  return 0;
}
