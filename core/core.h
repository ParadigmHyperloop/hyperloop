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

#ifndef PARADIGM_CORE_H
#define PARADIGM_CORE_H
#include "pod.h"
#include "core_checklists.h"

/**
 * A list of common tasks that should be run on every control loop cycle
 *
 * @param pod A pointer to the pod structure
 */
void common_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Boot
 *
 * @param pod A pointer to the pod structure
 */
void boot_state_checks(__unused pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Post
 *
 * @param pod A pointer to the pod structure
 */
void post_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is LP Fill
 *
 * @param pod A pointer to the pod structure
 */
void lp_fill_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is HP Fill
 *
 * @param pod A pointer to the pod structure
 */
void hp_fill_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Load
 *
 * @param pod A pointer to the pod structure
 */
void load_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Standby
 *
 * @param pod A pointer to the pod structure
 */
void standby_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Armed
 *
 * @param pod A pointer to the pod structure
 */
void armed_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Emergency
 *
 * @param pod A pointer to the pod structure
 */
void emergency_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Pushing
 *
 * @param pod A pointer to the pod structure
 */
void pushing_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Coasting
 *
 * @param pod A pointer to the pod structure
 */
void coasting_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Braking
 *
 * @param pod A pointer to the pod structure
 */
void braking_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Vent
 *
 * @param pod A pointer to the pod structure
 */
void vent_state_checks(pod_t *pod);

/**
 * A list of tasks that should be performed when the pod's state is Retrieval
 *
 * @param pod A pointer to the pod structure
 */
void retrieval_state_checks(__unused pod_t *pod);

/**
 * A generic list of checks to perform with skate sensors
 *
 * @param pod A pointer to the pod structure
 */
void skate_sensor_checks(pod_t *pod);


/**
 * A generic list of checks to perform relating to the lp packages
 *
 * @param pod A pointer to the pod structure
 */
void lp_package_checks(pod_t *pod);


/**
 * A generic list of checks to perform relating to the lateral distance sensors
 *
 * @param pod A pointer to the pod structure
 */
void lateral_sensor_checks(pod_t *pod);

/**
 * Adjust the setpoints for the brakes
 *
 * @param pod A pointer to the pod structure
 */
void adjust_brakes(__unused pod_t *pod);

/**
 * Adjust the setpoints for the skates
 *
 * @param pod A pointer to the pod structure
 */
void adjust_skates(__unused pod_t *pod);

/**
 * Adjust the setpoints for the hp fill valve
 *
 * @param pod A pointer to the pod structure
 */
void adjust_hp_fill(pod_t *pod);

/**
 * Adjust the setpoints for the vent valve
 *
 * @param pod A pointer to the pod structure
 */
void adjust_vent(pod_t *pod);

/**
 * Main control loop function
 *
 * @param arg Unused pthread arg
 */
void *core_main(__unused void *arg);

#endif /* PARADIGM_CORE_H */
