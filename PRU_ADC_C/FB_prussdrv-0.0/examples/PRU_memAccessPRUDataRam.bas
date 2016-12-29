'' This is file PRU_memAccessPRUDataRam.bas, an example to access
'' the data ram from host (ARM) and PRU.

'' Licence: GPLv3, translated from C source by
'' (C) 2014 by Thomas{ DoT ]Freiherr[ aT ]gmx[ dOt }net

'' Here's the original licence info:
/'
 * PRU_memAccessPRUDataRam.c
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
'/

/'
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2010-12
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 '/

/'****************************************************************************
* PRU_memAccessPRUDataRam.c
*
* The PRU reads and stores values into the PRU Data RAM memory. PRU Data RAM
* memory has an address in both the local data memory map and global memory
* map. The example accesses the local Data RAM using both the local address
* through a register pointed base address and the global address pointed by
* entries in the constant table.
*
*****************************************************************************'/


/'****************************************************************************
* Include Files                                                              *
****************************************************************************'/

'// Driver header file
#INCLUDE ONCE "BBB/prussdrv.bi"
#INCLUDE ONCE "BBB/pruss_intc_mapping.bi"

#INCLIB "pthread"

/'****************************************************************************
* Explicit External Declarations                                             *
****************************************************************************'/

/'****************************************************************************
* Local Macro Declarations                                                   *
****************************************************************************'/

#DEFINE PRU_NUM   0
#DEFINE ADDEND1   &h0010F012u
#DEFINE ADDEND2   &h0000567Au

#DEFINE AM33XX

/'****************************************************************************
* Local Typedef Declarations                                                 *
****************************************************************************'/


/'****************************************************************************
* Local Variable Definitions                                                 *
****************************************************************************'/


/'****************************************************************************
* Intertupt Service Routines                                                 *
****************************************************************************'/


/'****************************************************************************
* Global Variable Definitions                                                *
****************************************************************************'/

' static void *pruDataMem;
STATIC SHARED AS ANY PTR pruDataMem
' static unsigned int *pruDataMem_int;
STATIC SHARED AS UINTEGER PTR pruDataMem_int

/'****************************************************************************
* Local Function Definitions                                                 *
****************************************************************************'/

FUNCTION LOCAL_exampleInit(BYVAL pruNum AS USHORT) AS INTEGER
    '//Initialize pointer to PRU data memory
    IF pruNum = 1 THEN
      prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, @pruDataMem)
    ELSE
      prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, @pruDataMem)
    END IF
    pruDataMem_int = CAST(UINTEGER PTR, pruDataMem)

    '// Flush the values in the PRU data memory locations
    pruDataMem_int[1] = &h00
    pruDataMem_int[2] = &h00

    RETURN 0
END FUNCTION

FUNCTION LOCAL_examplePassed(BYVAL pruNum AS USHORT) AS USHORT
  RETURN (pruDataMem_int[1] = ADDEND1) ANDALSO _
         (pruDataMem_int[2] = ADDEND1 + ADDEND2)
END FUNCTION

/'****************************************************************************
* Main                                                                       *
****************************************************************************'/

VAR initdata = PRUSS_INTC_INITDATA

PRINT !"\tINFO: Starting " & COMMAND(0) & !" example.\r\n";
'/* Initialize the PRU */
prussdrv_init()

'/* Open PRU Interrupt */
VAR ret = prussdrv_open(PRU_EVTOUT_0)
IF ret THEN
  PRINT !"prussdrv_open open failed\n";
  END ret
END IF

'/* Get the interrupt initialized */
prussdrv_pruintc_init(@initdata)

'/* Initialize example */
PRINT !"\tINFO: Initializing example.\r\n";
LOCAL_exampleInit(PRU_NUM)

'/* Execute example on PRU */
PRINT !"\tINFO: Executing example.\r\n";
prussdrv_exec_program (PRU_NUM, "./PRU_memAccessPRUDataRam.bin")


'/* Wait until PRU0 has finished execution */
PRINT !"\tINFO: Waiting for HALT command.\r\n";
prussdrv_pru_wait_event(PRU_EVTOUT_0)
PRINT !"\tINFO: PRU completed transfer.\r\n";
prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT)

'/* Check if example passed */
IF LOCAL_examplePassed(PRU_NUM) THEN
  PRINT !"INFO: Example executed succesfully.\r\n";
ELSE
  PRINT !"INFO: Example failed.\r\n";
END IF

'/* Disable PRU and close memory mapping*/
prussdrv_pru_disable(PRU_NUM)
prussdrv_exit()


