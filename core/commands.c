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

#include "pod.h"
#include "pod-helpers.h"
#include "commands.h"

extern char *pod_mode_names[];

int helpCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  int count = snprintf(
      &outbuf[0], outbufc, "%s",
      "OpenLoop Pod CLI " POD_CLI_VERSION ". Copyright " POD_COPY_YEAR "\n"
      "This tool allows you to control various aspects of the pod\n"
      " - TCP:" __XSTR__(CMD_SVR_PORT) "\n"
                                       " - STDIN\n"
                                       "\n"
                                       "Available Commands:\n"
                                       " - help\n"
                                       " - ping\n"
                                       " - ready\n"
                                       " - brake\n"
                                       " - fill\n"
                                       " - skate\n"
                                       " - status\n"
                                       " - offset\n"
                                       " - calibrate\n"
                                       " - reset\n"
                                       " - emergency (alias: e)\n"
                                       " - exit\n"
                                       " - kill\n");
  return count;
}

int pingCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_t *pod = get_pod();
  pod->last_ping = get_time();
  return snprintf(&outbuf[0], outbufc, "PONG");
}

int calibrateCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_calibrate();
  pod_reset();
  return snprintf(&outbuf[0], outbufc, "CALIBRATION SET");
}

int resetCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_reset();
  return snprintf(&outbuf[0], outbufc, "OK");
}

int readyCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  int n;

  int32_t ready = get_value(&(pod->ready));
  if (ready == 0) {
    set_value(&(pod->ready), 1);
    n = snprintf(&outbuf[0], outbufc, "OK: SET POD READY BIT => 1");
  } else {
    n = snprintf(&outbuf[0], outbufc, "FAIL: POD READY BIT = %d, POD MODE = %d",
                 ready, get_pod_mode());
  }

  return n;
}

int statusCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_t *pod = get_pod();
  int i, c = 0;
  c += snprintf(&outbuf[0], outbufc, "=== STATUS REPORT ===\n"
                                     "State:\t%s\n"
                                     "Ready:\t%d\n"
                                     "Core:\t%f loops/sec\n"
                                     "Ax:\t%f\n"
                                     "Vx:\t%f\n"
                                     "Px:\t%f\n",
                pod_mode_names[get_pod_mode()], get_value(&(pod->ready)),
                get_value_f(&(pod->core_speed)), get_value_f(&(pod->accel_x)),
                get_value_f(&(pod->velocity_x)),
                get_value_f(&(pod->position_x)));

  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    c += snprintf(
        &outbuf[c], outbufc - c, "Skate %d:\t%s\n", i,
        (is_solenoid_open(&(pod->skate_solonoids[i])) ? "open" : "closed"));
  }

  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    c += snprintf(
        &outbuf[c], outbufc - c, "Caliper %d:\t%s\n", i,
        (is_solenoid_open(&(pod->wheel_solonoids[i])) ? "open" : "closed"));
  }

  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    c += snprintf(
        &outbuf[c], outbufc - c, "Clamp %d:\t%s\n", i,
        (is_solenoid_open(&(pod->ebrake_solonoids[i])) ? "open" : "closed"));
  }

  for (i = 0; i < N_LP_FILL_SOLENOIDS; i++) {
    c += snprintf(
        &outbuf[c], outbufc - c, "LP Fill %d:\t%s\n", i,
        (is_solenoid_open(&(pod->lp_fill_valve[i])) ? "open" : "closed"));
  }

  c += snprintf(&outbuf[c], outbufc - c, "HP Fill:\t%s\n",
                (is_solenoid_open(&(pod->hp_fill_valve)) ? "open" : "closed"));

  c += snprintf(&outbuf[c], outbufc - c, "LP Vent:\t%s\n",
                (is_solenoid_open(&(pod->vent_solenoid)) ? "open" : "closed"));

  return c;
}

int fillCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  if (argc == 2) {
    if (strncmp(argv[1], "lp", 2) == 0) {
      if (start_lp_fill()) {
        return snprintf(outbuf, outbufc, "Entered LP Fill State");
      } else {
        return snprintf(outbuf, outbufc, "LP Fill Pre-Check Failure");
      }
    } else if (strncmp(argv[1], "hp", 2) == 0) {
      if (start_hp_fill()) {
        return snprintf(outbuf, outbufc, "Entered HP Fill State");
      } else {
        return snprintf(outbuf, outbufc, "HP Fill Pre-Check Failure");
      }
    }
  }
  return snprintf(outbuf, outbufc, "Usage: fill <lp|hp>");
}

int overrideCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  if (argc < 3) {
    return snprintf(outbuf, outbufc,
                    "Usage: override <surface> [<number>] <new_value>%d",
                    get_pod_mode());
  }
  if (strncmp(argv[1], "skate", 5)) {
    if (argv[2][0] == '-') {
      override_surface(SKATE_OVERRIDE_ALL, false);
      return snprintf(outbuf, outbufc, "Skate Overrides Cleared\n");
    } else {
      override_surface(SKATE_OVERRIDE_ALL, true);

      if (argc == 3) {
        int i;
        int val = atoi(argv[2]);

        for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
          set_skate_target(i, val, true);
        }
      } else if (argc == 4) {
        int i = atoi(argv[2]);
        int val = atoi(argv[3]);
        set_skate_target(i, val, true);
      }
    }
  } else if (strncmp(argv[1], "brake", 5)) {
  }
  return 0;
}

int offsetCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  if (argc < 4) {
    return snprintf(outbuf, outbufc,
                    "Usage: offset <sensor> <number> <offset>%d",
                    get_pod_mode());
  }

  int no = atoi(argv[2]);
  int offset = atoi(argv[3]);
  sensor_t *sensor = NULL;

  if (strncmp(argv[1], "skate_transducer", 16)) {
    sensor = &(pod->skate_transducers[no]);
  } else if (strncmp(argv[1], "lp_transducer", 13)) {
    sensor = &(pod->lp_reg_transducers[no]);
  } else if (strncmp(argv[1], "hp_transducer", 13)) {
    sensor = &(pod->hp_transducer);
  } else if (strncmp(argv[1], "clamp_transducer", 16)) {
    sensor = &(pod->ebrake_transducers[no]);
  } else if (strncmp(argv[1], "lat_transducer", 14)) {
    sensor = &(pod->lateral_fill_transducers[no]);
  }

  sensor->offset = offset;
  return 0;
}

int emergencyCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  set_pod_mode(Emergency, "Command Line Initialized Emergency");
  return snprintf(outbuf, outbufc, "Pod Mode: %d", get_pod_mode());
}

int exitCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  if (argc > 1) {
    pod_exit(atoi(argv[1]));
  } else {
    pod_exit(1);
  }
  return -1;
}

int killCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  panic(POD_COMMAND_SUBSYSTEM, "Command Line Initiated Kill Command");
  return -1;
}

// You must keep this list in order from Longest String to Shortest,
// Doesn't matter the order umongst names of equal length.
// Has to deal with how commands are located, where "e" undercuts any command
// that starts with "e", like "exit"
command_t commands[] = {{.name = "emergency", .func = emergencyCommand},
                        {.name = "status", .func = statusCommand},
                        {.name = "override", .func = overrideCommand},
                        {.name = "offset", .func = offsetCommand},
                        {.name = "ready", .func = readyCommand},
                        {.name = "help", .func = helpCommand},
                        {.name = "fill", .func = fillCommand},
                        {.name = "ping", .func = pingCommand},
                        {.name = "exit", .func = exitCommand},
                        {.name = "kill", .func = killCommand},
                        {.name = "calibrate", .func = calibrateCommand},
                        {.name = "reset", .func = resetCommand},
                        {.name = "e", .func = emergencyCommand},
                        {.name = NULL}};
