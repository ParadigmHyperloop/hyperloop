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

#include "core_checklists.h"

bool core_pod_checklist(pod_t *pod) {
  // TODO: is_battery_power_ok()  // Voltage > 28, current > 0.2 A @james
  // TODO: is_rpm_ok()            // less than 6000 @akeating
  // TODO: is_imu_ok()            // temp (-40°C to +75°C) VERIFIED
  // TODO: is_velocity_too_fast() // 95 m/s (roughly 215 mph) @akeating
  
  // TODO: is_reg_temp_ok()       // 0 -> 50 @akeating
  // TODO: is_clamp_temp_ok()     // 0 -> 100something @akeating
  // TODO: is_battery_temp_ok()   // 0 -> 60something @james
  // TODO: is_caliper_temp_ok()   // 0 -> 100something @akeating
  // TODO: is_frame_temp_ok()     // 0 -> 40 C @edhurtig
  
  // TODO: is_frame_pressure_ok() // 0 -> 20 PSIA VERIFIED
  // TODO: is_hp_pressure_ok()    // 0 -> 1770 PSI... @akeating
  // TODO: is_lp_pressure_ok()    // 0 -> 150 PSI... @akeating
  
  if (is_solenoid_open(&(pod->hp_fill_valve))) {
    return false;
  }
  
  return true;
}

/**
 * Is the pod safe. Used to inhibit transitions to various different states
 */
bool pod_safe_checklist(pod_t *pod) {
  return core_pod_checklist(pod) && is_pod_stopped(pod) && is_pod_vented(pod);
}

/**
 * Is the pod safe to proceed to an HP Fill
 */
bool pod_hp_safe_checklist(pod_t *pod) {
  return core_pod_checklist(pod) && is_pod_stopped(pod) && is_hp_vented(pod);
}
