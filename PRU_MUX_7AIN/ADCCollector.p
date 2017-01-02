// The program sets the analog mux from (0 - 15) and performs a read
// This program collects ADC from AIN0 to AIN7 with certain sampling rate.
// After all 7 AIN pins are read the mux 4 bit selection is incremented
// The collected data are stored into PRU shared memory(buffer) first.
// The host program(ADCCollector.c) will read the stored ADC data
// Writen by Upendra Naidoo

.origin 0 // offset of the start of the code in PRU memory
.entrypoint START // program entry point, used by debugger only

#include "ADCCollector.hp"
.macro MUX_SELECT
	QBEQ MUX0, r4, 0 //jump to mux0
	QBEQ MUX1, r4, 1 //jump to mux1
	QBEQ MUX2, r4, 2 //jump to mux2
	QBEQ MUX3, r4, 3 //jump to mux3
	QBEQ MUX4, r4, 4 //jump to mux4
	QBEQ MUX5, r4, 5 //jump to mux5
	QBEQ MUX6, r4, 6 //jump to mux6
	QBEQ MUX7, r4, 7 //jump to mux7
	QBEQ MUX8, r4, 8 //jump to mux8
	QBEQ MUX9, r4, 9 //jump to mux9
	QBEQ MUX10, r4, 10 //jump to mux10
	QBEQ MUX11, r4, 11 //jump to mux11
	QBEQ MUX12, r4, 12 //jump to mux12
	QBEQ MUX13, r4, 13 //jump to mux13
	QBEQ MUX14, r4, 14 //jump to mux14
	QBEQ MUX15, r4, 15 //jump to mux15
	
MUX0:
    clr r30, r30, 15   // set P8_11 low
	clr r30, r30, 14   // set P8_12 low
	clr r30, r30, 7    // set P9_25 low
	clr r30, r30, 5    // set P9_27 low
	QBA INCREMENT

MUX1:
    set r30, r30, 15   // set P8_11 high
	clr r30, r30, 14   // set P8_12 low
	clr r30, r30, 7    // set P9_25 low
	clr r30, r30, 5    // set P9_27 low
	QBA INCREMENT

MUX2:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	clr r30, r30, 7    // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX3:
    set r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	clr r30, r30, 7    // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX4:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	set r30, r30, 7   // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX5:
    set r30, r30, 15   // set P8_11  
	clr r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX6:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX7:
    set r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	set r30, r30, 7   // set P9_25 
	clr r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX8:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	clr r30, r30, 7    // set P9_25 
	set r30, r30, 5   // set P9_27 
	QBA INCREMENT

MUX9:
    set r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	clr r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX10:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12
	clr r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX11:
    set r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12
	clr r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX12:
    clr r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX13:
    set r30, r30, 15   // set P8_11 
	clr r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX14:
    clr r30, r30, 15   // set P8_11 
	set r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	QBA INCREMENT

MUX15:
    set r30, r30, 15   // set P8_11
	set r30, r30, 14   // set P8_12 
	set r30, r30, 7    // set P9_25 
	set r30, r30, 5    // set P9_27 
	MOV r4, -1 //reset selection

INCREMENT:
	ADD r4, r4, 1
	
.endm

.macro RESETADC
    //Enable ADC STEPCONFIG 1-7
    MOV r2, 0x44E0D054
    MOV r3, 0x000000FE
    SBBO r3, r2, 0, 4

    MOV r5, 8 //Offset to pass in num_samples,num_ads,...
    MOV r8, 0 //current count
	MUX_SELECT
.endm

.macro READADC    
    READ:
	//Data avail?
	MOV r2, 0x44E0D0E4
	LBBO r3, r2, 0, 4
	QBEQ READ, r3, 0

        //Read ADC from FIFO
        MOV r2, 0x44E0D100 
        LBBO r3, r2, 0, 4
        //Add address counting
        ADD r5, r5, 4
        //Write ADC to PRU Shared RAM
        SBCO r3, CONST_PRUSHAREDRAM, r5, 4 
        
	ADD r8,r8,1 //current count

        QBNE READ, r8, r6
        ADD r9,r9,1
        QBEQ END, r9, r7

        //Tell host to read
        MOV r3, 1
        SBCO r3, CONST_PRUSHAREDRAM, 8, 4

        //Wait for host to read
        WAIT:
            LBCO r3, CONST_PRUSHAREDRAM, 8, 4
            QBNE WAIT, r3, 0

        RESETADC
    	QBA READ

    END:
        //Tell host to read
        MOV r3, 1
        SBCO r3, CONST_PRUSHAREDRAM, 8, 4

    	//Disable TSC_ADC_SS
    	//Set Writable
    	MOV   r2, 0x44E0D040
    	MOV   r3, 0x00000004
    	SBBO  r3, r2, 0, 4   

    	//Send event to host program
    	MOV r31.b0, PRU0_ARM_INTERRUPT+16 
    	HALT
.endm

START:
    // Enable OCP master port
    LBCO r0, CONST_PRUCFG, 4, 4
    CLR r0, r0, 4
    SBCO r0, CONST_PRUCFG, 4, 4

    //C28 will point to 0x00012000 (PRU shared RAM)
    MOV r0, 0x00000120
    MOV r1, CTPPR_0
    ST32 r0, r1

    //Reset SYSConfig Register to 0
    MOV  r2, 0x44E0D010 // load register address
    MOV  r3, 0x00000000
    SBBO r3, r2, 0, 4   // set register

    //Wait for ADC to be idle
    AdcIdle:
    	MOV  r2, 0x44E0D044 // load register address
    	QBBS AdcIdle, r2, 5

    //Disable TSC_ADC_SS
    //Set Writable
    MOV   r2, 0x44E0D040
    MOV   r3, 0x00000004
    SBBO  r3, r2, 0, 4   

    //Enable ADC STEPCONFIG 1-7
    MOV r2, 0x44E0D054
    MOV r3, 0x000000FE
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 1
    MOV r2, 0x44E0D064
    MOV r3, 0x00000010 //AIN0
    SBBO r3, r2, 0, 4
   
    //Init ADC STEPCONFIG 2
    MOV r2, 0x44E0D06C
    MOV r3, 0x00080010 //AIN1
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 3
    MOV r2, 0x44E0D074
    MOV r3, 0x00100010 //AIN2
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 4
    MOV r2, 0x44E0D07C
    MOV r3, 0x00180010 //AIN3
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 5
    MOV r2, 0x44E0D084
    MOV r3, 0x00200010 //AIN4
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 6
    MOV r2, 0x44E0D08C
    MOV r3, 0x00280010 //AIN5
    SBBO r3, r2, 0, 4

    //Init ADC STEPCONFIG 6
    MOV r2, 0x44E0D094
    MOV r3, 0x00300010 //AIN6
    SBBO r3, r2, 0, 4

    //Enable TSC_ADC_SS
    //Set writable
    MOV   r2, 0x44E0D040
    MOV   r3, 0x00000005
    SBBO  r3, r2, 0, 4


    MOV r5, 8 //Offset to pass in num_samples,num_ads,...
    MOV r8, 0 //current count
    MOV r9, 0 //total count
    LBCO r6, CONST_PRUSHAREDRAM, 0, 4 //num_ads
    LBCO r7, CONST_PRUSHAREDRAM, 4, 4 //num_samples

	MOV r4, 0
    //Read ADC and FIFOCOUNT
	
	MUX_SELECT
    READADC
