/**
 * Credit: https://github.com/osudrl/atrias/blob/master/firmware/atrias2.1/src/crc.c
 * Original Source License https://opensource.org/licenses/BSD-3-Clause
 * Original Source Page: https://code.google.com/archive/p/atrias/
 */
#ifndef _CRC_H
#define _CRC_H

#include <stdio.h>
#include <stdint.h>

typedef uint32_t crc_t;   /* KVH IMU has 32-bit CRC */

void crc_generate_table(void);
crc_t crc_calc(uint8_t *packet, uint8_t num_bytes);

#endif /* _CRC_H */
