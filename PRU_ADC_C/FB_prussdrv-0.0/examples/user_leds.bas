'' This is file user_leds.bas, an example to access output GPIOs
'' from PRU.

'' Licence GPLv3
'' (C) 2014 by Thomas{ DoT ]Freiherr[ aT ]gmx[ dOt }net

'' Thanks Charles Steinkuehler for his PD pasm code (user_leds.p).

'// Driver header file
#INCLUDE ONCE "BBB/prussdrv.bi"
#INCLUDE ONCE "BBB/pruss_intc_mapping.bi"

CONST PRU_NUM = 0

PRINT !"\tINFO: Starting " & COMMAND(0) & !" example.\r\n";
'/* Initialize the PRU */
prussdrv_init()

'/* Open PRU Interrupt */
IF prussdrv_open(PRU_EVTOUT_0) THEN
  PRINT !"prussdrv_open open failed\n";
ELSE
  '/* Execute example on PRU */
  PRINT !"\tINFO: Executing example.\r\n";
  IF prussdrv_exec_program (PRU_NUM, "./user_leds.bin") THEN
    PRINT !"prussdrv_exec_program open failed\n";
  ELSE
    PRINT !"\tINFO: example running, press any key.\r\n";
    SLEEP
  END IF
END IF

'/* Disable PRU and close memory mapping*/
prussdrv_pru_disable(PRU_NUM)
prussdrv_exit()

