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

#define N_WALKS 2
#define WAIT_USEC 50000

#define CONFIRM(cond)                                                          \
  do {                                                                         \
    if ((cond)) {                                                              \
      printf("[TEST] [PASS] " __XSTR__(cond) "\n");                            \
    } else {                                                                   \
      printf("[TEST] [FAIL] " __XSTR__(cond) "\n");                            \
      return 1;                                                                \
    }                                                                          \
  } while (0);

#define TEST_BUS_NAME "earth.paradigm.tests.bus.bus_a"
#define TEST_SEM_NAME "earth.paradigm.tests.bus.sem_a"
/**
 * Test to stress the pod's electrical system by bursting it under full load
 * and trying to generate worst case senario rush currents
 *
 * @return 0 on success. -1 on failure
 */
static int test_relays() {
  pod_t *pod = get_pod();
  solenoid_t *s;
  int i, prev = 0;
  for (i = 0; i < N_RELAY_CHANNELS; i++) {
    prev = (int)(((unsigned short)(i - 1)) % N_RELAY_CHANNELS);

    s = pod->relays[i];
    info("Open Solenoid on Relay %2.d", i);
    open_solenoid(s);

    // Ensure that the solenoid returns the proper states
    CONFIRM(is_solenoid_open(s));
    CONFIRM(!is_solenoid_closed(s));
    usleep(WAIT_USEC);
    CONFIRM(is_solenoid_open(s));
    CONFIRM(!is_solenoid_closed(s));

    s = pod->relays[prev];

    info("Close Solenoid on Relay %2.d", prev);

    close_solenoid(s);

    // Ensure that the solenoid returns the proper states
    CONFIRM(is_solenoid_closed(s));
    CONFIRM(!is_solenoid_open(s));
    usleep(WAIT_USEC);
    CONFIRM(is_solenoid_closed(s));
    CONFIRM(!is_solenoid_open(s));
  }

  s = pod->relays[prev + 1];
  info("Close Solenoid on Relay %2.d", prev + 1);
  close_solenoid(s);

  CONFIRM(is_solenoid_closed(s));
  CONFIRM(!is_solenoid_open(s));
  usleep(WAIT_USEC);
  CONFIRM(is_solenoid_closed(s));
  CONFIRM(!is_solenoid_open(s));

  return 0;
}

static int test_sensors() { return 0; }

static int test_bus_manager() {
  bus_t test_bus;
  sem_t *test_sem = sem_open(TEST_SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
  __block int test_fd = -1;
  bus_init(&test_bus, TEST_BUS_NAME, ^int {
    test_fd = open("/dev/random", O_RDONLY);
    return test_fd;
  });
  CONFIRM(test_bus.state == Init);
  CONFIRM(test_fd != -1);
  CONFIRM(test_bus.fd == test_fd);
  CONFIRM(strcmp(test_bus.name, TEST_BUS_NAME) == 0);

  __block int running = 0;
  bus_enqueue(&test_bus, ^(bus_t *bus) {
    debug("Executing test block");
    sleep(1);
    running = 1;

    if (strcmp(bus->name, TEST_BUS_NAME) != 0) {
      running = 2;
      return;
    }

    if (bus->fd != test_fd) {
      running = 3;
      return;
    }
    sem_post(test_sem);
  });

  CONFIRM(test_bus.state == Init);
  CONFIRM(running == 0);
  sleep(1);
  CONFIRM(running == 0);

  // Start the bus
  bus_run(&test_bus);
  CONFIRM(running == 0);
  sem_wait(test_sem);
  CONFIRM(running == 1);

  CONFIRM(test_bus.state == Run);

  running = 0;

  for (int i = 0; i < 100; i++) {
    bus_enqueue(&test_bus, ^(bus_t *bus) {
      debug("Executing block: %d", running);
      running++;

      if (strcmp(bus->name, TEST_BUS_NAME) != 0) {
        running = -100;
        return;
      }

      if (bus->fd != test_fd) {
        running = -1000;
        return;
      }
      sem_post(test_sem);
    });
  }

  while (running < 100) {
    sem_wait(test_sem);
    if (running < 0) {
      bus_destroy(&test_bus);
      return -1;
    }
  }

  CONFIRM(test_bus.head == NULL);
  int rc = bus_destroy(&test_bus);
  CONFIRM(rc == 0);

  return 0;
}

int self_tests(__unused pod_t *state) {
  info("Starting Self Tests");

  if (test_bus_manager() < 0) {
    error("Bus Manager Test Failed");
    return 1;
  }

  for (int i = 0; i < N_WALKS; i++) {
    if (test_relays() < 0) {
      error("Relay Walk Test Failed");
      return 1;
    }
  }

  if (test_sensors() < 0) {
    error("Relay Walk Test Failed");
    return 1;
  }

  info("Testing PASSED!");
  return 0;
}
