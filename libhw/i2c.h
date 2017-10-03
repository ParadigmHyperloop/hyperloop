//
//  i2c.h
//  pod
//
//  Created by Eddie Hurtig on 8/18/17.
//
//

#ifndef i2c_h
#define i2c_h

#include "hw.h"

/************************************
 * i2cfunc.h v3
 * func prototypes for I2C functions
 ************************************/

// bus=1 for interface I2C2 on BBB
// returns handle to be used in remainder functions
// addr is a 7-bit value (so, for example the BMP085 datasheet specifies
// 0xEE, so we need to right-shift by 1 and use 0x77 for this function)
int i2c_open(unsigned char bus, unsigned char addr);

// These functions return -1 on error, otherwise return the number of bytes
// read/written.
// To perform a 'repeated start' use the i2c_write_read function which can write
// some
// data and then immediately read data without a stop bit in between.
int i2c_write(int handle, unsigned char *buf, unsigned int length);
int i2c_read(int handle, unsigned char *buf, unsigned int length);
int i2c_write_read(int handle, unsigned char addr_w, unsigned char *buf_w,
                   unsigned int len_w, unsigned char addr_r,
                   unsigned char *buf_r, unsigned int len_r);
int i2c_write_ignore_nack(int handle, unsigned char addr_w, unsigned char *buf,
                          unsigned int length);
int i2c_read_no_ack(int handle, unsigned char addr_r, unsigned char *buf,
                    unsigned int length);
int i2c_write_byte(int handle, unsigned char val);
int i2c_read_byte(int handle, unsigned char *val);

// These functions return -1 on error, otherwise return 0 on success
int i2c_close(int handle);

void i2c_read_reg(int handle, int addr, unsigned char reg, unsigned char *buf,
                  size_t len);

int i2c_write_reg(int handle, int addr, int reg, int val);

int set_ssr(int handle, int addr, int channel, int value);

#endif /* i2c_h */
