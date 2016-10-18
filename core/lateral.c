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
 * TODO: Do Not Need to do anything with this anymore
 */

uint32_t readLateralSensor(uint8_t sensorno) {
  // CHANGE ME! Read in value assume units in mm
  return 6;
}

int lateralRead(pod_state_t *state) {

  setPodField(&(state->lateral_front_left), readLateralSensor(0));
  setPodField(&(state->lateral_front_right), readLateralSensor(1));
  setPodField(&(state->lateral_rear_left), readLateralSensor(2));
  setPodField(&(state->lateral_rear_right), readLateralSensor(3));

  return 0;
}
