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

#include <sys/time.h>


#define within(low, val, high) ((low <= val) && (val <= high))
#define outside(low, val, high) (!(within((low), (val), (high))))


#endif
