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

/**
 * Common C Definitions File for OPENLOOP pod controller
 *
 * This file is responsible for simply including system library headers
 */
#ifndef _OPENLOOP_POD_CDEFS_
#define _OPENLOOP_POD_CDEFS_

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdint.h>
#include <signal.h>
#include <semaphore.h>

#define within(low, val, high) ((low <= val) && (val <= high))
#define outside(low, val, high) (!(within((low), (val), (high))))

#define MIN(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

#define MAX(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

#define __STR__(s) #s
#define __XSTR__(s) __STR__(s)

#define assertUInt32Addition(a, b) assert(!((a >> 31) && (b >> 31)))

#endif
