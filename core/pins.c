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

void setup_pins(pod_t *pod) {

  int i;
  for (i = 0; i < N_SKATE_SOLONOIDS; i++) {
    initPin(pod->skate_solonoids[i].gpio);
    setPinValue(pod->skate_solonoids[i].gpio, ON);
  }

  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    initPin(pod->wheel_solonoids[i].gpio);
    setPinValue(pod->wheel_solonoids[i].gpio, ON);
  }

  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    initPin(pod->ebrake_solonoids[i].gpio);
    setPinValue(pod->ebrake_solonoids[i].gpio, ON);
  }
}
