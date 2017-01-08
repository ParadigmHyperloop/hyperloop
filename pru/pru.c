/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

/**
 * This file contains PSEUDO-code for the pru.  This is purly laying out the
 * logical requirements of the pru.
 *
 * get: read from an ADC pin and shove it into the shared memory address
 * set: set the given gpio pin with the given value (High/Low)
 * wait: wait for the given amount of time (amount of time for ADC to read)
 */
#define PULSE_PIN 10

// Select pins for the muxes (each select pin goes to all 4 muxes)
#define S0 11
#define S1 12
#define S2 13
#define S3 14

// Base Address for the PRU Shared Memory
#define BASE_ADDR 0x00

// Number of mux channels to read normally (Non-pulsed)
// Channels 0-13 Normal sensors
// Channels 14-15 7 OMRON sensors
#define STD_CHANNELS 14

#define MUXES 4

// ADC speed is 200khz, muxed accross 7 or 8 channels so 25khz
#define ADC_READ_TIME 0.04 // microseconds // 1/25khz

#define PWM_CYCE_FULL_COUNT 100 // Arbitrary, need to change
#define PWM_CYCE_LOW_COUNT 75 // 25% duty cycle

// Set the mux given a channel
#define MUX_SET(channel) do { \
  set(S0, (channel) & 0x01); \
  set(S1, (channel) & 0x02); \
  set(S2, (channel) & 0x04); \
  set(S3, (channel) & 0x08); \
} while (0);

int main() {
  register unsigned int pwmc = 0;
  while (1) {
    int i, j;

    if (pwmc == PWM_CYCE_LOW_COUNT) {
      // Turn on the pulse pin for the last 25 iterations
      set(PULSE_PIN, 1);
    } else if (pwmc == PWM_CYCE_FULL_COUNT) {
      MUX_SET(14);

      // Wait for the ADC to read in all 8 AIN inputs
      wait(ADC_READ_TIME);

      // Read the 4 Skate OMRONS
      get(AIN0, BASE_ADDR + (MUXES * 14) + 0);
      get(AIN1, BASE_ADDR + (MUXES * 14) + 1);
      get(AIN2, BASE_ADDR + (MUXES * 14) + 2);
      get(AIN3, BASE_ADDR + (MUXES * 14) + 3);

      MUX_SET(15);

      wait(ADC_READ_TIME);

      // Read the 3 Wheel OMRONS
      get(AIN0, BASE_ADDR + (MUXES * 15) + 0);
      get(AIN1, BASE_ADDR + (MUXES * 15) + 1);
      get(AIN2, BASE_ADDR + (MUXES * 15) + 2);
      // get(AIN3, BASE_ADDR + (MUXES * 15) + 3);

      // Turn off the pulse pin
      set(PULSE_PIN, 0);

      // Reset PWM cycle
      pwmc = 0;
    }

    for (i=0; i<STD_CHANNELS; i++) {
      SET_MUX(i);

      wait(ADC_READ_TIME);

      get(AIN0, BASE_ADDR + (MUXES * i) + 0);
      get(AIN1, BASE_ADDR + (MUXES * i) + 1);
      get(AIN2, BASE_ADDR + (MUXES * i) + 2);
      get(AIN3, BASE_ADDR + (MUXES * i) + 3);
    }

    // increment the pwm cycle by 1
    pwmc++;
  }


}
