//
//  mpye.c
//  pod
//
//  Created by Eddie Hurtig on 4/15/17.
//
//

#include "mpye.h"

static void enqueue_mpye_update(__unused mpye_t *m) {
//  bus_enqueue(m->bus, ^(bus_t *bus) {
//    unsigned char data[] = { m->address, m->channel, (unsigned char)m->value};
//    
//    write(bus->fd, data, sizeof(data)/sizeof(unsigned char));
//    m->queued = false;
//  });
}

void set_mpye(mpye_t *m, mpye_value_t val) {
  pthread_mutex_lock(&(m->mutex));
  
  m->value = val;

  if (!m->queued) {
    enqueue_mpye_update(m);
  }

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
