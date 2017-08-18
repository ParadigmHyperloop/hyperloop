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

#include <crc.h>
#include <imu.h>
#include <assert.h>
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>

#ifdef __linux__
static char *PORT_NAME = "/dev/ttyUSB0";
#else
static char *PORT_NAME = "/dev/cu.usbserial-A600DTJI";
#endif

#define SPINUP_ITER 10000
#define MAX_ITERATIONS 50000

typedef struct {
  int iterations;
  int spinup;
  int stop_on_fail;
  char *device;
  int quiet;
} config_t;

static
uint64_t getTime() {
  struct timeval currentTime;

  assert(gettimeofday(&currentTime, NULL) == 0);

  return (currentTime.tv_sec * 1000000ULL) + currentTime.tv_usec;
}

static
void usage() {
  printf("Usage: imu_test [-i ITERATIONS] [-s SPINUP] [-d DEVICE] [-q] [-f]\n");
  printf("  -i    The maximium number of iterations to read\n");
  printf("  -d    Path to the device (/dev/ttyUSB0)\n");
  printf("  -q    Disable printing of each datagram\n");
  printf("  -f    Stop immediately on failure\n");
  exit(1);
}

static
void parse_args(int argc, char *argv[], config_t *config) {
  int ch;
  while ((ch = getopt(argc, argv, "d:i:s:qfh")) != -1) {
    switch (ch) {
      case 'd':
        config->device = optarg;
        break;
      case 'i':
        config->iterations = atoi(optarg);
        break;
      case 's':
        config->spinup = atoi(optarg);
        break;
      case 'q':
        config->quiet = 1;
        break;
      case 'f':
        config->stop_on_fail = 1;
        break;
      case 'h':
      default:
        usage();
    }
  }
}

static
void finish(int code, int fd) {
  imu_disconnect(fd);
  exit(code);
}

int main(int argc, char *argv[]) {
  config_t config = {.iterations = MAX_ITERATIONS,
    .spinup = SPINUP_ITER,
    .device = PORT_NAME,
    .stop_on_fail = 0,
    .quiet = 0};

  parse_args(argc, argv, &config);

  printf("[TEST] Connecting to: %s\n", config.device);

  int fd = imu_connect(config.device);

  if (fd < 0) {
    exit(1);
  }

  int l = -1;
  int i = 0;
  int poop = 0;

  imu_datagram_t average = IMU_DATAGRAM_INIT;
  uint64_t start = getTime();
  while (i < config.iterations || config.iterations == 0) {
    imu_datagram_t data = IMU_DATAGRAM_INIT;

    ssize_t success = 0;
    while (success == 0) {
      // Loop until error or good read
      success = imu_read(fd, &data);
    }

    if (!config.quiet && success > 0) {
      printf("[TEST] hd: %X\tx: %f\ty: %f\tz: %f\twx: %f\twy: %f\twz: %f\t "
             "seq: %d\t stat: %X\t tp: %u\t crc: %X \t ckc: %X\t t: %" PRIu64 "\n",
             data.hd, data.x, data.y, data.z, data.wx, data.wy, data.wz,
             data.sequence, data.status, data.temperature, data.crc,
             data.computed_crc, getTime());
    }

    average.x = (average.x + data.x) / 2.0f;
    average.y = (average.y + data.y) / 2.0f;
    average.z = (average.z + data.z) / 2.0f;
    average.wx = (average.wx + data.wx) / 2.0f;
    average.wy = (average.wy + data.wy) / 2.0f;
    average.wz = (average.wz + data.wz) / 2.0f;

    if (i >= config.spinup) {
      if (success < 0) {
        fprintf(stderr, "[FAIL] [%d] IMU Read Failed\n", i);
        if (config.stop_on_fail) {
          finish(1, fd);
        } else {
          poop++;
          i++;
          continue; // Skip the rest of the checks for this read
        }
      }

      if (data.crc != data.computed_crc) {
        fprintf(stderr, "[FAIL] [%d] CRC error\n", i);
        if (config.stop_on_fail) {
          finish(1, fd);
        } else {
          poop++;
          i++;
          continue; // Skip the rest of the checks for this read
        }
      }

      if (data.status != 0x77) {
        fprintf(stderr, "[FAIL] [%d] Bad Status! %X\n", i, data.status);
        if (config.stop_on_fail) {
          finish(1, fd);
        } else {
          poop++;
        }
      }

      if ((l + 1) % 128 != data.sequence) {
        fprintf(stderr,
                "[FAIL] [%d] Skipped Sequence Number! %d not after %d\n", i,
                data.sequence, l);
        if (config.stop_on_fail) {
          finish(1, fd);
        } else {
          poop++;
        }
      }

      l = data.sequence;

      if (data.temperature < -20 || data.temperature > 50) {
        fprintf(stderr,
                "[FAIL] [%d] temperature warning (%dC). (min -20C, max 50C)\n",
                i, data.temperature);
        finish(1, fd);
      }
      fflush(stdout);

    } else if (i == (config.spinup - 1)) {
      fprintf(stderr, "[NOTE] Spinup complete %d/%d\n", i, config.spinup);
      l = data.sequence;
    } else {
      if (i % 1000 == 0) {
        fprintf(stderr, "[NOTE] Spinning Up %d/%d\n", i, config.spinup);
      }
    }
    i++;
  }
  uint64_t end = getTime();
  printf("[AVERAGE] x: %f\ty: %f\tz: %f\twx: %f\twy: %f\twz: %f\n", average.x,
         average.y, average.z, average.wx, average.wy, average.wz);

  double seconds = (end - start) / 1000000.0;
  printf("[TIME] %lf seconds. %d packets processed %lf packets / second",
         seconds, i, (double)i / seconds);

  if (poop > 0) {
    fprintf(stderr, "Found %d errors out of %d iterations, %d skipped. %f%%\n",
            poop, config.iterations, config.spinup,
            (float)poop / (float)(config.iterations - config.spinup));
    finish(1, fd);
  } else {
    finish(0, fd);
  }
  return 1;
  ;
}
