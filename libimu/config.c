#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <termios.h>
#include "imu.h"


#ifdef __linux__
static const char *PORT_NAME = "/dev/ttyUSB0";
#else
static const char *PORT_NAME = "/dev/cu.usbserial-A600DTJI";
#endif

#define CONFIG_CMD "=config,1\r\n"
#define CONFIG_RES "CONFIG,1"

#define SPINUP_ITER 10000

uint64_t getTime() {
  struct timeval currentTime;

  assert(gettimeofday(&currentTime, NULL) == 0);

  return (currentTime.tv_sec * 1000000ULL) + currentTime.tv_usec;
}

ssize_t read_with_timeout(int fd, void * buf, int len, int t) {
  fd_set set;
  struct timeval timeout;
  int rv;


  FD_ZERO(&set); /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  timeout.tv_sec = t / 1000;
  timeout.tv_usec = t % 1000;

  rv = select(fd + 1, &set, NULL, NULL, &timeout);
  if(rv == -1) {
    perror("select: "); /* an error accured */
    printf("\n");
  } else if(rv == 0) {
    fprintf(stderr, "read-timeout\n"); /* a timeout occured */
  } else {
    return read( fd, buf, len ); /* there was data to read */
  }
  return -1;
}

int main() {
  printf("Connecting to IMU: %s\n", PORT_NAME);
  int fd = imu_connect(PORT_NAME);

  if (fd < 0) {
    exit(1);
  }

  printf("IMU Connected!\n");

  char buf[4096];

  printf("Putting IMU into config mode!\n");
  int k = 0;
  while (1) {
    char cmd[64] = CONFIG_CMD;
    int i = 0;
    for (i=0; i < strlen(cmd); i++) {
      write(fd, &cmd[i], 1);
      write(STDOUT_FILENO, &cmd[i], 1);
      usleep(10000);
    }

    ssize_t nread = read_with_timeout(fd, buf, 4096, 1000);
    if (nread <= 0) {
      printf("IMU Stopped spewwing, might be in config mode\n");
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
      int k = 0;
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

      int i = 0;
      for (i=0; i < strlen(cmd); i++) {
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
