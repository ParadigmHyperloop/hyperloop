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

struct arguments {
  bool tests;
  bool ready;
};

struct arguments args = {0};

/**
 * WARNING: Do Not Directly Access this struct, use get_pod_state() instead to
 * get a pointer to the pod state.
 */
extern pod_state_t __state;
extern int serverfd;
extern int imufd;
extern int clients[MAX_CMD_CLIENTS];
extern int nclients;

void *core_main(void *arg);
void *logging_main(void *arg);
void *command_main(void *arg);

void usage() {
  fprintf(stderr, "Usage: core [-r] [-t]");
  exit(1);
}

void parse_args(int argc, char *argv[]) {
  int ch;

  while ((ch = getopt(argc, argv, "rt")) != -1) {
    switch (ch) {
    case 'r':
      args.ready = true;
      break;
    case 't':
      args.tests = true;
      break;
    default:
      usage();
    }
  }
  // argc -= optind;
  // argv += optind;
}

void set_pthread_priority(pthread_t task, int priority) {
  struct sched_param sp;
  sp.sched_priority = priority;

  // Set thread scheduling mode to Round-Robin
  if (pthread_setschedparam(task, SCHED_RR, &sp)) {
    fprintf(stderr, "WARNING: failed to set thread"
                    "to real-time priority\n");
  }
}

/**
 * Wrapper for exit() function. Allows us to dump a constant state on exit
 * Note: atexit handlers don't always work (expecially if exiting in a signal
 * handler)
 */
void pod_exit(int code) {
  fprintf(stderr, "=== POD IS SHUTTING DOWN NOW! ===\n");

  fprintf(stderr, "Closing IMU (fd %d)\n", imufd);
  imu_disconnect(imufd);

  while (nclients > 0) {
    fprintf(stderr, "Closing client %d (fd %d)\n", nclients, clients[nclients]);
    close(clients[nclients]);
    nclients--;
  }

  fprintf(stderr, "Closing command server (fd %d)\n", serverfd);
  close(serverfd);
  exit(code);
}

/**
 * Panic Signal Handler.  This is only called if the shit has hit the fan
 * This function fires whenever the controller looses complete control in itself
 *
 * The controller sets the EBRAKE pins to LOW (engage) and then kills all it's
 * threads.  This is done to prevent threads from toggling the Ebrake pins OFF
 * for whatever reason.
 *
 * This is a pretty low level function because it is attempting to cut out the
 * entire controller logic and just make the pod safe
 */
void signal_handler(int sig) {
  __state.mode = Emergency;

  // Manually make the pod safe
  int ebrake_pins[] = EBRAKE_PINS;

  // Set all the ebrake pins to 0
  for (int i = 0; i < sizeof(ebrake_pins) / sizeof(int); i++) {
    fprintf(stderr, "[PANIC] Forcing Pin %d => 0\n", ebrake_pins[i]);
    // TODO: ReEnable This:
    // digitalWrite(ebrake_pins[i], 0);
  }

  exit(EXIT_FAILURE);
}

void exit_signal_handler(int sig) {
#ifdef TESTING
  pod_exit(2);
#else
  switch (__state.mode) {
  case Boot:
  case Shutdown:
    error("Exiting by signal %d", sig);
    pod_exit(1);
  default:
    set_pod_mode(Emergency, "Recieved Signal %d", sig);
  }
#endif
}

void sigpipe_handler(int sig) { error("SIGPIPE Recieved"); }

int main(int argc, char *argv[]) {
  int boot_sem_ret = 0;

  parse_args(argc, argv);

  info("POD Booting...");
  info("Initializing Pod State");

  if (init_pod_state() < 0) {
    fprintf(stderr, "Failed to Initialize Pod State");
    pod_exit(1);
  }

  info("Loading POD state struct for the first time");
  pod_state_t *state = get_pod_state();

  info("Setting Up Pins");

#ifdef BBB
  setupPins(state);
  if (args.tests) {
    selfTest(state);
  }
#endif

  info("Registering POSIX signal handlers");
  // Pod Panic Signal
  signal(POD_SIGPANIC, signal_handler);

  // TCP Server can generate SIGPIPE signals on disconnect
  // TODO: Evaluate if this should trigger an emergency
  signal(SIGPIPE, sigpipe_handler);

  // Signals that should trigger soft shutdown
  signal(SIGINT, exit_signal_handler);
  signal(SIGTERM, exit_signal_handler);
  signal(SIGHUP, exit_signal_handler);

  // -----------------------------------------
  // Logging - Remote Logging System
  // -----------------------------------------
  info("Starting the Logging Client Connection");
  pthread_create(&(state->logging_thread), NULL, logging_main, NULL);

  // Wait for logging thread to connect to the logging server
  if (!args.ready) {
    boot_sem_ret = sem_wait(state->boot_sem);
    if (boot_sem_ret != 0) {
      perror("sem_wait wait failed: ");
      pod_exit(1);
    }
  }

  if (get_pod_mode() != Boot) {
    error("Remote Logging thread has requested shutdown, See log for details");
    pod_exit(1);
  }

  // -----------------------------------------
  // Commander - Remote Command Communication
  // -----------------------------------------
  info("Booting Command and Control Server");
  pthread_create(&(state->cmd_thread), NULL, command_main, NULL);

  // Wait for command thread to start it's server
  if (!args.ready) {
    boot_sem_ret = sem_wait(state->boot_sem);
    if (boot_sem_ret != 0) {
      perror("sem_wait wait failed: ");
      pod_exit(1);
    }
  }

  // Assert State is still boot
  if (get_pod_mode() != Boot) {
    error("Command thread has requested shutdown, See log for details");
    pod_exit(1);
  }

  info("Booting Core Controller Logic Thread");
  pthread_create(&(state->core_thread), NULL, core_main, NULL);

  // we're using the built-in linux Round Roboin scheduling
  // priorities are 1-99, higher is more important
  // important note: this is not hard real-time
  set_pthread_priority(state->core_thread, 70);
  set_pthread_priority(state->logging_thread, 10);
  set_pthread_priority(state->cmd_thread, 20);

  pthread_join(state->core_thread, NULL);

  // TODO: Clean this up
  error("Core thread joined");
  exit(1);

  pthread_join(state->logging_thread, NULL);
  pthread_join(state->cmd_thread, NULL);

  return 0;
}
