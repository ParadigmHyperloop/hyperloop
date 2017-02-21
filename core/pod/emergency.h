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

#ifndef OPENLOOP_POD_EMERGENCY_H
#define OPENLOOP_POD_EMERGENCY_H
#include "../pod.h"
#include "states.h"

/**
 * WARNING! TAKE EXTREME CARE IN WHERE YOU USE THIS! THIS WILL KILL THE PROG!
 *
 * Recommend instead:
 *
 *  set_pod_mode(Emergency, "Explain what happened");
 *
 * or the wrapper macro for set_pod_mode(Emergency) which adds file/line info
 *
 *  DECLARE_EMERGENCY("Explain what happened");
 *
 * Declare an immediate panic and exit. This will kill the controller, Logging
 * server, command server, and the entire process.
 *
 * The panic flow is as follows
 *   - Print the given panic notes to stderr ONLY
 *   - Issue a POD_SIGPANIC to own pid or just exit(POD_EX_PANIC)
 *   - The safety wrapper script should immediately attempt to set the CTRL_OK
 *     GPIO to LOW using the linux /sys/class/gpio tree signalling the
 *     Emergency Board that the controller crashed
 *   - The safety wrapper script will attempt to deenergize the Ebrake
 *     solenoids if possible but the Emergency Board should also open the
 *     Emergency brake solenoids as well when CTRL_OK drops LOW
 */
void pod_panic(int subsystem, char *file, int line, char *notes, ...);

/**
 * Set the given caution bit in the pod indicating a cautionary Alert
 */
void set_caution(pod_warning_t caution, pod_t *pod);

/**
 * Set the given warning bit in the pod indicating a warning Alert
 */
void set_warn(pod_warning_t warning, pod_t *pod);

/**
 * Clear the given caution bit in the pod cancelling a cautionary Alert
 */
void clear_caution(pod_warning_t caution, pod_t *pod);

/**
 * Clear the given warning bit in the pod cancelling a warning Alert
 */
void clear_warn(pod_warning_t warning, pod_t *pod);

/**
 * Check if the given caution bit is set in the pod
 */
bool has_caution(pod_caution_t caution, pod_t *pod);

/**
 * Check if the given warning bit is set in the pod
 */
bool has_warning(pod_warning_t warning, pod_t *pod);

#endif
