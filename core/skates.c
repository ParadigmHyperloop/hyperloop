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

uint32_t readDistanceSensor(int sensorno) {
  // TODO: Actually read in distance value
  return 1;
}

uint32_t readSkateTransducer(int thermocoupleno) {
  // TODO: Actually read in distance value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

int skateRead(pod_t *pod) {
  set_value(&(pod->skate_front_left_z), readDistanceSensor(0));
  set_value(&(pod->skate_front_right_z), readDistanceSensor(1));
  set_value(&(pod->skate_rear_left_z), readDistanceSensor(2));
  set_value(&(pod->skate_rear_right_z), readDistanceSensor(3));

  // TODO: Read in new values and put them into the pod struct
  int i;
  for (i = 0; i < N_SKATE_TRANSDUCERS; i++) {
    int32_t new_value =
        readSkateTransducer(i); // Read in value for relulator temp i
    set_value(&(pod->skate_transducers[i]), new_value);
  }

  return 0;
}
