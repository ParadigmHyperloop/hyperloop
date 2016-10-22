

'/*
 '* pruss_intc_mapping.h
 '*
 '* Example PRUSS INTC mapping for the application
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

#DEFINE AM33XX

#IFDEF AM33XX
#DEFINE PRU0_PRU1_INTERRUPT 17
#DEFINE PRU1_PRU0_INTERRUPT 18
#DEFINE PRU0_ARM_INTERRUPT 19
#DEFINE PRU1_ARM_INTERRUPT 20
#DEFINE ARM_PRU0_INTERRUPT 21
#DEFINE ARM_PRU1_INTERRUPT 22
#ELSE ' AM33XX
#DEFINE PRU0_PRU1_INTERRUPT 32
#DEFINE PRU1_PRU0_INTERRUPT 33
#DEFINE PRU0_ARM_INTERRUPT 34
#DEFINE PRU1_ARM_INTERRUPT 35
#DEFINE ARM_PRU0_INTERRUPT 36
#DEFINE ARM_PRU1_INTERRUPT 37
#ENDIF ' AM33XX

#DEFINE CHANNEL0 0
#DEFINE CHANNEL1 1
#DEFINE CHANNEL2 2
#DEFINE CHANNEL3 3
#DEFINE CHANNEL4 4
#DEFINE CHANNEL5 5
#DEFINE CHANNEL6 6
#DEFINE CHANNEL7 7
#DEFINE CHANNEL8 8
#DEFINE CHANNEL9 9
#DEFINE PRU0 0
#DEFINE PRU1 1
#DEFINE PRU_EVTOUT0 2
#DEFINE PRU_EVTOUT1 3
#DEFINE PRU_EVTOUT2 4
#DEFINE PRU_EVTOUT3 5
#DEFINE PRU_EVTOUT4 6
#DEFINE PRU_EVTOUT5 7
#DEFINE PRU_EVTOUT6 8
#DEFINE PRU_EVTOUT7 9
#DEFINE PRU0_HOSTEN_MASK &h0001
#DEFINE PRU1_HOSTEN_MASK &h0002
#DEFINE PRU_EVTOUT0_HOSTEN_MASK &h0004
#DEFINE PRU_EVTOUT1_HOSTEN_MASK &h0008
#DEFINE PRU_EVTOUT2_HOSTEN_MASK &h0010
#DEFINE PRU_EVTOUT3_HOSTEN_MASK &h0020
#DEFINE PRU_EVTOUT4_HOSTEN_MASK &h0040
#DEFINE PRU_EVTOUT5_HOSTEN_MASK &h0080
#DEFINE PRU_EVTOUT6_HOSTEN_MASK &h0100
#DEFINE PRU_EVTOUT7_HOSTEN_MASK &h0200

#DEFINE PRUSS_INTC_INITDATA TYPE<tpruss_intc_initdata>( _
  { PRU0_PRU1_INTERRUPT _
  , PRU1_PRU0_INTERRUPT _
  , PRU0_ARM_INTERRUPT _
  , PRU1_ARM_INTERRUPT _
  , ARM_PRU0_INTERRUPT _
  , ARM_PRU1_INTERRUPT _
  , CAST(BYTE, -1) }, _
  { TYPE<tsysevt_to_channel_map>(PRU0_PRU1_INTERRUPT, CHANNEL1) _
  , TYPE<tsysevt_to_channel_map>(PRU1_PRU0_INTERRUPT, CHANNEL0) _
  , TYPE<tsysevt_to_channel_map>(PRU0_ARM_INTERRUPT, CHANNEL2) _
  , TYPE<tsysevt_to_channel_map>(PRU1_ARM_INTERRUPT, CHANNEL3) _
  , TYPE<tsysevt_to_channel_map>(ARM_PRU0_INTERRUPT, CHANNEL0) _
  , TYPE<tsysevt_to_channel_map>(ARM_PRU1_INTERRUPT, CHANNEL1) _
  , TYPE<tsysevt_to_channel_map>(-1, -1)}, _
  { TYPE<tchannel_to_host_map>(CHANNEL0,PRU0) _
  , TYPE<tchannel_to_host_map>(CHANNEL1, PRU1) _
  , TYPE<tchannel_to_host_map>(CHANNEL2, PRU_EVTOUT0) _
  , TYPE<tchannel_to_host_map>(CHANNEL3, PRU_EVTOUT1) _
  , TYPE<tchannel_to_host_map>(-1, -1) }, _
  (PRU0_HOSTEN_MASK OR PRU1_HOSTEN_MASK OR PRU_EVTOUT0_HOSTEN_MASK OR PRU_EVTOUT1_HOSTEN_MASK) _
  )
