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

#ifndef OPENLOOP_CORE_H
#define OPENLOOP_CORE_H
#include "pod.h"


void common_checks(pod_t *pod);
void boot_state_checks(__unused pod_t *pod);
bool core_pod_checklist(pod_t *pod);
bool pod_safe_checklist(pod_t *pod);
bool pod_hp_safe_checklist(pod_t *pod);

bool start_lp_fill(void);
bool start_hp_fill(void);

void post_state_checks(pod_t *pod);
void lp_fill_state_checks(pod_t *pod);
void hp_fill_state_checks(pod_t *pod);
void load_state_checks(pod_t *pod);
void standby_state_checks(pod_t *pod);
void armed_state_checks(pod_t *pod);

void emergency_state_checks(pod_t *pod);
void pushing_state_checks(pod_t *pod);
void coasting_state_checks(pod_t *pod);
void braking_state_checks(pod_t *pod);
void vent_state_checks(pod_t *pod);
void retrieval_state_checks(__unused pod_t *pod);
void skate_sensor_checks(pod_t *pod);
void lp_package_checks(pod_t *pod);
void lateral_sensor_checks(pod_t *pod);

void adjust_brakes(__unused pod_t *pod);
void adjust_skates(__unused pod_t *pod);
void adjust_hp_fill(pod_t *pod);
void adjust_vent(pod_t *pod);
void *core_main(__unused void *arg);

#endif
