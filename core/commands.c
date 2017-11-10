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
#include <getopt.h>

extern char *pod_mode_names[];

command_t commands[];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

static int helpCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
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

static int pingCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  pod_t *pod = get_pod();
  pod->last_ping = get_time_usec();
  return snprintf(&outbuf[0], outbufc, "PONG:%d", get_pod_mode());
}

static int calibrateCommand(size_t argc, char *argv[], size_t outbufc,
                            char outbuf[]) {
  pod_t *pod = get_pod();
  pod_calibrate();
  return snprintf(&outbuf[0], outbufc, "CALIBRATION SET\nX: %f\nY: %f\nZ: %f\n",
                  get_value_f(&(pod->imu_calibration_x)),
                  get_value_f(&(pod->imu_calibration_y)),
                  get_value_f(&(pod->imu_calibration_z)));
}

static int resetCommand(size_t argc, char *argv[], size_t outbufc,
                        char outbuf[]) {
  if (pod_reset()) {
    return snprintf(&outbuf[0], outbufc, "Reseting Pod %s", get_pod()->name);
  } else {
    return snprintf(&outbuf[0], outbufc, "Reset Request Declined %s",
                    get_pod()->name);
  }
}

static int readyCommand(size_t argc, char *argv[], size_t outbufc,
                        char outbuf[]) {
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

static int armCommand(size_t argc, char *argv[], size_t outbufc,
                      char outbuf[]) {
  pod_t *pod = get_pod();
  if (get_value(&(pod->pusher_plate)) == 0) {
    return snprintf(outbuf, outbufc,
                    "Pusher plate is not depressed. Cannot Arm.");
  }

  if (!core_pod_checklist(pod)) {
    return snprintf(outbuf, outbufc,
                    "Pod not ready to arm. core checklist failed.");
  }

  if (set_pod_mode(Armed, "Remote Command Armed Pod")) {
    return snprintf(outbuf, outbufc, "Armed");
  } else {
    return snprintf(outbuf, outbufc, "Controller declined to transition to "
                                     "Armed State. Ensure pod is in Standby.");
  }
}

static int ventCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  pod_t *pod = get_pod();
  pod->return_to_standby = false;
  if (set_pod_mode(Vent, "Remote Command started Vent")) {
    return snprintf(outbuf, outbufc, "Venting Started");
  } else {
    return snprintf(outbuf, outbufc, "Pod Not Determined to be Stopped");
  }
}

static int statusCommand(size_t argc, char *argv[], size_t outbufc,
                         char outbuf[]) {
  pod_t *pod = get_pod();
  return status_dump(pod, outbuf, outbufc);
}

static int fillCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  if (start_hp_fill()) {
    return snprintf(outbuf, outbufc, "Entered HP Fill State");
  } else {
    return snprintf(outbuf, outbufc, "HP Fill Pre-Check Failure");
  }
}

static int standbyCommand(size_t argc, char *argv[], size_t outbufc,
                          char outbuf[]) {
  if (start_standby()) {
    return snprintf(outbuf, outbufc, "Entered Standby");
  } else {
    return snprintf(outbuf, outbufc, "Failed to enter Standby");
  }
}

// static int returnToStandbyCommand(size_t argc, char *argv[], size_t outbufc,
// char outbuf[]) {
//  if (argc < 2) {
//    return snprintf(outbuf, outbufc, "usage: returntostandby <0|1>");
//  }
//
//  int value = atoi(argv[0]);
//  get_pod()->return_to_standby = value;
//
//  return snprintf(outbuf, outbufc, "Set return_to_standby to %d", value);
//}

static int overrideCommand(size_t argc, char *argv[], size_t outbufc,
                           char outbuf[]) {
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
          // Map -100 to 100 logic to raw MPYE setpoint.
          set_skate_target(i, (unsigned short)(val * 1275 / 1000) + 128, true);
        }
      } else if (argc == 4) {
        int i = atoi(argv[2]);
        int val = atoi(argv[3]);
        set_skate_target(i, (unsigned short)(val * 1275 / 1000) + 128, true);
      }
    }
  } else if (strncmp(argv[1], "brake", 5)) {
  }
  return 0;
}

