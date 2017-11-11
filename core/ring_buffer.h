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

#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#ifndef PARADIGM_RING_BUFFER_H
#define PARADIGM_RING_BUFFER_H

/**
 * A basic statically sized ring buffer for fast, multithreaded access to data
 *
 * This implementation focuses on making a ring buffer to be used as a
 * fixed size self evicting in-memory FIFO queue. Elements added using the
 * ring_buf_* utility functions will assume this is your use case.
 */
typedef struct ring_buf {
  void *start;           // start pointer
  void *end;             // end pointer
  void *head;            // pointer to head of data
  void *tail;            // pointer to tail of data
  int sz;                // Size of each element
  int count;             // Number of elements
  bool initialized;      // If initialized
  pthread_mutex_t mutex; // mutex
} ring_buf_t;

/**
 * Initialize a ring_buf_t
 *
 * Callers shall provide a pointer to a ring_buf_t, a block of memory to store
 * buffered data into, the maximium number of elements to be stored in the
 * given block, and the size of each item that will be stored in the buffer
 *
 * @note each item must be of fixed size
 * @note The size of the block must be greater than or equal to size * item_sz
 *       bytes
 *
 * @param buf A pointer to the ring buffer
 * @param block A pointer to the block of memory for data storage
 * @param size Max number of elements the buffer can store
 * @param item_sz Size of each element to be stored
 */
void ring_buf_init(ring_buf_t *buf, void *block, int size, int item_sz);

/**
 * Append the given element to the ring buffer
 *
 * Callers shall provide a pointer to an element to append and a pointer to
 * the ring buffer. This function will overwrite the oldest item in the buffer
 * if the buffer is Full
 *
 * @param l A pointer to the element to append
 * @param size The size of the element in bytes
 * @param buf A pointer to the ring buffer
 *
 * @return 0 if the element was appended, 1 if the element overwrote an
 *         existing item, -1 on error
 */
int ring_buf_append(const void *l, int size, ring_buf_t *buf);

/**
 * Pop the oldest item out of the ring buffer.
 *
 * Callers shall provide a pointer to a buf->sz sized block of memory to
 * copy the element into.  Should the buffer be empty, this block of memory
 * will go untouched and -1 will be returned.
 *
 * @param l A pointer to where to place the popped element
 * @param size The size you have allocated to l
 * @param buf A pointer to the ring buffer
 *
 * @return -1 if the buffer is empty, otherwise 0
 */
int ring_buf_pop(void *l, int size, ring_buf_t *buf);

#endif
