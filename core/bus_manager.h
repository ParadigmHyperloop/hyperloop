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

#include "pod.h"

#ifndef PARADIGM_BUS_MANAGER_H
#define PARADIGM_BUS_MANAGER_H

#define MAX_HEAP_SIZE 64

struct bus_task;

typedef enum bus_task_variant {
  Immediate,
  Single,
  Looping
} bus_task_variant_t;

typedef struct bus {
  int fd;
  char name[16];
  struct bus_task ** queue;
  ring_buf_t buf;
  struct bus_task * heap;
  uint16_t heap_size;
  uint64_t heap_map;
  pthread_mutex_t mutex;
  sem_t *sem;
} bus_t;

typedef void (^bus_block_t)(bus_t * bus);

typedef struct bus_task {
  struct bus_task * next;
  struct bus_task * prev;
  bus_block_t execute;
  useconds_t min_time;
  bus_task_variant_t variant;
  
} bus_task_t;


int bus_init(bus_t *bus, char *name, int (^open_block)(void));

int bus_destroy(bus_t *bus);


int bus_enqueue(bus_t * bus, bus_block_t block, bus_task_variant_t variant);

void * bus_main(bus_t * bus);

#endif /* PARADIGM_BUS_MANAGER_H */
