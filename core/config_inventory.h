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

#ifndef PARADIGM_CONFIG_INVENTORY_H
#define PARADIGM_CONFIG_INVENTORY_H

#include "config_airsupply.h"
#include "config_overrides.h"
#include "config_photo.h"
#include "config_power.h"

/// MISC Sensors and other constants.
#define TUBE_LENGTH 1600
#define STOP_MARGIN 100
#define START_BRAKING 1100

// --------------------------
// Timers
// --------------------------

// 113.5 MPH
#define DEFAULT_WATCHDOG_TIMER                                                 \
  32000000 // 28.2 seconds // Main braking timeout initiated by pushing state //
           // 5MPH 4300000
#define DEFAULT_EMERGENCY_HOLD                                                 \
  30 * USEC_PER_SEC // 30 seconds held in the emergency state
#define DEFAULT_BRAKING_WAIT                                                   \
  2 * USEC_PER_SEC // Time before engaging secondary brake, if needed
#define DEFAULT_BRAKING_HOLD                                                   \
  30 * USEC_PER_SEC // min time to hold brakes before vent
#define DEFAULT_PUSHER_TIMEOUT                                                 \
  1 * USEC_PER_SEC // Timeout for the pusher plate debounce
#define DEFAULT_PUSHER_STATE_ACCEL_MIN                                         \
  0.25f // m/s/s // Threshold for transitioning into the pushing state
#define DEFAULT_PUSHER_STATE_MIN_TIMER                                         \
  24000000 // 20.2 seconds // Minimium time in the pushing state
#define DEFAULT_PUSHER_DISTANCE_MIN                                            \
  70 // mm // Distance to register the pusher as present
#define DEFAULT_PRIMARY_BRAKING_ACCEL_MIN                                      \
  -6.25 // m/s/s // "minimum" acceptable acceleration while braking

// 50 MPH
//#define WATCHDOG_TIMER 20200000                   // 20.2 seconds // Main
//braking timeout initiated by pushing state // 5MPH 4300000
//#define EMERGENCY_HOLD 30 * USEC_PER_SEC          // 30 seconds held in the
//emergency state
//#define BRAKING_WAIT 2 * USEC_PER_SEC             // Time before engaging
//secondary brake, if needed
//#define BRAKING_TIMEOUT 30 * USEC_PER_SEC         // min time to hold brakes
//before vent
//#define PUSHER_TIMEOUT 1 * USEC_PER_SEC           // Timeout for the pusher
//plate debounce
//#define PUSHING_STATE_ACCEL_X 0.2f // m/s/s       // Threshold for
//transitioning into the pushing state
//#define PUSHING_STATE_MIN_TIMER 15200000          // 15.2 seconds // Minimium
//time in the pushing state

// 15 MPH
//#define WATCHDOG_TIMER 8400000                    // 8.4 seconds // Main
//braking timeout initiated by pushing state // 5MPH 4300000
//#define EMERGENCY_HOLD 30 * USEC_PER_SEC          // 30 seconds held in the
//emergency state
//#define BRAKING_WAIT 2 * USEC_PER_SEC             // Time before engaging
//secondary brake, if needed
//#define BRAKING_TIMEOUT 20 * USEC_PER_SEC         // min time to hold brakes
//before vent
//#define PUSHER_TIMEOUT 1 * USEC_PER_SEC           // Timeout for the pusher
//plate debounce
//#define PUSHING_STATE_ACCEL_X 1.0 // m/s/s        // Threshold for
//transitioning into the pushing state
//#define PUSHING_STATE_MIN_TIMER 3400000           // 3.4 seconds // Minimium
//time in the pushing state

#define PRIMARY_BRAKING_ACCEL_X_NOM -7.84 // -0.8 G => mm/s/s
#define PRIMARY_BRAKING_ACCEL_X_MAX -24.5 // -2.5 G => mm/s/s

#define RETURN_TO_STANDBY false
// --------------------------
// Relay
// --------------------------
#define N_RELAY_CHANNELS 10

// --------------------------
// MUX
// --------------------------
#define N_ADCS 8
#define N_ADC_CHANNELS 16
#define THERMO_MUX_0 0
#define THERMO_MUX_1 1
#define PRESSURE_ADC 6
#define DISTANCE_ADC 7
#define PHOTO_MUX 4
#define SPARE_MUX_0 5
#define SPARE_MUX_1 6

#endif /* PARADIGM_CONFIG_INVENTORY_H */
