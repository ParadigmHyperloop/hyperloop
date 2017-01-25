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
static unsigned int ProcessingADC1(unsigned int value);

/******************************************************************************
* Global variable Declarations                                                *
******************************************************************************/
static void *sharedMem;
static unsigned int *sharedMem_int;

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

  sharedMem_int = (unsigned int *)sharedMem;
  sharedMem_int[OFFSET_SHAREDRAM + 0] = NUM_ADS;
  sharedMem_int[OFFSET_SHAREDRAM + 1] = num_samples;
  sharedMem_int[OFFSET_SHAREDRAM + 2] = 0;

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
 * Read a complete sensor pack into the given sensor_pack_t
 */
int pru_read(sensor_pack_t *pack) {
  int i;
  uint32_t *packp = pack;
  /* Read ADC */
  // TODO: Remove Print
  printf("\n\n AIN0  AIN1  AIN2  AIN3  AIN4  AIN5  AIN6 \n");
  printf(" ----  ----  ----  ----  ----  ----  ----\n\n");
  for (i = 0; i < 16; i++) {
    int j = 0;
    while (!sharedMem_int[OFFSET_SHAREDRAM + 2] && j < 1000) {
      // sleep(0.00000000001);
      usleep(1);
      j++;
    }

    if (j >= 1000) {
      error("PRU Timeout Occured");
      return -1;
    }


    // sizeof(unsigned int) * 7);
    uint32_t a0 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 3]);
    uint32_t a1 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 4]);
    uint32_t a2 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 5]);
    uint32_t a3 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 6]);
    uint32_t a4 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 7]);
    uint32_t a5 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 8]);
    uint32_t a6 = ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 9]);


    *(packp+i+(0*16)) = a0;
    *(packp+i+(1*16)) = a1;
    *(packp+i+(2*16)) = a2;
    *(packp+i+(3*16)) = a3;
    *(packp+i+(4*16)) = a4;
    *(packp+i+(5*16)) = a5;
    *(packp+i+(6*16)) = a6;

    // memcpy(packp+i+(2*16), &(a2), sizeof(a2));
    // memcpy(packp+i+(3*16), &(a3), sizeof(a3));
    // memcpy(pack+i+(4*16), &(a4), sizeof(a4));
    // memcpy(pack+i+(5*16), &(a5), sizeof(a5));
    // memcpy(pack+i+(6*16), &(a6), sizeof(a6));
    //
    printf(" %4d", a0);
    printf(" %4d", a1);
    printf(" %4d", a2);
    printf(" %4d", a3);
    printf(" %4d", a4);
    printf(" %4d", a5);
    printf(" %4d", a6);
    printf("\n");

    sharedMem_int[OFFSET_SHAREDRAM + 2] = 0;
  }

  printf("========= Sensor Pack ===========\n");
  for (i=0;i<sizeof(sensor_pack_t)/sizeof(uint32_t); i++) {
    printf("%d %d: %4d\n", i / 16, i % 16, *(packp+i));
  }

  return (0);
}

static unsigned int ProcessingADC1(unsigned int value) {
  // Clear upper 20 bits
  return value & 0x00000FFF;
}
