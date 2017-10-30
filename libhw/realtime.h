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


#ifndef PARADIGM_REALTIME_H
#define PARADIGM_REALTIME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define USEC_PER_SEC 1000000ull
#define NSEC_PER_SEC 1000000000ull
#define NSEC_PER_USEC 1000ull

/**
 * Get the current time of the pod in microseconds
 *
 * TODO: Make this function return nanosecond precision
 *
 * @return The current timestamp in microseconds
 */
uint64_t get_time_usec(void);

/**
 * @brief Loads the current RealTime timespec into the given address t.
 *
 * @note In the event of a failure to retrieve the current Realtime timespec,
 * an emergency condition is declared and the pod will safe itself.  A cached
 * timespec from the last successful realtime timespec will be copied into t
 */
void get_timespec(struct timespec *t);

/**
 * @brief Adds the given number of microseconds to the timespec pointed to by t
 */
void timespec_add_us(struct timespec *t, long us);

/**
 * @brief
 * Adds the given timespec t2 to t1
 *
 * @discussion
 * Equivalent to logical: (t1 = t1 - t2)
 *
 * @param t1 The timespec to subtrack t2 from
 * @param t2 The timespec to subtrack from t1
 */
void timespec_add(struct timespec *t1, const struct timespec *t2);

/**
 * @brief
 * Subtracts the given timespec t2 from t1
 *
 * @discussion
 * Equivalent to logical: (t1 = t1 - t2)
 *
 * @param t1 The timespec to subtrack t2 from
 * @param t2 The timespec to subtrack from t1
 */
void timespec_sub(struct timespec *t1, const struct timespec *t2);


/**
 * Compares two timespecs.
 *
 * @see strcmp
 */
int timespec_cmp(struct timespec *a, struct timespec *b);

/**
 * Returns the total number of nanoseconds in the timespec
 */
int64_t timespec_to_nsec(struct timespec *t);

#endif /* PARADIGM_REALTIME_H */
