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

/**
 * Helper Functions for checking pod state
 */
#include "pod.h"

/**
 * Determine if the Emergency Brakes are engaged based on their braking presure
 */
bool emergencyBrakesEngaged(pod_state_t *state);

/**
 * Determine if the Primary Brakes are engaged based on their braking presure
 */
bool primaryBrakesEngaged(pod_state_t *state);

/**
 * Determine if the pod is stopped
 */
bool podIsStopped(pod_state_t *state);
