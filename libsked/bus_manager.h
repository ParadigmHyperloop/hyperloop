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

#ifndef PARADIGM_BUS_MANAGER_H
#define PARADIGM_BUS_MANAGER_H

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

#define BUS_BUFFER_SIZE 8

struct bus_task;

typedef enum bus_state { Error, Init, Run, Killed } bus_state_t;

typedef struct bus {
  // File descriptor associated with the bus
  int fd;
  // Human readable name for the bus
  const char *name;
  // Pointer to the first element in the queue
  struct bus_task *head;
  // Pointer to the last element in the queue
  struct bus_task *tail;
  // The state of the bus, Init, Run, Killed, ect. Used to flag the bus to stop
  bus_state_t state;
  // If a task finishes faster than the throttle time, the bus will sleep.
  useconds_t throttle;
  // A lock for mutating the bus queue
  pthread_mutex_t mutex;
  // The thread that the bus is executing in
  pthread_t thread;
  // A semaphore for flagging when a new task has been added to the bus
  sem_t *sem;
  // A single integer buffer for clients to safely save state info into
  uint32_t mem[BUS_BUFFER_SIZE];
} bus_t;

typedef void (^bus_block_t)(bus_t *bus);

typedef enum bus_task_mode {
  Single,
  Requeue,
  Cancelled
} bus_task_mode_t;

typedef struct bus_task {
  struct bus_task *next;
  struct bus_task *prev;
  bus_block_t execute;
  bus_task_mode_t mode;
} bus_task_t;

int bus_init(bus_t *bus, const char *name, int (^open_block)(void));

void bus_run(bus_t *bus);

int bus_destroy(bus_t *bus);

/**
 * Enqueues work to be performed on the given bus. A pointer to the bus_task_t
 * is returned to the user.  This pointer shall not be free'd by the user, but
 * may be used to cancel the task.
 */
bus_task_t * bus_enqueue(bus_t *bus, bus_block_t block);

/**
 * Sends an asyncronous kill command to the bus, which will stop the thread and
 * make it joinable.
 *
 * @param bus The bus you would like to kill
 *
 * @results -1 on failure to post to the busses semaphore, otherwise zero
 */
int bus_kill(bus_t *bus);

#endif /* PARADIGM_BUS_MANAGER_H */
