/******************************************************************************
* Include Files                                                               *
******************************************************************************/
// Standard header files
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Driver header file
#include <pruss/prussdrv.h>
#include <pruss/pruss_intc_mapping.h>

/******************************************************************************
* Local Macro Declarations                                                    * 
******************************************************************************/
#define PRU_NUM 	 0
#define OFFSET_SHAREDRAM 2048		//equivalent with 0x00002000

#define PRUSS0_SHARED_DATARAM    4
#define SAMPLING_RATE 16000//280000//98000 //16khz   //***//prev = 16000
#define BUFF_LENGTH SAMPLING_RATE
#define PRU_SHARED_BUFF_SIZE 500 //***// prev = 500
#define CNT_ONE_SEC SAMPLING_RATE / PRU_SHARED_BUFF_SIZE

/******************************************************************************
* Functions declarations                                                      * 
******************************************************************************/
//static void Enable_ADC();
//static void Enable_PRU();

static unsigned int ProcessingADC1(unsigned int value);
/*****************************************************************************
* Local Function Definitions                                                 *
*****************************************************************************/
/* Enable ADC */
static int Enable_ADC()
{
	FILE *ain;

	ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
	if(!ain){
		printf("\tERROR: /sys/devices/bone_capemgr.9/slots open failed\n");
		return -1;
	}
	fseek(ain, 0, SEEK_SET); 
	fprintf(ain, "cape-bone-iio");
	fflush(ain);
	return 0;
}

/* Enable PRU */
static int Enable_PRU()
{
		FILE *ain;

		ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
		if(!ain){
			printf("\tERROR: /sys/devices/bone_capemgr.9/slots open failed\n");
			return -1;
		}
		fseek(ain, 0, SEEK_SET); 
		fprintf(ain, "BB-BONE-PRU-01");
		fflush(ain);
		return 0;
}

/* 
 * FIFO0DATA register includes both ADC and channelID
 * so we need to remove the channelID
 */
static unsigned int ProcessingADC1(unsigned int value)
{
	unsigned int result = 0;

	result = value << 20;
	result = result >> 20;

	return result;
}

/******************************************************************************
* Global variable Declarations                                                * 
******************************************************************************/
static void *sharedMem;
static unsigned int *sharedMem_int;

/******************************************************************************
* Main                                                                        * 
******************************************************************************/
int main (int argc, char* argv[])
{
	FILE *fp_out;
    unsigned int ret;
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	int i = 0, cnt = 0, total_cnt = 0;
	int target_buff = 1;
	int sampling_period = 0;

	if(argc != 2){
		printf("\tERROR: Sampling period is required by second\n");
		printf("\t       %s [sampling period]\n", argv[0]);
		return 0;
	}
	sampling_period = atoi(argv[1]);

	/* Enable PRU */
	Enable_PRU();
	/* Enable ADC */
	Enable_ADC();
	/* Initializing PRU */
    prussdrv_init();
    ret = prussdrv_open(PRU_EVTOUT_0);
    if (ret){
        printf("\tERROR: prussdrv_open open failed\n");
        return (ret);
    }
    prussdrv_pruintc_init(&pruss_intc_initdata);
    printf("\tINFO: Initializing.\r\n");
    prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &sharedMem);
    sharedMem_int = (unsigned int*) sharedMem;
	
	/* Open save file */
	fp_out = fopen("Results.txt", "w");
	if(fp_out == NULL){
		printf("\tERROR: file open failed\n");
		return 0;
	}

	/* Executing PRU. */
	printf("\tINFO: Sampling is started for %d seconds\n", sampling_period);
    printf("\tINFO: Collecting");
    prussdrv_exec_program (PRU_NUM, "./ADCCollector.bin");
	/* Read ADC */
	int temp = 0;
	temp++;
	while(1){
		while(1){
			if(sharedMem_int[OFFSET_SHAREDRAM] == 1 && target_buff == 1){ // First buffer is ready
				for(i=0; i<PRU_SHARED_BUFF_SIZE; i++){
				fprintf(fp_out, "%d\n", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 1 + i]));
				temp =  ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 1 + i]);
					
				}
				target_buff = 2;
				break;
			}else if(sharedMem_int[OFFSET_SHAREDRAM] == 2 && target_buff == 2){ // Second buffer is ready
				for(i=0; i<PRU_SHARED_BUFF_SIZE; i++){
				fprintf(fp_out, "%d\n", ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + PRU_SHARED_BUFF_SIZE + 1 + i]));
			    temp =  ProcessingADC1(sharedMem_int[OFFSET_SHAREDRAM + 1 + i]);
				
				}
				target_buff = 1;
				break;
			}
		}

		if(++cnt == CNT_ONE_SEC){
			printf(".");
			total_cnt += cnt;
			cnt = 0;
		}

		if(total_cnt == CNT_ONE_SEC * sampling_period){
			printf("\n\tINFO: Sampling completed ...\n");
			break;
		}
	}

	fclose(fp_out);
    printf("\tINFO: PRU completed transfer.\r\n");
    prussdrv_pru_clear_event (PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

    /* Disable PRU*/
    prussdrv_pru_disable(PRU_NUM);
    prussdrv_exit();

    return(0);
}


