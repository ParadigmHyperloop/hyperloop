//
//  mpye.c
//  pod
//
//  Created by Eddie Hurtig on 4/15/17.
//
//

#include "mpye.h"

static void enqueue_mpye_update(__unused mpye_t *m) {
  //TODO: Implement
}

void set_mpye(mpye_t *m, mpye_value_t val) {
  pthread_mutex_lock(&(m->mutex));
  
  m->value = val;

  if (!m->queued) {
    enqueue_mpye_update(m);
  }

  pthread_mutex_unlock(&(m->mutex));
}
