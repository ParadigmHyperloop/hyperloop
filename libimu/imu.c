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

#include "imu.h"
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "crc.h"

#define IMU_MESSAGE_SIZE 36

#ifndef B921600
#define B921600 921600
#endif

#ifndef IMU_TEMP_MIN
#define IMU_TEMP_MIN -40
#endif

#ifndef IMU_TEMP_MAX
#define IMU_TEMP_MAX 75
#endif

#ifndef IMU_STATUS_OK
#define IMU_STATUS_OK 0x77
#endif

union bytes_to_float
{
   unsigned char b[4];
   float f;
} b2f;


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
//   0x00, 0x01 // temperature bits (Int16)
// };

unsigned char imubuf[IMU_MESSAGE_SIZE] = {0};
int imubufc = 0;
/**
 * fills the buffer pointed to (should be imubuf) until it contains n elements
 */
ssize_t serial_read(int fd, unsigned char *buf, int n) {

  int r = 0;
  while (r < n) {
    ssize_t _r = read(fd, &buf[r], n - r);

    if (_r < 0) {
      // TODO: Check errno for EAGAIN
      return r;
    }
    r += _r;
  }
  // Success, read all n bytes
  return r;
}

ssize_t imu_read(int fd, imu_datagram_t * gram) {
  int i = 0;
  int remaining = IMU_MESSAGE_SIZE - imubufc;

  if (remaining > 0) {
    ssize_t r = serial_read(fd, &imubuf[imubufc], remaining);
    assert(r <= IMU_MESSAGE_SIZE);
    assert(r >= -1);
    imubufc += r;

    if (r == -1) {
      memset(gram, 0, sizeof(imu_datagram_t));
      return -1;
    } else if (r < remaining) {
      // for (i=0; i<imubufc; i++) {
      //   printf("%x ", imubuf[i]);
      // }
      assert(imubufc < 36);
      memset(gram, 0, sizeof(imu_datagram_t));
      return 0;
    }
    assert(imubufc == 36);
  }

  assert(imubufc == 36);

  unsigned char ideal[4] = {0xFE, 0x81, 0xFF,0x55};
  assert(imubufc == 36);


  int success = 1;
  assert(imubufc == 36);
  for (i = 0; i < 4; i++) {
    assert(imubufc == 36);
    if (imubuf[i] != ideal[i]) {
      success = 0;
    }
  }

  assert(imubufc == 36);
  if (!success) {
    imubufc--;
    assert(imubufc == 35);
    for (i=0; i<imubufc; i++) {
      imubuf[i] = imubuf[i+1];
    }
    memset(gram, 0, sizeof(imu_datagram_t));
    return -1;
  }

  // Massive Bit Shifting Operation.
  // See the example imu_datagram_t in the comment at the top of this file
  *gram = (imu_datagram_t){
    .hd = (imubuf[0] << 24) | (imubuf[1] << 16) | (imubuf[2] << 8) | imubuf[3],
    .wx = ((union bytes_to_float) { .b = { imubuf[7], imubuf[6], imubuf[5], imubuf[4] } }).f,
    .wy = ((union bytes_to_float) { .b = { imubuf[11], imubuf[10], imubuf[9], imubuf[8] } }).f,
    .wz = ((union bytes_to_float) { .b = { imubuf[15], imubuf[14], imubuf[13], imubuf[12] } }).f,
    .x = ((union bytes_to_float) { .b = { imubuf[19], imubuf[18], imubuf[17], imubuf[16] } }).f,
    .y = ((union bytes_to_float) { .b = { imubuf[23], imubuf[22], imubuf[21], imubuf[20] } }).f,
    .z = ((union bytes_to_float) { .b = { imubuf[27], imubuf[26], imubuf[25], imubuf[24] } }).f,
    .status = imubuf[28],
    .sequence = imubuf[29],
    .temperature = (uint16_t)(imubuf[30] << 8) | (imubuf[31]),
    .crc = (imubuf[32] << 24) | (imubuf[33] << 16) | (imubuf[34] << 8) | (imubuf[35] << 0),
    .computed_crc = crc_calc(&imubuf[0], 32)
  };

  imubufc = 0;

  return 1;
}


// Connect the serial device for the IMU
int imu_connect(const char * device) {
  crc_generate_table();

  // note("Connecting to IMU at: %s", device);
  if (access(device, F_OK) != 0) {
    // error_no("device '%s' does not exist", device);
    printf("IMU Device '%s' does not exist: %s\n", device, strerror(errno));
    return -1;
  }

  int fd = open(device, O_RDWR | O_NONBLOCK);

  if (fd < 0) {
    perror("IMU connect failed");
    return -1;
  }


  struct termios tty;

  if (tcgetattr(fd, &tty) < 0) {
      printf("Error from tcgetattr: %s\n", strerror(errno));
      return -1;
  }

  cfsetospeed(&tty, (speed_t)B921600);
  cfsetispeed(&tty, (speed_t)B921600);

  tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;         /* 8-bit characters */
  tty.c_cflag &= ~PARENB;     /* no parity bit */
  tty.c_cflag &= ~CSTOPB;     /* 1 Stop bit */
  tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

  /* setup for non-canonical mode */
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty.c_oflag &= ~OPOST;

  /* fetch bytes as they become available */
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 1;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
      printf("Error from tcsetattr: %s\n", strerror(errno));
      return -1;
  }

  return fd;
}

// Connect the serial device for the IMU
int imu_disconnect(int fd) {
  if (fd < 0) {
    fprintf(stderr, "IMU fd invalid\n");
    return -1;
  }


  // TODO: Reset Settings?

  return close(fd);
}

int imu_valid(imu_datagram_t * data) {
  return (data->crc == data->computed_crc) && (data->status == IMU_STATUS_OK);
}

int imu_ok(imu_datagram_t * data) {
  return (data->temperature > IMU_TEMP_MIN && data->temperature < IMU_TEMP_MAX);
}
