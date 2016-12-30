#ifndef OPENLOOP_POD_LOG_H
#define OPENLOOP_POD_LOG_H
#include "../pod.h"

typedef enum { Message = 1, Telemetry_float = 2, Telemetry_int32 } log_type_t;

typedef struct {
  char name[64];
  float value;
} log_float_data_t;

typedef struct {
  char name[64];
  int32_t value;
} log_int32_data_t;

typedef struct log {
  log_type_t type;
  union {
    char message[MAX_LOG_LINE];
    log_float_data_t float_data;
    log_int32_data_t int32_data;
  } v;
  STAILQ_ENTRY(log) entries;
} log_t;

/**
 * Sends the given message to all logging destinations
 */
int podLog(char *fmt, ...);

/**
 * Dump entire pod_t to the network logging buffer
 */
void logDump(pod_t *pod);

/**
 * Enqueue a telemetry packet for network transmission of the current state
 */
int logEnqueue(log_t *l);

#endif
