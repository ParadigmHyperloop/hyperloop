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

#include <sys/semaphore.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_HEAP_SIZE 64

struct bus_task;

typedef enum bus_state { Error, Init, Run, Killed } bus_state_t;

typedef struct bus {
  int fd;
  const char *name;
  struct bus_task *queue;
  struct bus_task *tail;
  bus_state_t state;
  useconds_t throttle;
  pthread_mutex_t mutex;
  pthread_t thread;
  sem_t *sem;
} bus_t;

typedef void (^bus_block_t)(bus_t *bus);

typedef struct bus_task {
  struct bus_task *next;
  struct bus_task *prev;
  bus_block_t execute;
} bus_task_t;

int bus_init(bus_t *bus, const char *name, int (^open_block)(void));

void bus_run(bus_t *bus);

int bus_destroy(bus_t *bus);

int bus_enqueue(bus_t *bus, bus_block_t block);

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
