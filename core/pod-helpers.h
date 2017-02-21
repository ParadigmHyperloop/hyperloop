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
