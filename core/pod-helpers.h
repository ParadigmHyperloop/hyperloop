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
bool any_clamp_brakes(pod_t *pod);

/**
 * Determine if the Primary Brakes are engaged based on their braking presure
 */
bool any_calipers(pod_t *pod);

/**
 * Determine if the pod is stopped
 */
bool is_pod_stopped(pod_t *pod);

/**
 * Determine if the pod is vented
 */
bool is_pod_vented(pod_t *pod);

/**
 * Determine if the pod is vented
 */
bool is_hp_vented(pod_t *pod);

/**
 * Determine if the pod is vented
 */
bool is_lp_vented(pod_t *pod);

/**
 * Determines the distance needed to stop given the current pod's velocity
 * (uses 1 clamp brake decelleration defined as CLAMP_BRAKING_ACCEL)
 */
float get_stopping_distance(pod_t *pod);

float get_stopping_deccel(pod_t *pod);
/**
 * Sets the desired solenoid state
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void set_solenoid(solenoid_t *s, solenoid_state_t val);
/**
 * Opens the solenoid if it is not already set to be open
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void open_solenoid(solenoid_t *s);

/**
 * Closes the given solenoid if it is not already set to closed
 *
 * @note Handles the complexities of Normally open and Normally closed
 * solenoids for you
 */
void close_solenoid(solenoid_t *s);

/**
 * Inhibits the controller from changing the solenoid state.
 *
 * @note unlock_solenoid, the function to reverse this one, should only be
 * called by a human initiated action (such as a control point command)
 */
void lock_solenoid(solenoid_t *s);

/**
 * Removes an inhibit in place to prevent the given solenoid from changing
 * state
 *
 * @note should only be called by a human initiated action (such as a control
 * point command)
 */
void unlock_solenoid(solenoid_t *s);

/**
 * Determines if a solenoid is in it's open state
 */
bool is_solenoid_open(solenoid_t *s);

/**
 * Determines if a solenoid is in it's closed state
 */
bool is_solenoid_closed(solenoid_t *s);

/**
 * Determines if a solenoid is in a locked out state
 */
bool is_solenoid_locked(solenoid_t *s);


sensor_t * get_sensor_by_name(pod_t *pod, char *name);
sensor_t * get_sensor_by_address(pod_t *pod, int mux, int input);

/**
 * Setup the given pin as a standard output pin
 */
bool setup_pin(int no);
