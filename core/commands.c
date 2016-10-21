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
#include "commands.h"

// You must keep this list in order from Longest String to Shortest,
// Doesn't matter the order umongst names of equal length.
// Has to deal with how commands are located, where "e" undercuts any command
// that starts with "e", like "exit"
command_t commands[] = {{.name = "emergency", .func = emergencyCommand},
                        {.name = "status", .func = statusCommand},
                        {.name = "skate", .func = skateCommand},
                        {.name = "override", .func = overrideCommand},
                        {.name = "ready", .func = readyCommand},
                        {.name = "brake", .func = brakeCommand},
                        {.name = "help", .func = helpCommand},
                        {.name = "ping", .func = pingCommand},
                        {.name = "exit", .func = exitCommand},
                        {.name = "kill", .func = killCommand},
                        {.name = "e", .func = emergencyCommand},
                        {.name = NULL}};

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
                                       " - skate\n"
                                       " - status\n"
                                       " - emergency (alias: e)\n"
                                       " - exit\n"
                                       " - kill\n");
  return count;
}

int pingCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return snprintf(&outbuf[0], outbufc, "PONG");
}

int readyCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_state_t *state = getPodState();

  int n;

  int32_t ready = getPodField(&(state->ready));
  if (ready == 0) {
    setPodField(&(state->ready), 1);
    n = snprintf(&outbuf[0], outbufc, "OK: SET POD READY BIT => 1");
  } else {
    n = snprintf(&outbuf[0], outbufc, "FAIL: POD READY BIT = %d, POD MODE = %d",
                 ready, getPodMode());
  }

  return n;
}

int statusCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  pod_state_t *state = getPodState();
  return snprintf(&outbuf[0], outbufc, "=== STATUS REPORT ===\n"
                                       "Mode:\t%d\n"
                                       "Ready:\t%d\n"
                                       "Ax:\t%f\n"
                                       "Vx:\t%f\n"
                                       "Px:\t%f\n",
                  getPodMode(), getPodField(&(state->ready)),
                  getPodField_f(&(state->accel_x)),
                  getPodField_f(&(state->velocity_x)),
                  getPodField_f(&(state->position_x)));
}

int brakeCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return 0;
}

int skateCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return 0;
}

int overrideCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  if (argc < 3) {
    return snprintf(outbuf, outbufc, "Usage: override <surface> [<number>] <new_value>%d", getPodMode());
  }
  if (strncmp(argv[1], "skate", 5)) {
    if (argv[2][0] == '-') {
      setManual(SKATE_OVERRIDE_ALL, true);
    } else {
      setManual(SKATE_OVERRIDE_ALL, true);

      if (argc == 3) {
        int i;
        int val = atoi(argv[2]);

        for (i=0;i<N_SKATE_SOLONOIDS;i++) {
          setSkates(i, val, true);
        }
      } else if (argc == 4){
        int i = atoi(argv[2]);
        int val = atoi(argv[3]);
        setSkates(i, val, true);
      }
    }
  } else if (strncmp(argv[1], "brake", 5)) {

  }
  return 0;
}

int emergencyCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  setPodMode(Emergency, "Command Line Initialized Emergency");
  return snprintf(outbuf, outbufc, "Pod Mode: %d", getPodMode());
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
