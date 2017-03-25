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

#ifndef OPENLOOP_OVERRIDES_H
#define OPENLOOP_OVERRIDES_H

#define PRIMARY_BRAKE_OVERRIDE_FRONT 0x0000000000000008
#define PRIMARY_BRAKE_OVERRIDE_MID 0x0000000000000010
#define PRIMARY_BRAKE_OVERRIDE_REAR 0x0000000000000020
#define PRIMARY_BRAKE_OVERRIDE_ALL                                             \
  (PRIMARY_BRAKE_OVERRIDE_FRONT) | (PRIMARY_BRAKE_OVERRIDE_MID) |              \
      (PRIMARY_BRAKE_OVERRIDE_REAR)

#define CLAMP_OVERRIDE_FRONT 0x0000000000000001
#define CLAMP_OVERRIDE_REAR 0x0000000000000002
#define CLAMP_OVERRIDE_ALL (CLAMP_OVERRIDE_FRONT) | (CLAMP_OVERRIDE_REAR)

#define CLAMP_OVERRIDE_LIST                                                    \
  { (CLAMP_OVERRIDE_FRONT), (CLAMP_OVERRIDE_REAR) }

#define SKATE_OVERRIDE_FRONT_LEFT 0x0000000000000040
#define SKATE_OVERRIDE_FRONT_RIGHT 0x0000000000000080
#define SKATE_OVERRIDE_MID_LEFT 0x0000000000000100
#define SKATE_OVERRIDE_MID_RIGHT 0x0000000000000200
#define SKATE_OVERRIDE_REAR_LEFT 0x0000000000000400
#define SKATE_OVERRIDE_REAR_RIGHT 0x0000000000000800

#define SKATE_OVERRIDE_ALL                                                     \
  (SKATE_OVERRIDE_FRONT_LEFT) | (SKATE_OVERRIDE_FRONT_RIGHT) |                 \
      (SKATE_OVERRIDE_MID_LEFT) | (SKATE_OVERRIDE_MID_RIGHT) |                 \
      (SKATE_OVERRIDE_REAR_LEFT) | (SKATE_OVERRIDE_REAR_RIGHT)

#define SKATE_OVERRIDE_LIST                                                    \
  {                                                                            \
    (SKATE_OVERRIDE_FRONT_LEFT), (SKATE_OVERRIDE_FRONT_RIGHT),                 \
        (SKATE_OVERRIDE_MID_LEFT), (SKATE_OVERRIDE_MID_RIGHT),                 \
        (SKATE_OVERRIDE_REAR_LEFT), (SKATE_OVERRIDE_REAR_RIGHT)                \
  }

#endif
