#ifndef OPENLOOP_PRU_H
#define OPENLOOP_PRU_H

#include "pod.h"
#include "pod-helpers.h"

#include <sys/mman.h>
#include <errno.h>
#ifdef HAS_PRU
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#else
#include "shims.h"
#endif

int pru_init(void);
int pru_read(pod_t *pod);
int pru_shutdown(void);

#endif
