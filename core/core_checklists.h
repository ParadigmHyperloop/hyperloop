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

#ifndef PARADIGM_CHECKLISTS_H
#define PARADIGM_CHECKLISTS_H

#include "pod.h"

/**
 * An aggregated checklist that should always be run on every control loop
 *
 * @param pod A pointer to the pod structure
 * @return true if the checklist passes, false if one or more of the checks
 * failed
 * TODO: A failure of a checklist item should be indicated by a bit flipped to
 *       1 in a global field (much like errno)
 */
bool core_pod_checklist(pod_t *pod);

/**
 * An aggregated checklist that should be run when the pod is expected to be
 * stopped (but could still be charged)
 *
 * @param pod A pointer to the pod structure
 * @return true if the checklist passes, false if one or more of the checks
 * failed
 */
bool pod_safe_checklist(pod_t *pod);

/**
 * An aggregated checklist that should be run when the pod is expected to be
 * completely deenergized.
 *
 * @param pod A pointer to the pod structure
 * @return true if the checklist passes, false if one or more of the checks
 * failed
 */
bool pod_hp_safe_checklist(pod_t *pod);

#endif /* PARADIGM_CHECKLISTS_H */
