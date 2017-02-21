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

#include "pod.h"
#include "pru.h"

struct arguments {
  bool tests;
  bool ready;
  char *imu_device;
};

struct arguments args = {
    .tests = false, .ready = false, .imu_device = IMU_DEVICE};

/**
 * WARNING: Do Not Directly Access this struct, use get_pod() instead to
 * get a pointer to the pod.
 */
extern pod_t _pod;
extern int serverfd;
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

  while ((ch = getopt(argc, argv, "rti:")) != -1) {
    switch (ch) {
    case 'r':
      args.ready = true;
      break;
    case 't':
      args.tests = true;
      break;
    case 'i':
      args.imu_device = optarg;
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
 * Wrapper for exit() function. Allows us to exit cleanly
 * Note: atexit handlers don't always work (expecially if exiting in a signal
 * handler)
 */
void pod_exit(int code) {
  pod_t *pod = get_pod();
  fprintf(stderr, "=== POD IS SHUTTING DOWN NOW! ===\n");

  if (pod->imu > -1) {
    fprintf(stderr, "Closing IMU (fd %d)\n", pod->imu);
    imu_disconnect(pod->imu);
  }

  while (nclients > 0) {
    fprintf(stderr, "Closing client %d (fd %d)\n", nclients, clients[nclients]);
    close(clients[nclients]);
    nclients--;
  }

#ifdef HAS_PRU
  fprintf(stderr, "Shutting down PRU\n");
  pru_shutdown();
#endif

  fprintf(stderr, "Closing command server (fd %d)\n", serverfd);
  close(serverfd);
  exit(code);
}

/**
 * Panic Signal Handler.  This is only called if the shit has hit the fan
 * This function fires whenever the controller looses complete control in itself
 *
 * The controller sets the CLAMP pins to LOW (engage) and then kills all it's
 * threads.  This is done to prevent threads from toggling the Ebrake pins OFF
 * for whatever reason.
 *
 * This is a pretty low level function because it is attempting to cut out the
 * entire controller logic and just make the pod safe
 */
void signal_handler(int sig) {
  if (sig == SIGTERM) {
// Power button pulled low, power will be cut in < 1023ms
// TODO: Sync the filesystem and unmount root to prevent corruption

#ifdef __linux__
    FILE *fp = fopen("/proc/sys/kernel/sysrq", "w");
    fwrite("1", sizeof(char), 1, fp);
    fclose(fp);

    umount2("/", MNT_FORCE);
    setPinValue(KILL_PIN, KILL_PIN_KILL_VALUE);
    FILE *fp = fopen("/proc/sysrq-trigger", "w");
    fwrite("o", sizeof(char), 1, fp);
    fclose(fp);
#endif
  }
  exit(EXIT_FAILURE);
}

void exit_signal_handler(int sig) {
#ifdef POD_DEBUG
  pod_exit(2);
#else
  switch (_pod.mode) {
  case Boot:
  case Shutdown:
    error("Exiting by signal %d", sig);
    pod_exit(1);
  default:
    set_pod_mode(Emergency, "Recieved Signal %d", sig);
  }
#endif
}

void sigpipe_handler(__unused int sig) { error("SIGPIPE Recieved"); }

int main(int argc, char *argv[]) {
  printf("<<< Paradigm HyperLoop Pod Controller >>>\n\nCopyright " POD_COPY_YEAR
         " " POD_COPY_OWNER "\n\nCredits:\n" POD_CREDITS "\n");

  int boot_sem_ret = 0;

  parse_args(argc, argv);

  info("POD Booting...");
  info("Initializing Pod");

  if (init_pod() < 0) {
    fprintf(stderr, "Failed to Initialize Pod");
    pod_exit(1);
  }

  info("Loading Pod struct for the first time");
  pod_t *pod = get_pod();

  if (args.tests) {
    self_tests(pod);
  }

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

  // Disable IMU by starting with core -i -
  if (args.imu_device[0] != '-') {
    while (true) {
      info("Connecting to IMU at: %s", args.imu_device);
      pod->imu = imu_connect(args.imu_device);
      if (pod->imu < 0) {
        info("IMU connection failed: %s", args.imu_device);
        sleep(1);
      } else {
        break;
      }
    }
  } else {
    pod->imu = -1;
  }

#ifdef HAS_PRU
  pru_init();
#endif
  // -----------------------------------------
  // Logging - Remote Logging System
  // -----------------------------------------
  info("Starting the Logging Client Connection");
  pthread_create(&(pod->logging_thread), NULL, logging_main, NULL);

  // Wait for logging thread to connect to the logging server
  if (!args.ready) {
    boot_sem_ret = sem_wait(pod->boot_sem);
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
  pthread_create(&(pod->cmd_thread), NULL, command_main, NULL);

  // Wait for command thread to start it's server
  if (!args.ready) {
    boot_sem_ret = sem_wait(pod->boot_sem);
    if (boot_sem_ret != 0) {
      perror("sem_wait wait failed: ");
      pod_exit(1);
    }
  }

  // Assert pod is still boot
  if (get_pod_mode() != Boot) {
    error("Command thread has requested shutdown, See log for details");
    pod_exit(1);
  }

  info("Booting Core Controller Logic Thread");

  pthread_create(&(pod->core_thread), NULL, core_main, NULL);

  // we're using the built-in linux Round Roboin scheduling
  // priorities are 1-99, higher is more important
  // important note: this is not hard real-time
  set_pthread_priority(pod->core_thread, 70);
  set_pthread_priority(pod->logging_thread, 10);
  set_pthread_priority(pod->cmd_thread, 20);

  pthread_join(pod->core_thread, NULL);

  if (pod->imu > -1) {
    imu_disconnect(pod->imu);
  }
  pru_shutdown();

  // If the core thread joins, then there is a serious issue.  Fail immediately
  error("Core thread joined");
  exit(1);
  return 1;
}
