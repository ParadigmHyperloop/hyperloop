#ifndef OPENLOOP_PRU_H
#define OPENLOOP_PRU_H

#include "pod.h"
#include <sys/mman.h>
#include <errno.h>
#ifdef __linux__
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#else
#include "shims.h"
#endif


int pru_init(void);
int pru_read(sensor_pack_t *pack);
int pru_shutdown(void);

#endif
