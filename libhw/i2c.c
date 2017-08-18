//
//  i2c.c
//  pod
//
//  Created by Eddie Hurtig on 8/18/17.
//
//

#include "i2c.h"

/*******************************
 * i2cfunc.c v3
 * wrapper for I2C functions
 *******************************/
#include <errno.h>

#ifdef BBB
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>


int i2c_open(unsigned char bus, unsigned char addr) {
  int file;
  char filename[16];
  sprintf(filename, "/dev/i2c-%d", bus);
  if ((file = open(filename, O_RDWR)) < 0) {
    fprintf(stderr, "i2c_open open error: %s\n", strerror(errno));
    return (file);
  }
  if (ioctl(file, I2C_SLAVE, addr) < 0) {
    fprintf(stderr, "i2c_open ioctl error: %s\n", strerror(errno));
    return (-1);
  }
  return (file);
}

int i2c_write(int handle, unsigned char *buf, unsigned int length) {
  if (write(handle, buf, length) != length) {
    fprintf(stderr, "i2c_write error: %s\n", strerror(errno));
    return (-1);
  }
  return (length);
}

int i2c_write_byte(int handle, unsigned char val) {
  if (write(handle, &val, 1) != 1) {
    fprintf(stderr, "i2c_write_byte error: %s\n", strerror(errno));
    return (-1);
  }
  return (1);
}

int i2c_read(int handle, unsigned char *buf, unsigned int length) {
  if (read(handle, buf, length) != length) {
    fprintf(stderr, "i2c_read error: %s\n", strerror(errno));
    return (-1);
  }
  return (length);
}

int i2c_read_byte(int handle, unsigned char *val) {
  if (read(handle, val, 1) != 1) {
    fprintf(stderr, "i2c_read_byte error: %s\n", strerror(errno));
    return (-1);
  }
  return (1);
}

int i2c_close(int handle) {
  if ((close(handle)) != 0) {
    fprintf(stderr, "i2c_close error: %s\n", strerror(errno));
    return (-1);
  }
  return (0);
}

int i2c_write_read(int handle, unsigned char addr_w, unsigned char *buf_w,
                   unsigned int len_w, unsigned char addr_r,
                   unsigned char *buf_r, unsigned int len_r) {
  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msgs[2];

  msgs[0].addr = addr_w;
  msgs[0].len = len_w;
  msgs[0].flags = 0;
  msgs[0].buf = buf_w;

  msgs[1].addr = addr_r;
  msgs[1].len = len_r;
  msgs[1].flags = 1;
  msgs[1].buf = buf_r;

  msgset.nmsgs = 2;
  msgset.msgs = msgs;

  if (ioctl(handle, I2C_RDWR, (unsigned long)&msgset) < 0) {
    fprintf(stderr, "i2c_write_read error: %s\n", strerror(errno));
    return -1;
  }
  return (len_r);
}

int i2c_write_ignore_nack(int handle, unsigned char addr_w, unsigned char *buf,
                          unsigned int length) {
  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msgs[1];

  msgs[0].addr = addr_w;
  msgs[0].len = length;
  msgs[0].flags = 0 | I2C_M_IGNORE_NAK;
  msgs[0].buf = buf;

  msgset.nmsgs = 1;
  msgset.msgs = msgs;

  if (ioctl(handle, I2C_RDWR, (unsigned long)&msgset) < 0) {
    fprintf(stderr, "i2c_write_ignore_nack error: %s\n", strerror(errno));
    return -1;
  }
  return (length);
}

int i2c_read_no_ack(int handle, unsigned char addr_r, unsigned char *buf,
                    unsigned int length) {
  struct i2c_rdwr_ioctl_data msgset;
  struct i2c_msg msgs[1];

  msgs[0].addr = addr_r;
  msgs[0].len = length;
  msgs[0].flags = I2C_M_RD | I2C_M_NO_RD_ACK;
  msgs[0].buf = buf;

  msgset.nmsgs = 1;
  msgset.msgs = msgs;

  if (ioctl(handle, I2C_RDWR, (unsigned long)&msgset) < 0) {
    fprintf(stderr, "i2c_read_no_ack error: %s\n", strerror(errno));
    return -1;
  }
  return (length);
}

int delay_ms(unsigned int msec) {
  int ret;
  struct timespec a;
  if (msec > 999) {
    fprintf(stderr, "delay_ms error: delay value needs to be less than 999\n");
    msec = 999;
  }
  a.tv_nsec = ((long)(msec)) * 1E6d;
  a.tv_sec = 0;
  if ((ret = nanosleep(&a, NULL)) != 0) {
    fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
  }
  return (0);
}

void i2c_read_reg(int handle, int addr, unsigned char reg, unsigned char *buf,
                  size_t len) {
  i2c_write_read(handle, addr, &reg, 1, addr, buf, len);
}

void i2c_write_reg(int handle, int addr, int reg, int val) {
  if (ioctl(handle, I2C_SLAVE, addr) < 0) {
    perror("Failed to change I2C Address: ");
    return;
  }
  printf("[0x%X] Writing 0x%X to register 0x%X\n", addr, val, reg);
  char pair[2];
  pair[0] = reg & 0xFF;
  pair[1] = val & 0xFF;
  i2c_write(handle, (unsigned char *)pair, 2);
}

#else

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

int i2c_open(unsigned char bus, unsigned char addr) { return -1; }

int i2c_write(int handle, unsigned char *buf, unsigned int length) { return 0; }

int i2c_write_byte(int handle, unsigned char val) { return 0; }

int i2c_read(int handle, unsigned char *buf, unsigned int length) { return 0; }

int i2c_read_byte(int handle, unsigned char *val) { return 0; }

int i2c_close(int handle) { return 0; }

int i2c_write_read(int handle, unsigned char addr_w, unsigned char *buf_w,
                   unsigned int len_w, unsigned char addr_r,
                   unsigned char *buf_r, unsigned int len_r) {

  return 0;
}

int i2c_write_ignore_nack(int handle, unsigned char addr_w, unsigned char *buf,
                          unsigned int length) {
  return 0;
}

int i2c_read_no_ack(int handle, unsigned char addr_r, unsigned char *buf,
                    unsigned int length) {

  return 0;
}

void i2c_read_reg(int handle, int addr, unsigned char reg, unsigned char *buf,
                  size_t len) {}

void i2c_write_reg(int handle, int addr, int reg, int val) {}
#pragma clang diagnostic pop
#endif
