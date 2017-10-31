#!/bin/bash

./BUILD/dst/usr/local/bin/core $@

echo "core exited $?"

if [ $? != 0 ]; then
  ../i2c_code_v4/ssr 5 0 # Turn off Release Solenoid A
  ../i2c_code_v4/ssr 7 0 # Turn off Release Solenoid B
  ../i2c_code_v4/ssr 4 4095 # Turn on Brake solenoid A
  ../i2c_code_v4/ssr 6 4095 # Turn on Brake solenoid B
fi