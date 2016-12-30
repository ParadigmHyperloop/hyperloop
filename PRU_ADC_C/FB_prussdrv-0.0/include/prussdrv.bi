

'/*
 '* prussdrv.h
 '*
 '* Describes PRUSS userspace driver for Industrial Communications
 '*
 '* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 '*
 '*
 '*  Redistribution and use in source and binary forms, with or without
 '*  modification, are permitted provided that the following conditions
 '*  are met:
 '*
 '*    Redistributions of source code must retain the above copyright
 '*    notice, this list of conditions and the following disclaimer.
 '*
 '*    Redistributions in binary form must reproduce the above copyright
 '*    notice, this list of conditions and the following disclaimer in the
 '*    documentation and/or other materials provided with the
 '*    distribution.
 '*
 '*    Neither the name of Texas Instruments Incorporated nor the names of
 '*    its contributors may be used to endorse or promote products derived
 '*    from this software without specific prior written permission.
 '*
 '*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 '*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 '*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 '*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 '*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 '*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 '*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 '*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 '*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 '*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 '*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 '*
'*/

'/*
 '* ============================================================================
 '* Copyright (c) Texas Instruments Inc 2010-11
 '*
 '* Use of this software is controlled by the terms and conditions found in the
 '* license agreement under which this software has been supplied or provided.
 '* ============================================================================
'*/


EXTERN "C" LIB "prussdrv" ' (h_2_bi -P_oCD option)

#IFNDEF _PRUSSDRV_H
#DEFINE _PRUSSDRV_H
' #include <sys/types.h>
#INCLUDE ONCE "crt/sys/types.bi" '__HEADERS__: sys/types.h

' #include <pthread.h>
' file not found: pthread.h

#DEFINE NUM_PRU_HOSTIRQS 8
#DEFINE NUM_PRU_HOSTS 10
#DEFINE NUM_PRU_CHANNELS 10
#DEFINE NUM_PRU_SYS_EVTS 64
#DEFINE PRUSS0_PRU0_DATARAM 0
#DEFINE PRUSS0_PRU1_DATARAM 1
#DEFINE PRUSS0_PRU0_IRAM 2
#DEFINE PRUSS0_PRU1_IRAM 3
#DEFINE PRUSS_V1 1
#DEFINE PRUSS_V2 2
#DEFINE PRUSS0_SHARED_DATARAM 4
#DEFINE PRUSS0_CFG 5
#DEFINE PRUSS0_UART 6
#DEFINE PRUSS0_IEP 7
#DEFINE PRUSS0_ECAP 8
#DEFINE PRUSS0_MII_RT 9
#DEFINE PRUSS0_MDIO 10
#DEFINE PRU_EVTOUT_0 0
#DEFINE PRU_EVTOUT_1 1
#DEFINE PRU_EVTOUT_2 2
#DEFINE PRU_EVTOUT_3 3
#DEFINE PRU_EVTOUT_4 4
#DEFINE PRU_EVTOUT_5 5
#DEFINE PRU_EVTOUT_6 6
#DEFINE PRU_EVTOUT_7 7

TYPE prussdrv_function_handler AS FUNCTION(BYVAL AS ANY PTR) AS ANY PTR

TYPE __sysevt_to_channel_map
  AS SHORT sysevt
  AS SHORT channel
END TYPE

TYPE tsysevt_to_channel_map AS __sysevt_to_channel_map

TYPE __channel_to_host_map
  AS SHORT channel
  AS SHORT host
END TYPE

TYPE tchannel_to_host_map AS __channel_to_host_map

TYPE __pruss_intc_initdata
  AS BYTE sysevts_enabled(NUM_PRU_SYS_EVTS-1)
  AS tsysevt_to_channel_map sysevt_to_channel_map(NUM_PRU_SYS_EVTS-1)
  AS tchannel_to_host_map channel_to_host_map(NUM_PRU_CHANNELS-1)
  AS UINTEGER host_enable_bitmask
END TYPE

TYPE tpruss_intc_initdata AS __pruss_intc_initdata

DECLARE FUNCTION prussdrv_init() AS INTEGER
DECLARE FUNCTION prussdrv_open(BYVAL host_interrupt AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_version() AS INTEGER
DECLARE FUNCTION prussdrv_strversion(BYVAL version AS INTEGER) AS CONST ZSTRING PTR
DECLARE FUNCTION prussdrv_pru_reset(BYVAL prunum AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_disable(BYVAL prunum AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_enable(BYVAL prunum AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_write_memory(BYVAL pru_ram_id AS UINTEGER, BYVAL wordoffset AS UINTEGER, BYVAL memarea AS CONST UINTEGER PTR, BYVAL bytelength AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pruintc_init(BYVAL prussintc_init_data AS CONST tpruss_intc_initdata PTR) AS INTEGER
DECLARE FUNCTION prussdrv_get_event_to_channel_map(BYVAL eventnum AS UINTEGER) AS SHORT
DECLARE FUNCTION prussdrv_get_channel_to_host_map(BYVAL channel AS UINTEGER) AS SHORT
DECLARE FUNCTION prussdrv_get_event_to_host_map(BYVAL eventnum AS UINTEGER) AS SHORT
DECLARE FUNCTION prussdrv_map_l3mem(BYVAL address AS ANY PTR PTR) AS INTEGER
DECLARE FUNCTION prussdrv_map_extmem(BYVAL address AS ANY PTR PTR) AS INTEGER
DECLARE FUNCTION prussdrv_extmem_size() AS UINTEGER
DECLARE FUNCTION prussdrv_map_prumem(BYVAL pru_ram_id AS UINTEGER, BYVAL address AS ANY PTR PTR) AS INTEGER
DECLARE FUNCTION prussdrv_map_peripheral_io(BYVAL per_id AS UINTEGER, BYVAL address AS ANY PTR PTR) AS INTEGER
DECLARE FUNCTION prussdrv_get_phys_addr(BYVAL address AS CONST ANY PTR) AS UINTEGER
DECLARE FUNCTION prussdrv_get_virt_addr(BYVAL phyaddr AS UINTEGER) AS ANY PTR
DECLARE FUNCTION prussdrv_pru_wait_event(BYVAL host_interrupt AS UINTEGER) AS UINTEGER
DECLARE FUNCTION prussdrv_pru_event_fd(BYVAL host_interrupt AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_send_event(BYVAL eventnum AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_clear_event(BYVAL host_interrupt AS UINTEGER, BYVAL sysevent AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_pru_send_wait_clear_event(BYVAL send_eventnum AS UINTEGER, BYVAL host_interrupt AS UINTEGER, BYVAL ack_eventnum AS UINTEGER) AS INTEGER
DECLARE FUNCTION prussdrv_exit() AS INTEGER
DECLARE FUNCTION prussdrv_exec_program(BYVAL prunum AS INTEGER, BYVAL filename AS CONST ZSTRING PTR) AS INTEGER
DECLARE FUNCTION prussdrv_start_irqthread(BYVAL host_interrupt AS UINTEGER, BYVAL priority AS INTEGER, BYVAL irqhandler AS prussdrv_function_handler) AS INTEGER

#ENDIF ' _PRUSSDRV_H

END EXTERN ' (h_2_bi -P_oCD option)
