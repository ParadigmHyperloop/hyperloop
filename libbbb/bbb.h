//
//  bbb.h
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

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


// GPIO Prototypes
ssize_t init_pin(gpio_t pin);
ssize_t set_pin_dir(gpio_t pin, gpio_dir_t value);
ssize_t set_pin_value(gpio_t pin, gpio_value_t value);
gpio_value_t get_pin_value(gpio_t pin);
#endif /* bbb_h */
