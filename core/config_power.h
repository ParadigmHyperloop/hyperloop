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

#ifndef PARADIGM_CONFIG_POWER_H
#define PARADIGM_CONFIG_POWER_H

// --------------------------
// Power Board
// --------------------------

#define KILL_PIN 86

#define KILL_PIN_KILL_VALUE 0
#define KILL_PIN_OK_VALUE 1

#define PB_PIN 87
#define PB_ON_VAL 1
#define PB_OFF_VAL 0

#define N_POWER_THERMO 4
#define POWER_THERMO_MUX THERMO_MUX_1
#define POWER_THERMO_INPUTS                                                    \
  { 6, 7, 8, 9 }

// --------------------------
// Batteries
// --------------------------

#define N_BATTERIES 3
#define BATTERY_I2C_ADDRESSES                                                  \
  { 0x0b, 0x0b, 0x0b }

// --------------------------
// Frame Thermocouple
// --------------------------
#define N_FRAME_THERMO 1
#define FRAME_THERMO_MUX THERMO_MUX_1
#define FRAME_THERMO_INPUTS                                                    \
  { 10 }

#endif /* PARADIGM_CONFIG_POWER_H */
