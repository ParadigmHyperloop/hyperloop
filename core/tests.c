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

#include "pod.h"

int relay_walk(void);
int sensor_walker(void);
int self_tests(__unused pod_t *state);

#define N_WALKS 3

#define CONFIRM(cond) do { \
  if ((cond)) { \
    printf("[TEST] [PASS] " __XSTR__(cond) "\n"); \
  } else { \
    printf("[TEST] [FAIL] " __XSTR__(cond) "\n"); \
    exit(1); \
  } \
} while (0);
/**
 * Test to stress the pod's electrical system by bursting it under full load
 * and trying to generate worst case senario rush currents
 *
 * @return 0 on success. -1 on failure
 */
int relay_walk() {
  pod_t *pod = get_pod();
  solenoid_t *s;
  int i, prev = 0;
  for (i = 0; i < N_RELAY_CHANNELS; i++) {
    prev = (int)(((unsigned short)(i - 1)) % N_RELAY_CHANNELS);

    s = pod->relays[i];
    info("Open Solenoid on Relay %2.d", i);
    open_solenoid(s);

    // Ensure that the solenoid returns the proper states
    CONFIRM(is_solenoid_open(s));
    CONFIRM(!is_solenoid_closed(s));
    usleep(50000);
    CONFIRM(is_solenoid_open(s));
    CONFIRM(!is_solenoid_closed(s));

    s = pod->relays[prev];

    info("Close Solenoid on Relay %2.d", prev);


    close_solenoid(s);

    // Ensure that the solenoid returns the proper states
    assert(is_solenoid_closed(s));
    assert(!is_solenoid_open(s));
    usleep(50000);
    assert(is_solenoid_closed(s));
    assert(!is_solenoid_open(s));
  }

  s = pod->relays[prev + 1];
  info("[CLOS] Solenoid on Relay %2.d", prev + 1);
  close_solenoid(s);
  usleep(50000);

  return 0;
}

int sensor_walker() {
  return 0;
}

int self_tests(__unused pod_t *state) {
  info("Starting Self Tests");
  int i;
  for (i = 0; i < N_WALKS; i++) {
    if (relay_walk() < 0) {
      error("Relay Walk Test Failed");
      return 1;
    }
  }

  if (sensor_walker() < 0) {
    error("Relay Walk Test Failed");
    return 1;
  }
  info("Testing Complete!");
  return 0;
}
