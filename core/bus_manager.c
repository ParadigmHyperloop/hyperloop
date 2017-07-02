//
//  bus_manager.c
//  pod
//
//  Created by Eddie Hurtig on 6/1/17.
//
//

#include "bus_manager.h"

#define TASK_THROTTLE 5000 /* microseconds */

void * bus_main(bus_t * bus) {
  bus_task_t * head = *bus->queue;
  
  uint64_t timestamp = get_time_usec();
  while (get_pod_mode() != Shutdown) {
    while (head && get_pod_mode() != Shutdown) {
      timestamp = get_time_usec();
      head->execute(bus);
      
      useconds_t elapsed = (useconds_t)(get_time_usec() - timestamp);
      
      if (elapsed < head->min_time) {
        usleep(head->min_time - elapsed);
      } else if (elapsed < TASK_THROTTLE) {
        usleep(TASK_THROTTLE - elapsed);
      }
      
      head = head->next;
    }
    error("Bus %s is empty", bus->name);
    sleep(1);
  }
  
  warn("Bus Manager %s is shutting down", bus->name);
  return NULL;
}

int bus_enqueue(bus_t * bus, bus_block_t block, bus_task_variant_t variant) {
  bus_task_t *task = NULL;
  
  // FIXME: This is a long lock
  pthread_mutex_lock(&bus->mutex);
  for (uint16_t i = 0; i < bus->heap_size && i < MAX_HEAP_SIZE; i++) {
    if (!((bus->heap_map << i) & 0x1)) {
      // Heap Entry is currently free
      bus->heap_map = (0x1 << i) | bus->heap_map;
      task = &bus->heap[i];
      break;
    }
  }
  
  if (task == NULL) {
    pthread_mutex_unlock(&bus->mutex);
    return -1;
  }
  
  memset(task, 0, sizeof(bus_task_t));

  task->execute = block;
  task->variant = variant;
  
  if (bus->queue == NULL) {
    // If the queue is empty then set this to the first and only element.
    bus->queue = &task;
    task->prev = task; // Infinite Loop
    task->next = NULL;
  } else {
    // Insert this task into the head of the queue.
    task->next = *bus->queue;
    (*bus->queue)->prev = task;
    bus->queue = &task;
  }

  pthread_mutex_unlock(&bus->mutex);

  return 0;
}


int bus_init(bus_t *bus, char *name, int (^open_block)(void)) {
  strncpy(bus->name, name, sizeof(bus->name));
  pthread_mutex_init(&bus->mutex, NULL);
  
  bus->fd = open_block();
  bus->queue = NULL;
  bus->heap_size = MAX_HEAP_SIZE;
  bus->heap_map = 0;
  bus->heap = malloc(bus->heap_size);
  
  if (bus->heap == NULL) {
    return -1;
  }
  return 0;
}

int bus_destroy(bus_t *bus) {
  if (bus->heap) {
    free(bus->heap);
  }
  return 0;
}
