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

#define N_WALKS 10

/**
 * Test to stress the pod's electrical system by bursting it under full load
 * and trying to generate worst case senario rush currents
 *
 * @return 0 on success. -1 on failure
 */
int relay_walk() {
  pod_t *pod = get_pod();
  solenoid_t *s;
  int i;
  for (i=0;i<N_RELAY_CHANNELS;i++) {
    s = pod->relays[i];
    info("Opening Solenoid on relay %d", i);
    info(" > gpio: %d", s->gpio);
    info(" > value: %d", s->value);
    info(" > name: %s", s->name);
    if (is_solenoid_open(s)) {
      close_solenoid(s);
      usleep(500000);
    } else {
      open_solenoid(s);
      usleep(500000);
    }

    info("Closing Solenoid on relay %d", i);

    if (is_solenoid_closed(s)) {
      open_solenoid(s);
      usleep(100000);
    } else {
      close_solenoid(s);
      usleep(100000);
    }
  }

  return 0;
}

int sensor_walker() {
  // sensor_pack_t sensors = read_pru_dataset();
  return 0;
}

int self_tests(__unused pod_t *state) {
  info("Starting Self Tests");
  int i;
  for (i=0;i<N_WALKS;i++) {
    if (relay_walk() < 0) {
      error("Relay Walk Test Failed");
      exit(1);
    }
  }

  if (sensor_walker() < 0) {
    error("Relay Walk Test Failed");
    exit(1);
  }
  return 0;
}
