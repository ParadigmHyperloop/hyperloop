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

#ifndef OPENLOOP_COMMANDS_H
#define OPENLOOP_COMMANDS_H

#include <stdint.h>

typedef struct {
  char *name;
  int (*func)(size_t argc, char *argv[], size_t outbufc, char outbuf[]);
} command_t;

#endif
