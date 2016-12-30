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
#include "libBBB.h"
#include "pod.h"

void setup_pins(pod_state_t * state) {


  int i;
  for (i=0; i<N_SKATE_SOLONOIDS; i++) {
    initPin(state->skate_solonoids[i].gpio);
    setPinValue(state->skate_solonoids[i].gpio, ON);
  }

  for (i=0; i<N_WHEEL_SOLONOIDS; i++) {
    initPin(state->wheel_solonoids[i].gpio);
    setPinValue(state->wheel_solonoids[i].gpio, ON);
  }

  for (i=0; i<N_EBRAKE_SOLONOIDS; i++) {
    initPin(state->ebrake_solonoids[i].gpio);
    setPinValue(state->ebrake_solonoids[i].gpio, ON);
  }
}
