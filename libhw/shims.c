/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#include "libBBB.h"
#include "shims.h"
// GPIO Prototypes
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#ifndef BBB
int initPin(int pinnum) { return 0; }
int setPinDirection(int pinnum, char *dir) { return 0; }

// PWM Prototypes
int initPWM(int mgrnum, char *pin) { return 0; }
int setPWMPeriod(int helpnum, char *pin, int period) { return 0; }
int setPWMDuty(int helpnum, char *pin, int duty) { return 0; }
int setPWMOnOff(int helpnum, char *pin, int run) { return 0; }

// UART Prototypes
int initUART(int mgrnum, char *uartnum) { return 0; }
void closeUART(int fd) {}
int configUART(UART u, int property, char *value) { return 0; }
int txUART(int uart, unsigned char data) { return 0; }
unsigned char rxUART(int uart) { return '\0'; }
int strUART(int uart, char *buf) { return 0; }

// I2C Prototypes
int initI2C(int modnum, int addr) { return 0; }
void closeI2C(int device) { return; }
int writeByteI2C(int device, unsigned char *data) { return 0; }
int writeBufferI2C(int device, unsigned char *buf, int len) { return 0; }
int readByteI2C(int device, unsigned char *data) { return 0; }
int readBufferI2C(int device, int numbytes, unsigned char *buf) { return 0; }

// SPI Prototypes
int initSPI(int modnum) { return 0; }
void closeSPI(int device) { return; }
int writeByteSPI(int device, unsigned char *data) { return 0; }
int writeBufferSPI(int device, unsigned char *buf, int len) { return 0; }
int readByteSPI(int device, unsigned char *data) { return 0; }
int readBufferSPI(int device, int numbytes, unsigned char *buf) { return 0; }

// ADC Prototypes
int initADC(int mgrnum) { return 0; }
int readADC(int helpnum, char *ach) { return 0; }

// Time Prototypes
void pauseSec(int sec) { return; }
int pauseNanoSec(long nano) { return 0; }
#endif /* BBB */

#ifndef HAS_PRU
int prussdrv_init(void) { return 0; }
int prussdrv_open(unsigned int host_interrupt) { return 0; }
int prussdrv_pruintc_init(const tpruss_intc_initdata *prussintc_init_data) {
  return 0;
}

int __pru_shared_mem__[4096];
int prussdrv_map_prumem(unsigned int pru_ram_id, void **address) {
  *address = &__pru_shared_mem__[0];
  return 0;
}
int prussdrv_pru_disable(unsigned int prunum) { return 0; }
int prussdrv_exit(void) { return 0; }
int prussdrv_exec_program(int prunum, const char *filename) { return 0; }
int prussdrv_pru_clear_event(unsigned int host_interrupt,
                             unsigned int sysevent) {
  return 0;
}
#endif /* HAS_PRU */
#pragma clang diagnostic pop
