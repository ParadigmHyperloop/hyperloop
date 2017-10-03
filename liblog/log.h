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

#ifndef OPENLOOP_POD_LOG_H
#define OPENLOOP_POD_LOG_H
#include <stdio.h>
#include <stdint.h>
#include <sys/queue.h>
#include <unistd.h>
#include <stdarg.h>

#ifndef __unused
#define __unused  __attribute__((unused))
#endif
#ifndef __printflike
#define __printflike(a, b) __attribute__((format(printf, (a), (b))))
#endif

#include "ring_buffer.h"

#ifndef PACKET_INTERVAL
#define PACKET_INTERVAL USEC_PER_SEC / 10 // Delay between sending packets
#endif

#define MAX_LOGS 32
#define MAX_LOG_SIZE 512

#define LOG_FILE_PATH "./hyperloop-core.log"

#define TELEMETRY_PACKET_VERSION 2

typedef enum {
  Message = 1,
  Telemetry_float = 2,
  Telemetry_int32 = 3,
  Packet = 4
} log_type_t;

typedef struct {
  char name[64];
  float value;
} log_float_data_t;

typedef struct {
  char name[64];
  int32_t value;
} log_int32_data_t;

typedef uint16_t relay_mask_t;

typedef struct log {
  log_type_t type;
  char data[MAX_LOG_SIZE];
  size_t sz;
  STAILQ_ENTRY(log) entries;
} log_t;


/**
 * Enqueue a telemetry packet for network transmission of the current state
 */
int log_enqueue(log_t *l);

/**
 * Log a standard message to stdout and a log file
 */
__printflike(1, 2)
int pod_log(char *fmt, ...);

/**
 * Main entry point into the logging server. Use with pthread_create
 */
void *logging_main(__unused void *arg);

#endif