static int offsetCommand(size_t argc, char *argv[], size_t outbufc,
                         char outbuf[]) {
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

static int packCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  pod_t *pod = get_pod();

  if (argc < 3) {
    return snprintf(outbuf, outbufc, "Usage: pack <pack> <0|1>");
  }

  int pack = atoi(argv[1]);
  int on_off = atoi(argv[2]);

  solenoid_t *s = &pod->battery_pack_relays[pack];
  if (on_off == 0) {
    close_solenoid(s);
  } else {
    open_solenoid(s);
  }

  return snprintf(outbuf, outbufc, "Set %s to %d", s->name, on_off);
}

static int emergencyCommand(size_t argc, char *argv[], size_t outbufc,
                            char outbuf[]) {
  set_pod_mode(Emergency, "Command Line Initialized Emergency");
  get_pod()->manual_emergency = true;
  return snprintf(outbuf, outbufc, "Pod Mode: %d", get_pod_mode());
}

static int stateCommand(size_t argc, char *argv[], size_t outbufc,
                        char outbuf[]) {
  pod_mode_t new_mode = NonState;
  if (argc == 1) {
    // Nothing, proceed to print
  } else if (argc == 2) {
    for (int i = 0; i < N_POD_STATES - 1; i++) {
      if (strncmp(pod_mode_names[i], argv[1], strlen(pod_mode_names[i])) == 0) {
        new_mode = (pod_mode_t)i;
        break;
      }
    }

    if (new_mode == NonState) {
      errno = 0;
      int m = (pod_mode_t)atoi(argv[1]);
      if (errno != EINVAL) {
        new_mode = (pod_mode_t)m;
      }
    }

    if (new_mode < 0 && new_mode >= N_POD_STATES) {
      return snprintf(outbuf, outbufc, "Invalid Mode: %s", argv[0]);
    }

    force_pod_mode(new_mode, "Remote Command Changed State");
  } else {
    return snprintf(outbuf, outbufc, "Usage: %s <state>", argv[0]);
  }

  new_mode = get_pod_mode();
  return snprintf(outbuf, outbufc, "Pod Mode: %d (%s)", new_mode,
                  pod_mode_names[new_mode]);
}

