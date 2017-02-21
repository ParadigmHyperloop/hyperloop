#ifndef OPENLOOP_PRU_H
#define OPENLOOP_PRU_H

#include "pod-helpers.h"
#include "pod.h"

#include <errno.h>
#include <sys/mman.h>
#ifdef HAS_PRU
#include <pruss_intc_mapping.h>
#include <prussdrv.h>
#else
#include "shims.h"
#endif

int pru_init(void);
int pru_read(pod_t *pod);
int pru_shutdown(void);

#endif
