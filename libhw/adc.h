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

#ifndef PARADIGM_ADC_H
#define PARADIGM_ADC_H

#include "hw.h"

typedef struct adc {
  // The ID number for the ADC (Used to switch the demux)
  uint32_t num;
  // The human identifiable name of the ADC
  const char *name;
  // The scheduler
  bus_t *bus;
  int fd;
} adc_t;

typedef struct adc_response_t {
  // The Return code for the ADC Value (0 on success, -1 on error)
  int rc;
  // The raw value read from the ADC
  int raw;
  // The ADC struct
  adc_t *adc;
} adc_response_t;

typedef int (^ADCBlock)(adc_response_t raw);

int init_adc(adc_t *adc, int fd, const char *name, int num);

int read_adc(adc_t *adc, uint8_t channel);

int set_gpio_for_adc(adc_t *adc);

uint16_t int_to_spi_channel(uint8_t channel);

int open_spi(void);

#endif /* PARADIGM_ADC_H */
