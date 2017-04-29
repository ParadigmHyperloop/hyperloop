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

#include "commands.h"
#include "pod.h"

extern char *pod_mode_names[];

command_t commands[];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
int helpCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  int count =
      snprintf(&outbuf[0], outbufc, "%s",
               "Pod CLI " POD_CLI_VERSION_STR ". Copyright " POD_COPY_YEAR
               " " POD_COPY_OWNER "\n" POD_CREDITS
               "This tool allows you to control various aspects of the pod\n"
               " - TCP:" __XSTR__(CMD_SVR_PORT) "\n - STDIN\n\n"
                                                "Available Commands:\n");
  
  command_t *command = &commands[0];
  while (command->name) {
    count += snprintf(&outbuf[count], outbufc, " - %s\n", command->name);
    command++;
  }

  return count;
}

int pingCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();
  pod->last_ping = get_time_usec();
  return snprintf(&outbuf[0], outbufc, "PONG:%d", get_pod_mode());
}

int calibrateCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();
  pod_calibrate();
  pod_reset();
  return snprintf(&outbuf[0], outbufc, "CALIBRATION SET\nX: %f\nY: %f\nZ: %f\n",
                  get_value_f(&(pod->imu_calibration_x)),
                  get_value_f(&(pod->imu_calibration_y)),
                  get_value_f(&(pod->imu_calibration_z)));
}

int resetCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_reset();
  return snprintf(&outbuf[0], outbufc, "Reseting Pod %s", get_pod()->name);
}

int readyCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
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

int armCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();
  if (get_value(&(pod->pusher_plate)) == 1) {
    return snprintf(outbuf, outbufc,
                    "ERROR: PUSHER PLATE DEPRESSED CANNOT ARM");
  }

  if (!core_pod_checklist(pod)) {
    return snprintf(outbuf, outbufc, "Pod not ready to arm. core checklist");
  }

  set_pod_mode(Armed, "Remote Command Armed Pod");
  return snprintf(outbuf, outbufc, "Armed");
}

int ventCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  if (is_pod_stopped(pod)) {
    return snprintf(outbuf, outbufc, "Venting Started");
  } else {
    return snprintf(
        outbuf, outbufc,
        "Pod Not Determined to be Stopped, override solenoid to vent");
  }
}

int statusCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  return status_dump(pod, outbuf, outbufc);
}

int fillCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
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

int overrideCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
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

int offsetCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  if (argc < 3) {
    return snprintf(outbuf, outbufc, "Usage: offset <sensor> <offset>");
  }
  sensor_t *sensor = get_sensor_by_name(pod, argv[1]);

  if (sensor == NULL) {
    return snprintf(outbuf, outbufc, "Sensor '%s' not found", argv[1]);
  }

  double offset = atoi(argv[2]);

  double old_offset = sensor->offset;
  sensor->offset = offset;
  return snprintf(outbuf, outbufc,
                  "Changed offset of %s: %lf -> %lf. Reading: %f", sensor->name,
                  old_offset, offset, get_sensor(sensor));
}

int emergencyCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  set_pod_mode(Emergency, "Command Line Initialized Emergency");
  return snprintf(outbuf, outbufc, "Pod Mode: %d", get_pod_mode());
}

int exitCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  if (argc > 1) {
    pod_exit(atoi(argv[1]));
  } else {
    pod_exit(1);
  }
  return -1;
}

int killCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  panic(POD_COMMAND_SUBSYSTEM, "Command Line Initiated Kill Command");
  return -1;
}

int pushCommand(size_t argc, char *argv[], size_t outbufc, char outbuf[]) {
  pod_t *pod = get_pod();

  if (argc > 1) {
    pod->pusher_plate_override = 1;
    set_value(&(pod->pusher_plate), atoi(argv[1]));
    return snprintf(outbuf, outbufc, "Set Pusher plate override to %s",
                    (atoi(argv[1]) == 1 ? "ACTIVE" : "INACTIVE"));
  } else {
    if (pod->pusher_plate_override == 1) {
      pod->pusher_plate_override = 0;
      return snprintf(outbuf, outbufc, "Disabled Pusher Plate Override");
    } else {
      return snprintf(outbuf, outbufc, "No Pusher Plate Override In Effect");
    }
  }
}

// You must keep this list in order from Longest String to Shortest,
// Doesn't matter the order amongst names of equal length.
// Has to deal with how commands are located, where "e" undercuts any command
// that starts with "e", like "exit"
command_t commands[] = {{.name = "emergency", .func = emergencyCommand},
                        {.name = "calibrate", .func = calibrateCommand},
                        {.name = "override", .func = overrideCommand},
                        {.name = "status", .func = statusCommand},
                        {.name = "offset", .func = offsetCommand},
                        {.name = "ready", .func = readyCommand},
                        {.name = "reset", .func = resetCommand},
                        {.name = "vent", .func = ventCommand},
                        {.name = "help", .func = helpCommand},
                        {.name = "fill", .func = fillCommand},
                        {.name = "ping", .func = pingCommand},
                        {.name = "exit", .func = exitCommand},
                        {.name = "push", .func = pushCommand},
                        {.name = "kill", .func = killCommand},
                        {.name = "arm", .func = armCommand},
                        {.name = "e", .func = emergencyCommand},
                        {.name = NULL}};
#pragma clang diagnostic pop
