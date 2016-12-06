// The program sets the analog mux from (0 - 15) and performs a read
// This program collects ADC from AIN0 with certain sampling rate.
// The collected data are stored into PRU shared memory(buffer) first.
// The host program(ADCCollector.c) will read the stored ADC data
// This program uses double buffering technique. 
// The host program can recognize the buffer status by buffer status variable
// 0 means empty, 1 means first buffer is ready, 2 means second buffer is ready.
// When each buffer is ready, host program read ADC data from the buffer.


.origin 0 // offset of the start of the code in PRU memory
.entrypoint START // program entry point, used by debugger only

#include "ADCCollector.hp"

#define BUFF_SIZE 0x00000fa0 //Total buff size: 4kbyte(Each buffer has 2kbyte: 500 piece of data) //***//previously was 00000fa0
#define HALF_SIZE BUFF_SIZE / 2

#define SAMPLING_RATE 200000 //Sampling rate(16khz) //***//16000
#define DELAY_MICRO_SECONDS (1000000 / SAMPLING_RATE) //Delay by sampling rate
#define CLOCK 200000000 // PRU is always clocked at 200MHz
#define CLOCKS_PER_LOOP 2 // loop contains two instructions, one clock each
#define DELAYCOUNT DELAY_MICRO_SECONDS * CLOCK / CLOCKS_PER_LOOP / 1000 / 1000 * 3  //if sampling rate = 98000 --> = 3061.224
#define MUX_SIZE 15 // (0 - 15)

.macro MUX_SELECT
	QBEQ MUX0, r1, 0 //jump to mux0
	QBEQ MUX1, r1, 1 //jump to mux1
	QBEQ MUX2, r1, 2 //jump to mux2
	QBEQ MUX3, r1, 3 //jump to mux3
	QBEQ MUX4, r1, 4 //jump to mux4
	QBEQ MUX5, r1, 5 //jump to mux5
	QBEQ MUX6, r1, 6 //jump to mux6
	QBEQ MUX7, r1, 7 //jump to mux7
	QBEQ MUX8, r1, 8 //jump to mux8
	QBEQ MUX9, r1, 9 //jump to mux9
	QBEQ MUX10, r1, 10 //jump to mux10
	QBEQ MUX11, r1, 11 //jump to mux11
	QBEQ MUX12, r1, 12 //jump to mux12
	QBEQ MUX13, r1, 13 //jump to mux13
	QBEQ MUX14, r1, 14 //jump to mux14
	QBEQ MUX15, r1, 15 //jump to mux15
	
MUX0:
    clr r30, r30, 15   // set P8_11 low
	clr r30, r30, 14   // set P8_12 low
	clr r31, r31, 15   // set P8_15 low
	clr r31, r31, 14   // set P8_16 low
	QBA DONE

MUX1:
    set r30, r30, 15   // set P8_11 high
	QBA DONE

MUX2:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	QBA DONE

MUX3:
    set r30, r30, 15   // set P8_11 
	QBA DONE

MUX4:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	set r31, r31, 15   // set P8_15 
	QBA DONE

MUX5:
    set r30, r30, 15   // set P8_11  
	QBA DONE

MUX6:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	QBA DONE

MUX7:
    set r30, r30, 15   // set P8_11 
	QBA DONE

MUX8:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	clr r31, r31, 15   // set P8_15 
	set r31, r31, 14   // set P8_16 
	QBA DONE

MUX9:
    set r30, r30, 15   // set P8_11 
	QBA DONE

MUX10:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	QBA DONE

MUX11:
    set r30, r30, 15   // set P8_11 
	QBA DONE

MUX12:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	set r31, r31, 15   // set P8_15 
	QBA DONE

MUX13:
    set r30, r30, 15   // set P8_11 
	QBA DONE

MUX14:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	QBA DONE

MUX15:
    set r30, r30, 15   // set P8_11
	MOV r1, 0 //reset selection

DONE:	

.endm

.macro DELAY
    MOV r10, DELAYCOUNT
    DELAY:
        SUB r10, r10, 1
        QBNE DELAY, r10, 0
.endm

.macro READADC
    //Initialize buffer status (0: empty, 1: first buffer is ready, 2: second buffer is ready)
    MOV r2, 0
    SBCO r2, CONST_PRUSHAREDRAM, 0, 4 
	MOV r4, MUX_SIZE
	MOV r1, 0
		
    INITV:
        MOV r5, 0 //Shared RAM address of ADC Saving position 
        MOV r6, BUFF_SIZE  //Counting variable 
			
    READ:
		QBA MUX //Loop mux selection signal 
		
	//Read ADC from FIFO0DATA	
	CONTINUE:
		MOV r2, 0x44E0D100 
        LBBO r3, r2, 0, 4 
        //Add address counting
        ADD r5, r5, 4
        //Write ADC to PRU Shared RAM
        SBCO r3, CONST_PRUSHAREDRAM, r5, 4 

        DELAY
        
        SUB r6, r6, 4
        MOV r2, HALF_SIZE
        QBEQ CHBUFFSTATUS1, r6, r2 //If first buffer is ready
        QBEQ CHBUFFSTATUS2, r6, 0 //If second buffer is ready
        QBA READ

    //Change buffer status to 1
    CHBUFFSTATUS1:
        MOV r2, 1 
        SBCO r2, CONST_PRUSHAREDRAM, 0, 4
        QBA READ

    //Change buffer status to 2
    CHBUFFSTATUS2:
        MOV r2, 2
        SBCO r2, CONST_PRUSHAREDRAM, 0, 4
        QBA INITV
	
    //Send event to host program
    MOV r31.b0, PRU0_ARM_INTERRUPT+16 
    HALT
	
	MUX:
		MUX_SELECT
		ADD r1,r1, 1
		QBA CONTINUE
.endm

// Starting point
START:
    // Enable OCP master port
    LBCO r0, CONST_PRUCFG, 4, 4      //#define CONST_PRUCFG	     C4     taken from ADCCollector.hp
    CLR r0, r0, 4
    SBCO r0, CONST_PRUCFG, 4, 4

    //C28 will point to 0x00012000 (PRU shared RAM)
    MOV r0, 0x00000120
    MOV r1, CTPPR_0
    ST32 r0, r1

    //Init ADC CTRL register
    MOV r2, 0x44E0D040
    MOV r3, 0x00000005
    SBBO r3, r2, 0, 4

    //Enable ADC STEPCONFIG 1
    MOV r2, 0x44E0D054
    MOV r3, 0x00000002
    SBBO r3, r2, 0, 4
    
    //Init ADC STEPCONFIG 1
    MOV r2, 0x44E0D064
    MOV r3, 0x00000001 //continuous mode
    SBBO r3, r2, 0, 4

    //Read ADC and FIFOCOUNT
	READADC	