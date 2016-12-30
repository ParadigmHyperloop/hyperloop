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

/**
 * Walks all the surfaces to the given level (0 for min, 1 for max).
 * For example: relays will be closed if lvl is 1 and released/opened if set
 * to 0.  MPYEs will be closed when lvl is 1 and opened if 0.
 *
 * lvl = 1 indicates the state of the surface that will draw the most currents
 *
 * @param lvl The state to set surfaces to
 * @param sleep The sleep time between each surface actuation.
 * @param sleep_block The sleep time between actuating all surfaces of a given
 *   type.
 *
 * @return 0 on success. -1 on failure
 */
int set_all_surfaces(int lvl, int sleep, int sleep_block) {
  int i;

  solenoid_state_t s = kSolenoidOpen;
  if (lvl == 0) {
    s = kSolenoidClosed;
  }

  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    set_skate_target(i, s, false);
    usleep(sleep);
  }
  usleep(sleep_block);

  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    set_emergency_brakes(i, s, false);
    usleep(sleep);
  }
  usleep(sleep_block);

  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    set_caliper_brakes(i, s, false);
    usleep(sleep);
  }
  usleep(sleep_block);

  return 0;
}

/**
 * Test to stress the pod's electrical system by bursting it under full load
 * and trying to generate worst case senario rush currents
 *
 * @param state A pointer
 *
 * @return 0 on success. -1 on failure
 */
int relay_walker() {
  int n = 0;
  int lvl = 0;
  int half_second = 5000000;
  while (n <= 10) {
    lvl = n % 2;
    set_all_surfaces(lvl, half_second, 0);
    n++;
  }

  n = 0;
  while (n <= 10) {
    lvl = n % 2;
    set_all_surfaces(lvl, 0, half_second);
    n++;
  }
  sleep(2);

  n = 0;
  while (n <= 10) {
    lvl = n % 2;
    set_all_surfaces(lvl, 0, 0);
    usleep(half_second);
    n++;
  }

  return 0;
}

int sensor_walker() {
  // sensor_pack_t sensors = read_pru_dataset();
  return 0;
}

int self_tests(pod_t *state) {
  if (relay_walker() < 0) {
    error("Relay Walk Test Failed");
    exit(1);
  }

  if (sensor_walker() < 0) {
    error("Relay Walk Test Failed");
    exit(1);
  }
  return 0;
}
