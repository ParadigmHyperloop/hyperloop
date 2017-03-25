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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "bbb.h"

/*
  Memory Mapped Getting Started
 #define GPIO1_START_ADDR 0x4804C000
 #define GPIO1_END_ADDR   0x4804e000
 #define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)

 #define GPIO_SETDATAOUT 0x194
 #define GPIO_CLEARDATAOUT 0x190
 #define USR3 (1<<24)

 volatile void *gpio_addr;
 volatile unsigned int *gpio_setdataout_addr;
 volatile unsigned int *gpio_cleardataout_addr;
 int fd = open("/dev/mem", O_RDWR);
 gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);

 gpio_setdataout_addr   = gpio_addr + GPIO_SETDATAOUT;
 gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

*/
ssize_t sysfs_write(int pin, char *op, char *data) {
  char path[SYSFS_GPIO_MAX_PATH];
  snprintf(path, SYSFS_GPIO_MAX_PATH, SYSFS_GPIO_PIN_FMT, pin, op);

  int fd = open(path, O_WRONLY);

  if (fd < 0) {
    return -1;
  }

  ssize_t rc = write(fd, data, strlen(data));

  close(fd);

  return rc;
}

ssize_t sysfs_read(int pin, char *op, char *data, size_t len) {
  char path[SYSFS_GPIO_MAX_PATH];
  snprintf(path, SYSFS_GPIO_MAX_PATH, SYSFS_GPIO_PIN_FMT, pin, op);
  int fd = open(path, O_RDONLY);

  if (fd < 0) {
    return -1;
  }

  ssize_t rc = read(fd, data, len);

  close(fd);

  return rc;
}

ssize_t init_pin(__unused int pin) {
  return -1;
}

ssize_t set_pin_direction(gpio_t pin, gpio_dir_t dir) {
  char *data;
  switch (dir) {
    case kGpioIn:
      data = SYSFS_GPIO_IN;
      break;
    case kGpioOut:
      data = SYSFS_GPIO_OUT;
      break;
    case kGpioDirError:
      return -1;
  }

  return sysfs_write(pin, SYSFS_GPIO_DIR_FILE, data);
}

gpio_dir_t get_pin_direction(gpio_t pin) {
  char data[4];
  ssize_t count = sysfs_read(pin, SYSFS_GPIO_DIR_FILE, data, 4);

  if (count < 0) {
    return kGpioDirError;
  }

  if (strncmp(data, SYSFS_GPIO_IN, strlen(SYSFS_GPIO_IN))) {
    return kGpioIn;
  } else if (strncmp(data, SYSFS_GPIO_OUT, strlen(SYSFS_GPIO_OUT))) {
    return kGpioOut;
  }
  
  return kGpioDirError;
}


ssize_t set_pin_value(gpio_t pin, gpio_value_t value) {
  char *data;
  switch (value) {
    case kGpioLow:
      data = SYSFS_GPIO_LOW;
      break;
    case kGpioHigh:
      data = SYSFS_GPIO_HIGH;
      break;
    case kGpioValError:
      return -1;
  }

  return sysfs_write(pin, SYSFS_GPIO_VAL_FILE, data);

}

gpio_value_t get_pin_value(gpio_t pin) {
  char c;
  ssize_t r = sysfs_read(pin, SYSFS_GPIO_VAL_FILE, &c, 1);

  if (r != 1) {
    return kGpioValError;
  }

  switch (c) {
    case '0':
      return kGpioLow;
    case '1':
      return kGpioHigh;
    default:
      return kGpioValError;
  }
}
