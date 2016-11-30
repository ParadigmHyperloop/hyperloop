/* Copyright (c) 2013, Ville Mölsä
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *   Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <wordexp.h>
#include <unistd.h>

#include "bbb.h"

//	Utils

char *bbb_wildCardPath(char *path) {
	size_t size = 0;
	char *ptr = NULL;
	static char ret[1024];
	wordexp_t p;
	struct stat st;

	wordexp(path, &p, 0);

	if (p.we_wordc > 0) {
		size = strlen(*(p.we_wordv));

		if (size < sizeof(ret) && stat(*(p.we_wordv), &st) == 0) {
			memset(ret, 0, sizeof(ret));
			memcpy(ret, *(p.we_wordv), size);
			ptr = ret;
		}
	}

	wordfree(&p);
	return ptr;
}

//	Get Index

int bbb_getIndexByStr(char *str) {
	int size = strlen(str);
	int header = 0;
	int pin = 0;

	if (str[0] == 'P' && size >= 4) {
		if (str[1] == '8') {
			header = 8;
		} else {
			if (str[1] == '9') {
				header = 9;
			} else {
				return -1;
			}
		}
	} else {
		return -1;
	}

	if (str[2] == '_') {
		pin = atoi((str + 3));
	} else {
		return -1;
	}

	return bbb_getIndexByPin(header, pin);
}

int bbb_getIndexByPin(int header, int pin) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		if (bbb_pinout_table[i].header == header && bbb_pinout_table[i].pin == pin) {
			return i;
		}
	}

	return -1;
}

int bbb_getIndexByGpio(int gpio) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		if (bbb_pinout_table[i].gpio == gpio) {
			return i;
		}
	}

	return -1;
}

int bbb_getIndexBySignalName(char *signalname) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		if (strcmp(bbb_pinout_table[i].sig, signalname) == 0) {
			return i;
		}
	}

	return -1;
}

int bbb_getIndexByName(char *name) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		if (strcmp(bbb_pinout_table[i].name, name) == 0) {
			return i;
		}
	}

	return -1;
}

//	Index to values

int bbb_getHeader(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].header;
	}

	return -1;
}

int bbb_getPin(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].pin;
	}

	return -1;
}

int bbb_getGpio(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].gpio;
	}

	return 0;
}

bbb_pin_type bbb_getType1(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].type1;
	}

	return 0;
}

bbb_pin_type bbb_getType2(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].type2;
	}

	return 0;
}

bbb_pin_type bbb_getType3(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].type3;
	}

	return 0;
}

char *bbb_getSignalName(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].name;
	}

	return "undefined";
}

char *bbb_getName(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].name;
	}

	return "undefined";
}

// 	Index to String Values

char *bbb_getPinStrByIndex(int index) {
	static char pinstr[10];

	if (index >= 0 && index <= bbb_table_size) {
		memset(pinstr, 0, sizeof(pinstr));
		snprintf(pinstr, sizeof(pinstr), "P%d_%d", bbb_pinout_table[index].header, bbb_pinout_table[index].pin);

		return pinstr;
	}

	return "undefined";
}

char *bbb_getGpioStrByIndex(int index) {
	static char gpiostr[10];

	if (index >= 0 && index <= bbb_table_size) {
		memset(gpiostr, 0, sizeof(gpiostr));
		snprintf(gpiostr, sizeof(gpiostr), "%d", bbb_pinout_table[index].gpio);

		return gpiostr;
	}

	return "undefined";
}

char *bbb_getTypeStrByIndex(int index) {
	static char typestr[56];

	if (index >= 0 && index <= bbb_table_size) {
		memset(typestr, 0, sizeof(typestr));
		snprintf(typestr, sizeof(typestr), "%s %s %s", bbb_pinType2str(bbb_pinout_table[index].type1), bbb_pinType2str(bbb_pinout_table[index].type2), bbb_pinType2str(bbb_pinout_table[index].type3));

		return typestr;
	}

	return "undefined";
}

char *bbb_getSignalNameByIndex(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].sig;
	}

	return "undefined";
}

char *bbb_getNameByIndex(int index) {
	if (index >= 0 && index <= bbb_table_size) {
		return bbb_pinout_table[index].name;
	}

	return "undefined";
}

//	Types

char *bbb_pinType2str(bbb_pin_type type) {
	char *ret = "undefined";

	if (type >= BBB_NONE && type <= BBB_SPI) {
		ret = bbb_pin_type_str[type];
	}

	return ret;
}

bbb_pin_type bbb_str2pinType(char *str) {
	int i = 0;
	bbb_pin_type ret = BBB_NONE;

	for (i = 0; i < bbb_pin_type_str_size; i++) {
		if (strcmp(bbb_pin_type_str[i], str) == 0) {
			ret = i;
			break;
		}
	}

	return ret;
}

//	ADC

int bbb_enableADC() {
	int fd = -1;
	static int ret = -1;
	char buffer[1024];
	char *ptr = "cape-bone-iio";
	int size = strlen(ptr);
	char *wpath = NULL;

	if ((wpath = bbb_wildCardPath(BBB_SLOTS)) != NULL) {
		if ((fd = open(wpath, O_RDWR | O_APPEND)) < 0) {
			return -1;
		}

		memset(buffer, 0, sizeof(buffer));

		if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
			return -1;
		}

		if (strncmp(buffer, ptr, size) != 0) {
			LOG("Enabling ADC\n");

			if (write(fd, ptr, size) != size) {
				return -1;
			}
		}

		return 0;
	}

	return -1;
}

int bbb_getADC(int id) {
	int fd = -1;
	static int ret = -1;
	char buffer[128];
	char path[128];
	char *wpath = NULL;

	memset(path, 0, sizeof(path));
	snprintf(path, sizeof(path), "%s/AIN%d", BBB_HELPER, id);

	if ((wpath = bbb_wildCardPath(path)) != NULL) {
		if ((fd = open(wpath, O_RDONLY)) < 0) {
			return -1;
		}

		memset(buffer, 0, sizeof(buffer));

		if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
			return -1;
		}

		ret = atoi(buffer);
	}

	return ret;
}

//	I2C

int bbb_enableI2Cdevice(int bus, unsigned char address, char *module) {
	int fd = -1;
	static int ret = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (address > 0 && module != NULL) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/%d-00%.2x", BBB_I2CDEVICES, bus, address);

		if ((wpath = bbb_wildCardPath(path)) == NULL) {
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "/sys/bus/i2c/devices/i2c-%d/new_device", bus);

			if ((wpath = bbb_wildCardPath(path)) == NULL) {
				return -1;
			}

			if ((fd = open(wpath, O_WRONLY)) < 0) {
				return -1;
			}

			LOG("Enabling I2C device(%d), Address(0x%.2x), module(%s)\n", bus, address, module);

			memset(ptr, 0, sizeof(ptr));
			ret = snprintf(ptr, sizeof(ptr), "%s 0x%.2x", module, address);

			if ((ret = write(fd, ptr, ret)) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		} else {
			ret = 1;
		}
	}

	return ret;
}

int bbb_disableI2Cdevice(int bus, unsigned char address) {
	int fd = -1;
	static int ret = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (address > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/%d-00%.2x", BBB_I2CDEVICES, bus, address);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "/sys/bus/i2c/devices/i2c-%d/delete_device", bus);

			if ((wpath = bbb_wildCardPath(path)) != NULL) {
				if ((fd = open(wpath, O_WRONLY)) < 0) {
					return -1;
				}

				LOG("Disabling I2C device(%d), Address(0x%.2x)\n", bus, address);

				memset(ptr, 0, sizeof(ptr));
				ret = snprintf(ptr, sizeof(ptr), "0x%.2x", address);

				if ((ret = write(fd, ptr, ret)) < 0) {
					ret = -1;
				} else {
					ret = 0;
				}

				close(fd);
			}
		}
	}

	return ret;
}

//	GPIO

int bbb_enableGpio(int gpio) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (gpio > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) == NULL) {
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s/export", BBB_GPIOP);

			if ((wpath = bbb_wildCardPath(path)) != NULL) {
				if ((fd = open(wpath, O_WRONLY)) < 0) {
					return -1;
				}

				LOG("Enabling GPIO(%d)\n", gpio);

				memset(ptr, 0, sizeof(ptr));
				ret = snprintf(ptr, sizeof(ptr), "%d", gpio);

				if (write(fd, ptr, ret) < 0) {
					ret = -1;
				} else {
					ret = 0;
				}

				close(fd);
			}
		} else {
			return 0;
		}
	}

	return ret;
}

int bbb_disableGpio(int gpio) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (gpio > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			memset(path, 0, sizeof(path));
			snprintf(path, sizeof(path), "%s/unexport", BBB_GPIOP);

			if ((wpath = bbb_wildCardPath(path)) != NULL) {
				if ((fd = open(wpath, O_WRONLY)) < 0) {
					return -1;
				}

				LOG("Disabling GPIO(%d)\n", gpio);

				memset(ptr, 0, sizeof(ptr));
				ret = snprintf(ptr, sizeof(ptr), "%d", gpio);

				if (write(fd, ptr, ret) < 0) {
					ret = -1;
				} else {
					ret = 0;
				}

				close(fd);
			}
		}
	}

	return ret;
}

int bbb_setGpioDirection(int gpio, int direction) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char *ptr = "IN";
	char *wpath = NULL;

	if (gpio > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d/direction", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_WRONLY)) < 0) {
				return -1;
			}

			if (direction > 0) {
				ret = 3;
				ptr = "out";
			} else {
				ret = 2;
				ptr = "in";
			}

			if (write(fd, ptr, ret) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_getGpioDirection(int gpio) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (gpio > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d/direction", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_RDONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));

			if ((ret = read(fd, ptr, sizeof(ptr))) <= 0) {
				close(fd);
				return -1;
			}

			if (ret >= 2 && strncmp(ptr, "in", 2) == 0) {
				ret = 0;
			} else {
				if (ret >= 3 && strncmp(ptr, "out", 3) == 0) {
					ret = 1;
				} else {
					ret = -1;
				}
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_setGpioValue(int gpio, int value) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (gpio > 0 && value >= 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d/value", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_WRONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));
			ret = snprintf(ptr, sizeof(ptr), "%d", (value > 0) ? 1 : 0);

			if (write(fd, ptr, ret) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_getGpioValue(int gpio) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (gpio > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/gpio%d/value", BBB_GPIOP, gpio);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_RDONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));

			if ((ret = read(fd, ptr, sizeof(ptr))) <= 0) {
				close(fd);
				return -1;
			}

			ret = atoi(ptr);

			close(fd);
		}
	}

	return ret;
}

//	PWM

int bbb_enablePwm(int header, int pin) {
	int fd = -1;
	int ret = -1;
	char buffer[4096];
	char ptr[128];
	char *pwm = "am33xx_pwm";
	int size = strlen(pwm);
	char *wpath = NULL;

	if ((wpath = bbb_wildCardPath(BBB_SLOTS)) != NULL) {
		if ((fd = open(wpath, O_RDWR | O_APPEND)) < 0) {
			return -1;
		}

		memset(buffer, 0, sizeof(buffer));

		if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
			close(fd);
			return -1;
		}

		if (strncmp(buffer, pwm, size) != 0) {
			LOG("Enabling PWM\n");

			if (write(fd, pwm, size) != size) {
				close(fd);
				return -1;
			}
		}

		memset(ptr, 0, sizeof(ptr));
		ret = snprintf(ptr, sizeof(ptr), "bone_pwm_P%d_%d", header, pin);

		if (strncmp(buffer, ptr, ret) != 0) {
			LOG("Enabling PWM P%d_%d\n", header, pin);

			if (write(fd, ptr, ret) != ret) {
				close(fd);
				return -1;
			}
		}

		close(fd);

		return 0;
	}

	return -1;
}

int bbb_setPwmPeriod(int header, int pin, int time) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/period", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
                        if ((fd = open(wpath, O_WRONLY)) < 0) {
                                return -1;
                        }

			if (bbb_setPwmDuty(header, pin, 0) < 0) {
				close(fd);
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));
			ret = snprintf(ptr, sizeof(ptr), "%d", time);

			if (write(fd, ptr, ret) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_setPwmDuty(int header, int pin, int time) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/duty", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_WRONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));
			ret = snprintf(ptr, sizeof(ptr), "%d", time);

			if (write(fd, ptr, ret) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_setPwmHz(int header, int pin, char *hz) {
	int time = 0;
	int i, size = 0;
	char ptr[128];
	char c;
	int f, z = -1, h = -1, k = -1, m = -1, fl = -1;

	if (header > 0 && pin > 0 && hz != NULL) {
		i = size = strlen(hz);

		if (size <= 0) {
			return -1;
		}

		for (i = (size - 1), f = -1, c = hz[i]; i >= 0; i--, c = hz[i], f = -1) {
			switch (c) {
				case 'z':
				case 'Z':
					f = z = i;
					break;
				case 'h':
				case 'H':
					f = h = i;
					break;
				case 'k':
				case 'K':
					f = k = i;
					break;
				case 'm':
				case 'M':
					f = m = i;
					break;
				case '.':
					f = fl = i;
				default:
					break;
			}

			if (f < 0 && (c < 48 || c > 57)) {
				return -1;
			}
		}

		time = Hz;

		if (z >= 0) {
			size--;
		}

		if (h >= 0) {
			size--;
		}

		if (k >= 0) {
			time = kHz;
			size--;
		}

		if (m >= 0) {
			time = MHz;
			size--;
		}

		if (size > 0) {
			memset(ptr, 0, sizeof(ptr));
			memcpy(ptr, hz, size);

			if (fl > 0) {
				time = (int) time / atof(ptr);
			} else {
				time = (int) time / atoi(ptr);
			}
		}

		return bbb_setPwmPeriod(header, pin, time);
	}

	return -1;
}

int bbb_setPwmPercent(int header, int pin, int duty) {
	int time = bbb_getPwmPeriod(header, pin);

	if (header > 0 && pin > 0 && duty >= 0 && duty <= 100) {
		return bbb_setPwmDuty(header, pin, ((time / 100) * duty));
	}

	return -1;
}

int bbb_setPwmPolarity(int header, int pin, int polarity) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/polarity", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_WRONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));
			ret = snprintf(ptr, sizeof(ptr), "%d", polarity);

			if (write(fd, ptr, ret) < 0) {
				ret = -1;
			} else {
				ret = 0;
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_getPwmPeriod(int header, int pin) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/period", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_RDONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));

			if (read(fd, ptr, sizeof(ptr)) <= 0) {
				ret = -1;
			} else {
				ret = atoi(ptr);
			}

			close(fd);
		}
	}

	return ret;
}

int bbb_getPwmDuty(int header, int pin) {
	static int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/duty", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_RDONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));

			if (read(fd, ptr, sizeof(ptr)) <= 0) {
				ret = -1;
			} else {
				ret = atoi(ptr);
			}

			close(fd);
		}
	}

	return ret;
}

char *bbb_getPwmHz(int header, int pin) {
	static char ptr[128];

	if (header > 0 && pin > 0) {
		int time = bbb_getPwmPeriod(header, pin);

		memset(ptr, 0, sizeof(ptr));
		snprintf(ptr, sizeof(ptr), "%dHz", (int) (Hz / time));

		return ptr;
	}

	return "undefined";
}

int bbb_getPwmPercent(int header, int pin) {
	static int ret = -1;

	if (header > 0 && pin > 0) {
		int period = bbb_getPwmPeriod(header, pin);
		int duty = bbb_getPwmDuty(header, pin);

		ret = ((duty * 100) / period);
	}

	return ret;
}

int bbb_getPwmPolarity(int header, int pin) {
	int ret = -1;
	int fd = -1;
	char path[128];
	char ptr[128];
	char *wpath = NULL;

	if (header > 0 && pin > 0) {
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/pwm_test_P%d_%d.*/polarity", BBB_OCP2, header, pin);

		if ((wpath = bbb_wildCardPath(path)) != NULL) {
			if ((fd = open(wpath, O_RDONLY)) < 0) {
				return -1;
			}

			memset(ptr, 0, sizeof(ptr));

			if (read(fd, ptr, sizeof(ptr)) <= 0) {
				ret = -1;
			} else {
				ret = atoi(ptr);
			}

			close(fd);
		}
	}

	return ret;
}

