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

#include "ring_buffer.h"

void ring_buf_init(ring_buf_t *buf, void *block, int size, int item_sz) {
  buf->head = buf->start = buf->tail = block;
  buf->end = (unsigned char *)block + size * item_sz;
  buf->sz = item_sz;
  buf->initialized = true;
  pthread_mutex_init(&(buf->mutex), NULL);
}

int ring_buf_append(const void *l, int size, ring_buf_t *buf) {
  pthread_mutex_lock(&(buf->mutex));

  // Copy the new element into the buffer

  if (size > buf->sz) {
    size = buf->sz;
  }

  memcpy(buf->head, l, size);

  buf->head = (unsigned char *)buf->head + buf->sz;
  if (buf->head >= buf->end) {
    buf->head = buf->start;
  }

  if (buf->head == buf->tail) {
    buf->tail = (unsigned char *)buf->tail + buf->sz;
    if (buf->tail == buf->end) {
      buf->tail = buf->start;
    }
    pthread_mutex_unlock(&(buf->mutex));

    // 1 indicates success but overwrote a queued log
    return 1;
  }
  buf->count++;

  pthread_mutex_unlock(&(buf->mutex));

  return 0;
}

int ring_buf_pop(void *l, int size, ring_buf_t *buf) {
  pthread_mutex_lock(&(buf->mutex));

  if (buf->tail == buf->head) {
    pthread_mutex_unlock(&(buf->mutex));
    return -1;
  }
  if (buf->sz < size) {
    size = buf->sz;
  }
  // Copy the element into the given location `l`
  memcpy(l, buf->tail, size);

  buf->tail = (unsigned char *)buf->tail + buf->sz;
  if (buf->tail >= buf->end) {
    buf->tail = buf->start;
  }

  buf->count--;

  pthread_mutex_unlock(&(buf->mutex));
  return 0;
}
