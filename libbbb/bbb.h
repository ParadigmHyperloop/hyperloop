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

#ifndef bbb_h
#define bbb_h

#include <stdio.h>

#define SYSFS_GPIO_MAX_PATH 64

#define SYSFS_GPIO_BASE "/sys/class/gpio"
#define SYSFS_GPIO_PIN_FMT SYSFS_GPIO_BASE "/gpio%d/%s"

#define SYSFS_GPIO_EXPORT "export"
#define SYSFS_GPIO_EXPORT_FULL SYSFS_GPIO_BASE "/" SYSFS_GPIO_BASE

#define SYSFS_GPIO_LOW "0"
#define SYSFS_GPIO_HIGH "1"

#define SYSFS_GPIO_IN "in"
#define SYSFS_GPIO_OUT "out"

#define SYSFS_GPIO_DIR_FILE "direction"
#define SYSFS_GPIO_VAL_FILE "value"

typedef int gpio_t;

// TODO: Determine if it's C99 standard to store enums as (un)signed ints...
// then assign explicit values to each enum case
typedef enum gpio_value {
  kGpioValError,
  kGpioHigh,
  kGpioLow
} gpio_value_t;

typedef enum gpio_dir {
  kGpioDirError,
  kGpioOut,
  kGpioIn
} gpio_dir_t;

// SYSFS Helpers
ssize_t sysfs_write(int pin, char *op, char *data);
ssize_t sysfs_read(int pin, char *op, char *data, size_t len);
  
  
// GPIO Prototypes
ssize_t init_pin(gpio_t pin);
ssize_t set_pin_direction(gpio_t pin, gpio_dir_t value);
gpio_dir_t get_pin_direction(gpio_t pin);
ssize_t set_pin_value(gpio_t pin, gpio_value_t value);
gpio_value_t get_pin_value(gpio_t pin);
#endif /* bbb_h */
