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

#ifndef PARADIGM_DEBUG_H
#define PARADIGM_DEBUG_H

// --------------
// Debug Printing
// --------------
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#ifndef TAG
#define TAG ""
#endif

#ifdef POD_DEBUG
#define FLINE __FILE__ ":" __XSTR__(__LINE__)
#define output(prefix_, fmt_, ...)                                             \
  pod_log((prefix_ TAG "[%s] {" FLINE "} " fmt_ "\n"), __FUNCTION__,           \
          ##__VA_ARGS__)
#else
#define output(prefix_, fmt_, ...)                                             \
  pod_log((prefix_ TAG fmt_ "\n"), ##__VA_ARGS__)
#endif

#ifdef POD_DEBUG
#define debug(fmt_, ...) output("[DEBG] ", fmt_, ##__VA_ARGS__)
#else
#define debug(fmt_, ...)
#endif

#define warn(fmt_, ...) output("[WARN] ", fmt_, ##__VA_ARGS__)
#define error(fmt_, ...) output("[ERRR] ", fmt_, ##__VA_ARGS__)
#define info(fmt_, ...) output("[INFO] ", fmt_, ##__VA_ARGS__)
#define note(fmt_, ...) output("[NOTE] ", fmt_, ##__VA_ARGS__)
#define fatal(fmt_, ...) output("[FATL] ", fmt_, ##__VA_ARGS__)

#define panic(subsystem, notes, ...)                                           \
  pod_panic(subsystem, __FILE__, __LINE__, notes, ##__VA_ARGS__)

#define DECLARE_EMERGENCY(message, ...)                                        \
  set_pod_mode(Emergency, __FILE__ ":" __XSTR__(LINE__) message, ##__VA_ARGS__)

#pragma clang diagnostic pop

#endif
