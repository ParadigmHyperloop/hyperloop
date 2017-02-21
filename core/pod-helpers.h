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

sensor_t *get_sensor_by_name(pod_t *pod, char *name);
sensor_t *get_sensor_by_address(pod_t *pod, int mux, int input);
