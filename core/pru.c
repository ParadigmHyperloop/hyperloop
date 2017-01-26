// The program sets the analog mux from (0 - 15) and performs a read
// This program collects ADC from AIN0 to AIN7 with certain sampling rate.
// After all 7 AIN pins are read the mux 4 bit selection is incremented
// The collected data are stored into PRU shared memory(buffer) first.
// The PRU assembly program(ADCCollector.p) will read the stored ADC data in the
// shared buffer
// Writen by Upendra Naidoo
// Standard header files

// Handles all the PRU Magic

#include "pru.h"

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
static unsigned int pru_get_int(unsigned int value);

/******************************************************************************
* Global variable Declarations                                                *
******************************************************************************/
static void *sharedMem;
static unsigned int *pru_shared_mem_int;

int pru_init() {
  unsigned int ret;
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  int num_samples = 16000000; // TODO: Change this
  // TODO: Remove print
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

  pru_shared_mem_int = (unsigned int *)sharedMem;
  pru_shared_mem_int[OFFSET_SHAREDRAM + 0] = NUM_ADS;
  pru_shared_mem_int[OFFSET_SHAREDRAM + 1] = num_samples;
  pru_shared_mem_int[OFFSET_SHAREDRAM + 2] = 0;

  /* Executing PRU. */
  prussdrv_exec_program(PRU_NUM, "./ADCCollector.bin");
  printf("\tINFO: Sampling is started for %d samples\n", num_samples);
  printf("\tINFO: Collecting");
  // TODO: Blindly returning 0
  return 0;
}

int pru_shutdown() {
  prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
  printf("\n\n\tINFO: Sampling completed ...\n");
  printf("\tINFO: PRU completed transfer.\r\n");

  /* Disable PRU*/
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();
  // TODO: Blindly returning 0
  return 0;
}

/******************************************************************************
* Main                                                                        *
******************************************************************************/

/**
 * Read a complete set of sensors from the PRU
 */
int pru_read(pod_t * pod) {
  int input;
  /* Read ADC */
  // TODO: Remove Print
  printf("\n\n AIN0  AIN1  AIN2  AIN3  AIN4  AIN5  AIN6 \n");
  printf(" ----  ----  ----  ----  ----  ----  ----\n\n");

  // Loop over the external 16 channels
  for (input = 0; input < 16; input++) {
    int j = 0;
    while (!pru_shared_mem_int[OFFSET_SHAREDRAM + 2] && j < 1000) {
      usleep(1);
      j++;
    }

    if (j >= 1000) {
      error("PRU Timeout Occured");
      return -1;
    }

    int mux;
    // Get the 7 internal AIN readings
    for (mux=0;mux<7;mux++) {
      sensor_t *s = get_sensor_by_address(pod, mux, input);
      if (s != NULL) {
        uint32_t val = pru_get_int(pru_shared_mem_int[OFFSET_SHAREDRAM + 3 + mux]);

        printf(" %4d", val);

        update_sensor(s, val);
      } else {
        printf("     ");
      }
    }

    printf("\n");

    pru_shared_mem_int[OFFSET_SHAREDRAM + 2] = 0;
  }

  return (0);
}

static unsigned int pru_get_int(unsigned int value) {
  // Clear upper 20 bits
  return value & 0x00000FFF;
}
