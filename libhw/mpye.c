//
//  mpye.c
//  pod
//
//  Created by Eddie Hurtig on 4/15/17.
//
//

#include "mpye.h"

static int mpye_to_ssr_setpoint(mpye_value_t val) {
  return 2000 + val * 12;
}

void set_mpye(mpye_t *m, mpye_value_t val) {
  pthread_mutex_lock(&(m->mutex));
  m->value = val;
  set_ssr(m->bus->fd, m->address, m->channel, mpye_to_ssr_setpoint(val));
  pthread_mutex_unlock(&(m->mutex));
}

int mpye_init(mpye_t *m, char *name, bus_t *bus, unsigned char address, unsigned char channel) {
  strncpy(m->name, name, MAX_NAME);

  m->bus = bus;
  m->address = address;
  m->channel = channel;
  m->value = 0;
  m->locked = false;

  return 0;
}