static int manualCommand(size_t argc, char *argv[], size_t outbufc,
                         char outbuf[]) {
  // TODO: Implement using set_pod_mode()
  pod_t *pod = get_pod();
  manual_config_t *c = &pod->manual;
  if (argc == 1) {
    // Nothing, just return the printout of the current manual setpoint
  } else if (argc == 15) {
    c->front_brake = atoi(argv[1]);
    c->rear_brake = atoi(argv[2]);
    c->vent = atoi(argv[3]);
    c->fill = atoi(argv[4]);
    c->battery_a = atoi(argv[5]);
    c->battery_b = atoi(argv[6]);
    c->skate_a = atoi(argv[7]);
    c->skate_b = atoi(argv[8]);
    c->skate_c = atoi(argv[9]);
    c->skate_d = atoi(argv[10]);
    c->mpye_a = (atoi(argv[11]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) +
                MPYE_A_SETPOINT;
    c->mpye_b = (atoi(argv[12]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) +
                MPYE_A_SETPOINT;
    c->mpye_c = (atoi(argv[13]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) +
                MPYE_A_SETPOINT;
    c->mpye_d = (atoi(argv[14]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) +
                MPYE_A_SETPOINT;
  } else {
    return snprintf(outbuf, outbufc, "Usage: %s [setpoints]", argv[0]);
  }

  return snprintf(outbuf, outbufc, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                  c->front_brake, c->rear_brake, c->vent, c->fill, c->battery_a,
                  c->battery_b, c->skate_a, c->skate_b, c->skate_c, c->skate_d,
                  c->mpye_a, c->mpye_b, c->mpye_c, c->mpye_d);
}

static int exitCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  if (argc > 1) {
    pod_exit(atoi(argv[1]));
  } else {
    pod_exit(1);
  }
  return -1;
}

static int killCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  panic(POD_COMMAND_SUBSYSTEM, "Command Line Initiated Kill Command");
  return -1;
}

static int pushCommand(size_t argc, char *argv[], size_t outbufc,
                       char outbuf[]) {
  return snprintf(outbuf, outbufc, "Push Command Not Allowed");
}

static int flightProfileGetCommand(size_t argc, char *argv[], size_t outbufc,
                                   char outbuf[]) {
  // Returns flight profile information
  pod_t *pod = get_pod();
  flight_profile_t *profile = &pod->flight_profile;
  return snprintf(outbuf, outbufc, "watchdog_timer: %i\n"
                                   "emergency_hold: %i\n"
                                   "braking_wait: %i\n"
                                   "pusher_timeout: %i\n"
                                   "pusher_state_accel_min: %f\n"
                                   "pusher_state_min_timer: %i\n"
                                   "pusher_distance_min: %f\n"
                                   "primary_braking_accel_min: %f\n",
                  profile->watchdog_timer, profile->emergency_hold,
                  profile->braking_wait, profile->pusher_timeout,
                  profile->pusher_state_accel_min,
                  profile->pusher_state_min_timer, profile->pusher_distance_min,
                  profile->primary_braking_accel_min);
}

static int flightProfileCommand(size_t argc, char *argv[], size_t outbufc,
                                char outbuf[]) {
  // Allows user to configure flight profiles
  pod_mode_t mode = get_pod_mode();
  if (mode == Boot) { // Only allowed in Boot mode
    // get current profile
    pod_t *pod = get_pod();
    flight_profile_t *profile = &pod->flight_profile;

    // Define options for getopt_long
    int opt = 0;
    static struct option long_options[] = {
        {"watchdog_timer", optional_argument, NULL, 'w'},
        {"emergency_hold", optional_argument, NULL, 'e'},
        {"braking_wait", optional_argument, NULL, 'r'},
        {"pusher_timeout", optional_argument, NULL, 't'},
        {"pusher_state_accel_min", optional_argument, NULL, 'a'},
        {"pusher_state_min_timer", optional_argument, NULL, 'm'},
        {"pusher_distance_min", optional_argument, NULL, 'd'},
        {"primary_braking_accel_min", optional_argument, NULL, 'b'},
        {NULL, 0, NULL, 0}};

    // Parse arguments
    int long_index = 0;
    optind = 0; // Resets index of argument to parse
    while ((opt = getopt_long(argc, argv, "w:e:r:t:a:m:d:b:", long_options,
                              &long_index)) != -1) {
      switch (opt) {
      case 'w':
        set_watchdog_timer(profile, atoi(optarg));
        break;
      case 'e':
        set_emergency_hold(profile, atoi(optarg));
        break;
      case 'r':
        set_braking_wait(profile, atoi(optarg));
        break;
      case 't':
        set_pusher_timeout(profile, atoi(optarg));
        break;
      case 'a':
        set_pusher_state_accel_min(profile, atof(optarg));
      case 'm':
        set_pusher_state_min_timer(profile, atoi(optarg));
        break;
      case 'd':
        set_pusher_distance_min(profile, atof(optarg));
        break;
      case 'b':
        set_primary_braking_accel_min(profile, atof(optarg));
        break;
      default:
        return snprintf(outbuf, outbufc, "Invalid Argument(s)\n");
      }
    }
    return flightProfileGetCommand(argc, argv, outbufc, outbuf);

  } else {
    return snprintf(outbuf, outbufc, "Cannot configure flight profiles in mode "
                                     "%d (%s). Pod must be in Boot mode.",
                    mode, pod_mode_names[mode]);
  }
}

// battery_pack_relays

// You must keep this list in order from Longest String to Shortest,
// Doesn't matter the order amongst names of equal length.
// Has to deal with how commands are located, where "e" undercuts any command
// that starts with "e", like "exit"
command_t commands[] = {{.name = "emergency", .func = emergencyCommand},
                        {.name = "calibrate", .func = calibrateCommand},
                        {.name = "override", .func = overrideCommand},
                        {.name = "standby", .func = standbyCommand},
                        {.name = "status", .func = statusCommand},
                        {.name = "manual", .func = manualCommand},
                        {.name = "offset", .func = offsetCommand},
                        {.name = "ready", .func = readyCommand},
                        {.name = "state", .func = stateCommand},
                        {.name = "reset", .func = resetCommand},
                        {.name = "fpget", .func = flightProfileGetCommand},
                        {.name = "vent", .func = ventCommand},
                        {.name = "pack", .func = packCommand},
                        {.name = "help", .func = helpCommand},
                        {.name = "fill", .func = fillCommand},
                        {.name = "ping", .func = pingCommand},
                        {.name = "exit", .func = exitCommand},
                        {.name = "push", .func = pushCommand},
                        {.name = "kill", .func = killCommand},
                        {.name = "arm", .func = armCommand},
                        {.name = "fp", .func = flightProfileCommand},
                        {.name = "e", .func = emergencyCommand},
                        {.name = NULL}};
#pragma clang diagnostic pop
