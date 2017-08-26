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
#define WATCHDOG_TIMER 10 * USEC_PER_SEC  // 5MPH 4300000
#define EMERGENCY_HOLD 30 * USEC_PER_SEC
#define BRAKING_WAIT 1 * USEC_PER_SEC      // Time before engaging secondary brake, if needed
#define BRAKING_TIMEOUT 20 * USEC_PER_SEC  // min time to hold brakes before vent
#define PUSHER_TIMEOUT 2 * USEC_PER_SEC
#define RETURN_TO_STANDBY false
#define PUSHING_STATE_ACCEL_X 1 // m/s/s

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