//	Serial

int bbb_enableSerial(int index) {
        int fd = -1;
        static int ret = -1;
        char buffer[1024];
        char path[128];
        char *wpath = NULL;

        if ((wpath = bbb_wildCardPath(BBB_SLOTS)) != NULL) {
                if ((fd = open(wpath, O_RDWR | O_APPEND)) < 0) {
                        return -1;
                }

                memset(buffer, 0, sizeof(buffer));

                if ((ret = read(fd, buffer, sizeof(buffer))) < 0) {
                        return -1;
                }

		memset(path, 0, sizeof(path));
		ret = snprintf(path, sizeof(path), "BB-UART%d", index);

                if (strncmp(buffer, path, ret) != 0) {
                        LOG("Enabling Serial %s\n", path);

                        if (write(fd, path, ret) != ret) {
                                return -1;
                        }
                }

                return 0;
        }

	return -1;
}

//	Debug

void bbb_showByIndex(int index) {
	LOG("%s %s %s %s %s\n", bbb_getPinStrByIndex(index), bbb_getGpioStrByIndex(index), bbb_getTypeStrByIndex(index), bbb_getSignalNameByIndex(index), bbb_getNameByIndex(index));
}

void bbb_showPin(char *str) {
	bbb_showByIndex(bbb_getIndexByStr(str));
}

void bbb_showAll() {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		bbb_showByIndex(i);
	}
}

void bbb_showByType(bbb_pin_type type) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		if (bbb_pinout_table[i].type1 == type || bbb_pinout_table[i].type2 == type || bbb_pinout_table[i].type2 == type) {
			bbb_showByIndex(i);
		}
	}
}

void bbb_showByTypeOnly(bbb_pin_type type) {
	int i;

	for (i = 0; i < bbb_table_size; i++) {
		int found = 0;

		if (bbb_pinout_table[i].type1 == type || bbb_pinout_table[i].type2 == type || bbb_pinout_table[i].type2 == type) {
			found = 1;
		}

		if (type != BBB_ANALOG) {
			if (bbb_pinout_table[i].type1 == type || bbb_pinout_table[i].type1 == BBB_GPIO) {
				if (bbb_pinout_table[i].type2 == type || bbb_pinout_table[i].type2 == BBB_GPIO) {
					if (bbb_pinout_table[i].type3 == type || bbb_pinout_table[i].type3 == BBB_GPIO) {
						if (found) {
							bbb_showByIndex(i);
						}
					}
				}
			}
		}

		else {
			if (found) {
				bbb_showByIndex(i);
			}
		}
	}
}
