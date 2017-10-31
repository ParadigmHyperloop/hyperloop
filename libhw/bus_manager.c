//
//  bus_manager.c
//  pod
//
//  Created by Eddie Hurtig on 6/1/17.
//
//

#include "bus_internal.h"

#define TASK_THROTTLE 5000 /* microseconds */

static void task_run(bus_task_t *task, bus_t *bus) {
  assert(task);
  assert(task->execute);
  assert(bus);
  task->execute(bus);
}

static bus_task_t *task_create(bus_block_t execution) {
  bus_task_t *task = malloc(sizeof(bus_task_t));
  if (!task) {
    return NULL;
  }

  task->execute = execution;
  assert(Block_copy(task->execute));

  return task;
}

static void task_destroy(bus_task_t *task) {
  assert(task);
  Block_release(task->execute);
  free(task);
}

static bus_task_t * bus_dequeue(bus_t *bus) {
  assert(bus);

  pthread_mutex_lock(&bus->mutex);

  bus_task_t * task = bus->head;
  if (task) {
    bus->head = task->next;
  }

  pthread_mutex_unlock(&bus->mutex);


  return task;
}

static void *bus_main(bus_t *bus) {

  bus->state = Run;

  uint64_t timestamp = get_time_usec();
  while (bus->state != Killed) {
    int rc = sem_wait(bus->sem);
    assert(rc == 0);
    bus_task_t *task = bus_dequeue(bus);
    if (task && bus->state != Killed) {
      printf("Executing Task %p", (void *)bus->head);
      timestamp = get_time_usec();
      if (task->mode != Cancelled) {
        task_run(task, bus);

        useconds_t elapsed = (useconds_t)(get_time_usec() - timestamp);

        if (elapsed < bus->throttle) {
          usleep(bus->throttle - elapsed);
        }
      }

      pthread_mutex_lock(&bus->mutex);

      if (task->mode == Requeue) {
        // Put the task back onto the tail of the queue
        bus->tail->next = task;
        task->prev = bus->tail;
        bus->tail = task;
      } else {
        task_destroy(task);
      }
      pthread_mutex_unlock(&bus->mutex);

    } else {
      printf("Bus %s is empty or bus is being killed", bus->name);
    }
  }

  printf("Bus Manager %s is shutting down", bus->name);
  return NULL;
}

void bus_run(bus_t *bus) {
  pthread_create(&(bus->thread), NULL, (void *(*)(void *))bus_main, bus);
}

int bus_kill(bus_t *bus) {
  pthread_mutex_lock(&bus->mutex);

  bus->state = Killed;

  pthread_mutex_unlock(&bus->mutex);

  // Note: This sem_post() does NOT belong in the critical section
  int rc = sem_post(bus->sem);
  assert(rc == 0);

  return (rc != 0 ? -1 : 0);
}

bus_task_t * bus_enqueue(bus_t *bus, bus_block_t block) {
  bus_task_t *task = task_create(block);
  if (!task) {
    printf("Task failed to schedule on bus %p", bus);
    return NULL;
  }

  pthread_mutex_lock(&bus->mutex);

  memset(task, 0, sizeof(bus_task_t));

  task->execute = block;

  if (bus->head == NULL) {
    // If the queue is empty then set this to the first and only element.
    bus->head = task;
    task->prev = NULL;
    task->next = NULL;
    bus->tail = task;
  } else {
    // Insert this task into the end of the queue.
    bus->tail->next = task;
    task->prev = bus->tail;
    bus->tail = task;
  }
  pthread_mutex_unlock(&bus->mutex);

  assert(Block_copy(task->execute));

  int rc = sem_post(bus->sem);
  assert(rc == 0);

  return task;
}

int bus_init(bus_t *bus, const char *name, int (^open_block)(void)) {
  bus->name = name;

  bus->fd = open_block();

  if (bus->fd < 0) {
    perror("Bus Failed to open: ");
    return -1;
  }

  bus->head = NULL;
  bus->throttle = TASK_THROTTLE;
  bus->state = Init;

  pthread_mutex_init(&bus->mutex, NULL);
  bus->sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
  assert(bus->sem != SEM_FAILED);

  for (int i = 0; i < BUS_BUFFER_SIZE; i++) {
    bus->mem[i] = 0;
  }

  printf("Initialized bus: %s", bus->name);

  return 0;
}

int bus_destroy(bus_t *bus) {
  int rc = bus_kill(bus);
  assert(rc == 0);

  rc = pthread_join(bus->thread, NULL);
  assert(rc == 0);
  bus_task_t *next, *head = bus->head;
  while (head) {
    next = head->next;
    task_destroy(head);
    head = next;
  }

  if (bus->fd >= 0) {
    close(bus->fd);
    bus->fd = -1;
  }
  return 0;
}
