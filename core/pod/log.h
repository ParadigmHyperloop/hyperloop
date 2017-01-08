#ifndef OPENLOOP_POD_LOG_H
#define OPENLOOP_POD_LOG_H
#include "../pod.h"

typedef enum { Message = 1, Telemetry_float = 2, Telemetry_int32 = 3, Packet = 4 } log_type_t;

typedef struct {
  char name[64];
  float value;
} log_float_data_t;

typedef struct {
  char name[64];
  int32_t value;
} log_int32_data_t;


typedef uint16_t solenoid_mask_t;

typedef struct telemetry_packet {
  uint8_t version;
  uint16_t length;
  // state
  pod_mode_t state;
  // Solenoids
  solenoid_mask_t solenoids;
  uint64_t timestamp;
  // IMU
  float position_x;
  float position_y;
  float position_z;

  float velocity_x;
  float velocity_y;
  float velocity_z;

  float acceleration_x;
  float acceleration_y;
  float acceleration_z;

  // Distance sensors
  float corners[N_CORNER_DISTANCE];
  float wheels[N_CORNER_DISTANCE];
  float lateral[N_CORNER_DISTANCE];

  // Pressures
  float hp_pressure;
  float reg_pressure[N_REG_PRESSURE];
  float clamp_pressure[N_CLAMP_PRESSURE];
  float skate_pressure[N_SKATE_PRESSURE];

  // Thermocouples
  float hp_thermo;
  float reg_thermo[N_REG_THERMO];
  float reg_surf_thermo[N_REG_SURF_THERMO];
  float power_thermo[POWER_THERMO_MUX];
  float frame_thermo;

  // Photo
  float rpms[N_WHEEL_PHOTO];
  float stripe_count;
} telemetry_packet_t;

typedef struct log {
  log_type_t type;
  union {
    char message[MAX_LOG_LINE];
    log_float_data_t float_data;
    log_int32_data_t int32_data;
    telemetry_packet_t packet;
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
