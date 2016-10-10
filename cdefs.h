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

#define within(low, val, high) ((low <= val) && (val <= high))
#define outside(low, val, high) (!(within((low), (val), (high))))

#define __STR__(s) #s

#endif
