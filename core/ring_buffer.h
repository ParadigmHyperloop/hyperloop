#ifndef OPENLOOP_RING_BUFFER_H
#define OPENLOOP_RING_BUFFER_H
#include "pod.h"

typedef struct ring_buf {
  void *start;
  void *end;
  void *head;
  void *tail;
  int sz;
  int count;
  bool initialized;
  pthread_mutex_t mutex;
} ring_buf_t;

#endif
