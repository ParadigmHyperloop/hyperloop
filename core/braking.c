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

uint32_t readWheelPressure(int thermocoupleno) {
  // TODO: Actually read in thermocouple value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

uint32_t readWheelThermocouple(int thermocoupleno) {
  // TODO: Actually read in thermocouple value
  // TODO: Probably want to abstract this down to a generic readThermocouple()
  return 15;
}

uint32_t readEBrakePressure(int sensorno) {
  // TODO: Actually read in presure value
  // TODO: Probably want to abstract this down to a generic readPressure()
  return (get_pod()->tmp_brakes == 1 ? 50 : 0); // For simulator
}

uint32_t readEBrakeThermocouple(int thermocoupleno) {
  // TODO: Actually read in presure value
  // TODO: Probably want to abstract this down to a generic readPressure()
  return (get_pod()->tmp_ebrakes == 1 ? 50 : 0); // For simulator
}

// Read the presure sensors for the brakes
int readBraking(pod_t *pod) {
  debug("[readBraking] Reading presure sensors for brakes");

  // TODO: Read in new values and put them into the pod struct
  int i;

  for (i = 0; i < N_WHEEL_SOLONOIDS; i++) {
    int32_t new_value =
        readWheelThermocouple(i); // Read in value for wheel thermal sensor i
    set_value(&(pod->wheel_thermocouples[i]), new_value);
  }

  for (i = 0; i < N_EBRAKE_SOLONOIDS; i++) {
    int32_t new_value =
        readEBrakeThermocouple(i); // Read in value for ebrake thermal sensor i
    set_value(&(pod->ebrake_thermocouples[i]), new_value);
  }

  return 0;
}
