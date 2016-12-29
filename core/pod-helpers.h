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
bool any_emergency_brakes(pod_t *pod);

/**
 * Determine if the Primary Brakes are engaged based on their braking presure
 */
bool any_calipers(pod_t *pod);

/**
 * Determine if the pod is stopped
 */
bool is_pod_stopped(pod_t *pod);

/**
 * Sets the desired solenoid state
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void set_solenoid(pod_solenoid_t *s, solenoid_state_t val);
/**
 * Opens the solenoid if it is not already set to be open
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void open_solenoid(pod_solenoid_t *s);

/**
 * Closes the given solenoid if it is not already set to closed
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void close_solenoid(pod_solenoid_t *s);
