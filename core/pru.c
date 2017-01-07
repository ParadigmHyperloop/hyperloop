// The program sets the analog mux from (0 - 15) and performs a read
// This program collects ADC from AIN0 to AIN7 with certain sampling rate.
// After all 7 AIN pins are read the mux 4 bit selection is incremented
// The collected data are stored into PRU shared memory(buffer) first.
// The PRU assembly program(ADCCollector.p) will read the stored ADC data in the
// shared buffer
// Writen by Upendra Naidoo
// Standard header files

// Handles all the PRU Magic

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "pod.h"

/******************************************************************************
* Local Macro Declarations                                                    *
******************************************************************************/
#define PRU_NUM 0
#define OFFSET_SHAREDRAM 2048 // equivalent with 0x00002000

#define PRUSS0_SHARED_DATARAM 4
#define NUM_ADS 7 // Num of A/Ds being used

/******************************************************************************
* Functions declarations                                                      *
******************************************************************************/
static unsigned int ProcessingADC1(unsigned int value);

/******************************************************************************
* Global variable Declarations                                                *
******************************************************************************/
static void *sharedMem;
static unsigned int *sharedMem_int;

int init_pru() {
  unsigned int ret;
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  int num_samples = 16000000; // TODO: Change this

  /* Initializing PRU */
  prussdrv_init();
  ret = prussdrv_open(PRU_EVTOUT_0);
  if (ret) {
    printf("\tERROR: prussdrv_open open failed\n");
    return (ret);
  }

  prussdrv_pruintc_init(&pruss_intc_initdata);
  printf("\tINFO: Initializing.\r\n");
  prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &sharedMem);

  sharedMem_int = (unsigned int *)sharedMem;
  sharedMem_int[OFFSET_SHAREDRAM + 0] = NUM_ADS;
  sharedMem_int[OFFSET_SHAREDRAM + 1] = num_samples;
  sharedMem_int[OFFSET_SHAREDRAM + 2] = 0;

  /* Executing PRU. */
  prussdrv_exec_program(PRU_NUM, "./ADCCollector.bin");
  printf("\tINFO: Sampling is started for %d samples\n", num_samples);
  printf("\tINFO: Collecting");
}

int pru_shutdown() {
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
  printf("\n\n\tINFO: Sampling completed ...\n");
  printf("\tINFO: PRU completed transfer.\r\n");

  /* Disable PRU*/
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();
}

/******************************************************************************
* Main                                                                        *
******************************************************************************/

/**
 * Read a complete sensor pack into the given sensor_pack_t
 */
int pru_read(sensor_pack_t *pack) {
  int i;
  /* Read ADC */

  printf("\n\n AIN0  AIN1  AIN2  AIN3  AIN4  AIN5  AIN6 \n");
  printf(" ----  ----  ----  ----  ----  ----  ----\n\n");
  for (i = 0; i < 16; i++) {

    while (!sharedMem_int[OFFSET_SHAREDRAM + 2]) {
      sleep(0.00000000001);
    }

    // memcpy(pack+(i*7), &(sharedMem_int[OFFSET_SHAREDRAM + 3]),
    // sizeof(unsigned int) * 7);

    printf(" %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 3]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 4]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 5]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 6]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 7]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 8]));
    printf("  %4d", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 9]));
    printf("\n");

    sharedMem_int[OFFSET_SHAREDRAM + 2] = 0;
  }

  return (0);
}

static unsigned int ProcessingADC1(unsigned int value) {
  // Clear upper 20 bits
  return value & 0x00000FFF;
}
