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

#include <imu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>

#ifdef __linux__
static const char *PORT_NAME = "/dev/ttyUSB0";
#else
static const char *PORT_NAME = "/dev/cu.usbserial-A600DTJI";
#endif

#define CONFIG_CMD "=config,1\r\n"
#define CONFIG_RES "CONFIG,1"

#define SPINUP_ITER 10000

typedef struct config {
  const char * device;
} config_t;

static
void usage() {
  printf("Usage: imu_config [-d DEVICE]\n");
  printf("  -d    Path to the device (/dev/ttyUSB0)\n");
  exit(1);
}


static
void parse_args(int argc, char *argv[], config_t *config) {
  int ch;

  while ((ch = getopt(argc, argv, "d:h")) != -1) {
    switch (ch) {
      case 'd':
        config->device = optarg;
        break;
      case 'h':
      default:
        usage();
    }
  }
}

static
ssize_t read_with_timeout(int fd, void *buf, int len, int t) {
  fd_set set;
  struct timeval timeout;
  int rv;

  FD_ZERO(&set);    /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  timeout.tv_sec = t / 1000;
  timeout.tv_usec = t % 1000;

  rv = select(fd + 1, &set, NULL, NULL, &timeout);
  if (rv == -1) {
    perror("select: "); /* an error occurred */
    printf("\n");
  } else if (rv == 0) {
    fprintf(stderr, "read-timeout\n"); /* a timeout occurred */
  } else {
    return read(fd, buf, len); /* there was data to read */
  }
  return -1;
}

int main(int argc, char *argv[]) {
  config_t config = {
    .device = PORT_NAME
  };

  parse_args(argc, argv, &config);

  printf("Connecting to IMU: %s\n", config.device);
  int fd = imu_connect(config.device);

  if (fd < 0) {
    exit(1);
  }

  printf("IMU Connected!\n");

  char buf[4096];

  printf("Putting IMU into config mode!\n");
  int k = 0;
  while (1) {
    char cmd[64] = CONFIG_CMD;
    size_t i = 0;
    for (i = 0; i < strlen(cmd); i++) {
      write(fd, &cmd[i], 1);
      write(STDOUT_FILENO, &cmd[i], 1);
      usleep(10000);
    }

    ssize_t nread = read_with_timeout(fd, buf, 4096, 1000);
    if (nread <= 0) {
      printf("IMU Stopped spewing, might be in config mode\n");
      break;
    }

    if (strncmp(buf, "CONFIG,1", 8) == 0) {
      printf("done!\n");
      break;
    }

    if (k > 20) {
      printf("Might still be in Normal Mode\n");
      break;
    }
    k++;
  }
  char cmd[64];
  printf("Enter Command > ");

  while ((fgets(cmd, 64, stdin) != NULL) /* && (cmd[0] != '\n') */) {
    if (strncmp(cmd, "drain", 5) == 0) {
      printf("Draining the serial pipe...");
      fflush(stdout);
      k = 0;
      while (1) {
        k++;
        if (k > 1000) {
          printf("\nPipe is big or device is still in normal mode\n");
          break;
        }
        printf(".");
        fflush(stdout);
        ssize_t nread = read_with_timeout(fd, buf, 4096, 1000);
        if (nread <= 0) {
          printf("done!\n");
          break;
        }
      }
      fflush(stdout);
    } else if (strncmp(cmd, "exit", 4) == 0) {
      break;
    } else if (strncmp(cmd, "help", 4) == 0) {
      printf("Use any command from the KVH 1725 IMU manual then hit enter\n");
      printf(" - Or type 'help', 'exit', or 'drain' (clears kern buffer)\n");
    } else {
      printf("Sending Command: %s", cmd);

      size_t i = 0;
      for (i = 0; i < strlen(cmd); i++) {
        write(fd, &cmd[i], 1);
        write(STDOUT_FILENO, &cmd[i], 1);
        usleep(10000);
      }

      ssize_t nread = read_with_timeout(fd, buf, 4096, 1000);
      if (nread > 0) {
        write(STDOUT_FILENO, buf, nread);
      }
    }
    printf("Enter Command > ");
  }
  imu_disconnect(fd);
  return 0;
}
