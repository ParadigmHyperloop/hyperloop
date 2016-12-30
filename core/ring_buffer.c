#include "pod.h"

void ring_buf_init(ring_buf_t *buf, void *block, int size, int item_sz) {
  buf->head = buf->start = buf->tail = block;
  buf->end = block + size * item_sz;
  buf->sz = item_sz;
  buf->initialized = true;
  pthread_mutex_init(&(buf->mutex), NULL);
}

int ring_buf_append(log_t *l, ring_buf_t *buf) {
  pthread_mutex_lock(&(buf->mutex));

  // Copy the new element into the buffer
  memcpy(buf->head, l, buf->sz);

  buf->head += buf->sz;
  if (buf->head >= buf->end) {
    buf->head = buf->start;
  }

  if (buf->head == buf->tail) {
    buf->tail += buf->sz;
    if (buf->tail == buf->end) {
      buf->tail = buf->start;
    }
    pthread_mutex_unlock(&(buf->mutex));

#ifdef DEBUG
    fprintf(stderr, "[RINGBUF] Buf Full Dropping oldest message from buffer");
#endif

    // 1 indicates success but overwrote a queued log
    return 1;
  }
  buf->count++;

  pthread_mutex_unlock(&(buf->mutex));

  return 0;
}

int ring_buf_pop(log_t *l, ring_buf_t *buf) {
  pthread_mutex_lock(&(buf->mutex));

  if (buf->tail == buf->head) {
    pthread_mutex_unlock(&(buf->mutex));
    return -1;
  }
  // fprintf(stderr, "\n <<< Popping %p (%s)\n", buf->tail, ((log_t
  // *)buf->tail)->content.message);
  // Copy the new element into the buffer
  memcpy(l, buf->tail, buf->sz);

  buf->tail += buf->sz;
  if (buf->tail >= buf->end) {
    buf->tail = buf->start;
  }

  buf->count--;

  pthread_mutex_unlock(&(buf->mutex));
  return 0;
}
