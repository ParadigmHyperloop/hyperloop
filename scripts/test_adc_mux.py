#!/usr/bin/env python
import time
SELECT = [45, 44, 49, 117]  # , 125]

while True:
    for i in range(16):
        for j, p in enumerate(SELECT):
            with open("/sys/class/gpio/gpio{}/value".format(p), 'w') as f:
                f.write('1' if (i >> j) & 0x1 else '0')
        val = 0.0
        with open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw") as f:
            val = float(f.read())

        print "{}\t{}".format(i, val)
        time.sleep(0.1)
