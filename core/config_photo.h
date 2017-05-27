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

#define N_CORNER_DISTANCE 4
#define CORNER_DISTANCE_MUX DISTANCE_MUX
#define CORNER_DISTANCE_INPUTS                                                 \
  { 0, 1, 2, 3 }

#define N_WHEEL_DISTANCE 3
#define WHEEL_DISTANCE_MUX DISTANCE_MUX
#define WHEEL_DISTANCE_INPUTS                                                  \
  { 4, 5, 6 }

// --------------------------
// Lateral
// --------------------------

#define N_LATERAL_DISTANCE 3
#define LATERAL_DISTANCE_MUX DISTANCE_MUX
#define LATERAL_DISTANCE_INPUTS                                                \
  { 7, 8, 9 }

// --------------------------
// Shell
// --------------------------

#define N_SHELL_PHOTO 4
#define SHELL_PHOTO_MUX PHOTO_MUX
#define SHELL_PHOTO_INPUTS                                                     \
  { 0, 1, 2, 3 }

// --------------------------
// Wheels
// --------------------------

#define N_WHEEL_PHOTO 3
#define WHEEL_PHOTO_MUX PHOTO_MUX
#define WHEEL_PHOTO_INPUTS                                                     \
  { 4, 5, 6 }

#endif /* PARADIGM_CONFIG_PHOTO_H */
