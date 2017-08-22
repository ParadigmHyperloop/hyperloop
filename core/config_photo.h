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

#ifndef PARADIGM_CONFIG_PHOTO_H
#define PARADIGM_CONFIG_PHOTO_H

// --------------------------
// Vertical
// --------------------------

#define N_LEVITATION_DISTANCE 8
#define LEVITATION_DISTANCE_ADC DISTANCE_ADC
#define LEVITATION_DISTANCE_SENSORS                                                 \
  { 5, 10, 4, 11, 3, 12, 2, 13 }


// --------------------------
// Pusher
// --------------------------

#define N_PUSHER_DISTANCE 4
#define PUSHER_DISTANCE_ADC DISTANCE_ADC
#define PUSHER_DISTANCE_INPUTS                                                \
  { 9, 6, 8, 7 }

#define PUSHER_PRESENT_DISTANCE 0.05
#define PUSHER_TIMEOUT 1 * 1000 * 1000

#define DISTANCE_CALIBRATION_A 6.0434
#define DISTANCE_CALIBRATION_B -19.279
#define DISTANCE_CALIBRATION_C 18.546

#endif /* PARADIGM_CONFIG_PHOTO_H */
