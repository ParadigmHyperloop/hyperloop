/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#ifndef PARADIGM_POD_HELPERS_H
#define PARADIGM_POD_HELPERS_H

/**
 * Helper Functions for checking pod state
 */
#include "pod.h"

/**
 * Attempts to enter the LP Fill state
 *
 * @return true if the state transition was successful, false if a condition
 *         failed
 */
bool start_lp_fill(void);

/**
 * Attempts to enter the HP Fill state
 *
 * @return true if the state transition was successful, false if a condition
 *         failed
 */
bool start_hp_fill(void);

/**
 * Determine if the Emergency Brakes are engaged based on their braking presure
 *
 * @param pod A pointer to the pod structure
 * @return true if any of the clamps are engaged, false if all are released
 */
bool any_clamp_brakes(pod_t *pod);

/**
 * Determine if the Primary Brakes are engaged based on their braking presure
 *
 * @param pod A pointer to the pod structure
 * @return true if any of the capipers are engaged, false if all are released
 */
bool any_calipers(pod_t *pod);

/**
 * Determine if the pod is stopped
 *
 * @param pod A pointer to the pod structure
 * @return true if the pod is deemed to be stopped, false if it is moving
 */
bool is_pod_stopped(pod_t *pod);

/**
 * Determine if the pod is vented
 *
 * @param pod A pointer to the pod structure
 * @return true if true if any of the air supply systems are still energized
 *         above 20 PSIG
 */
bool is_pod_vented(pod_t *pod);

/**
 * Determine if the hp air supply system is vented
 *
 * @param pod A pointer to the pod structure
 * @return true if true if the hp air supply system is still energized above 
 *         20 PSIG
 */
bool is_hp_vented(pod_t *pod);

/**
 * Determine if both of the lp air supply systems are vented
 *
 * @param pod A pointer to the pod structure
 * @return true if true if the lp air supply systems is still energized
 *         above 20 PSIG
 */
bool is_lp_vented(pod_t *pod);

/**
 * Determines the distance needed to stop given the current pod's velocity and
 * decceleration.
 *
 * @param pod A pointer to the pod structure
 * @return the estimated stopping distance of the pod at it's current 
 *         deccelleration
 */
// TODO: This function currently uses the globally deffined expected decelleration... not the current decelleration
float get_stopping_distance(pod_t *pod);

/**
 * Gets the remaining distance in the usable travel space in the tube
 *
 * @example If the TUBE_LENGTH is 1500 and the STOPPING_MARGIN is 300, this 
 *          function will return 1200 - the pod's X position.
 *
 * @param pod A pointer to the pod structure
 * @return the remaining distance before the "finish line" marker
 */
float get_remaining_distance(pod_t *pod);

/**
 * Computes the minimium decelleration required for the pod to stop at the 
 * end of the tube (minus the STOPPING_MARGIN)
 *
 * @param pod A pointer to the pod structure
 * @return The remaining distance in meters
 */
float get_stopping_deccel(pod_t *pod);

/**
 * Gets the sensor with the given name
 *
 * @param pod A pointer to the pod_t to search
 * @param name The name of the sensor to find
 *
 * @return A pointer to the sensor_t in the given pod_t or NULL if the sensor 
 *         is not found
 */
sensor_t *get_sensor_by_name(pod_t *pod, char *name);

/**
 * Gets the sensor at the input of the given mux
 * TODO: This needs to be changed for the new SPI ADCs
 *
 * @param pod A pointer to the pod_t to search
 * @param mux The mux number that the sensor resides on
 * @param input The input pin on the given mux that the sensor resides on
 *
 * @return A pointer to the sensor_t in the given pod_t or NULL if the sensor
 *         is not found
 */
sensor_t *get_sensor_by_address(pod_t *pod, int mux, int input);
#endif
