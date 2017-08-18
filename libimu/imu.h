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

#ifndef __OPENLOOPALLIANCE_IMU_H__
#define __OPENLOOPALLIANCE_IMU_H__

#include <stdint.h>
#include <unistd.h>

/**
 * Represents an IMU datagram packet
 */
typedef struct imu_datagram {
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
  // sequence
  uint8_t sequence;
  // status
  uint8_t status;
  // temperature
  int16_t temperature;
  // CRC check (actual)
  uint32_t crc;
  // CRC check (computed)
  uint32_t computed_crc;
} imu_datagram_t;

#define IMU_DATAGRAM_INIT { \
.hd = 0.0, \
.x = 0.0, \
.y = 0.0, \
.z = 0.0, \
.wx = 0.0, \
.wy = 0.0, \
.wz = 0.0, \
.sequence = 0, \
.status = 0, \
.temperature = 0, \
.crc = 0, \
.computed_crc = 0 \
}

/**
 * Connect to the IMU on the given device path
 *
 * @param device The path to the IMU device, ex. "/dev/ttyUSB0"
 *
 * @return file descriptor on success, -1 on failure
 */
int imu_connect(const char *device);

/**
 * Disconnect the IMU
 *
 * @param fd The fd of the connected IMU
 *
 * @return the result of close(2)
 */
int imu_disconnect(const int fd);

/**
 * Connect to the IMU on the given device path
 *
 * @param fd The file descriptor for the IMU
 * @param data Pointer to an IMU datagram to populate
 *
 * @return 0 on success, -1 on failure
 */
ssize_t imu_read(int fd, imu_datagram_t *data);

/**
 * Determines whether the given imu_datagram_t has a valid CRC and status
 *
 * @param data Pointer to an imu_datagram_t to validate
 *
 * @return 1 if valid, 0 if invalid
 */
int imu_valid(imu_datagram_t *data);

/**
 * Determines if the IMU is in an OK state based on the given imu_datagram_t
 *
 * @param data A pointer to an imu_datagram_t
 *
 * @return 1 if IMU is OK, 0 if it is not OK
 */
int imu_ok(imu_datagram_t *data);

#endif
