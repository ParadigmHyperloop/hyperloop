#!/usr/bin/env python
import time
import sys
import os

PACKET = "\xFE\x81\xFF\x55\x37\xA9\x6A\x6E\x38\x58\x6C\x1F\xB7\x5B\xF8\x62" \
         "\xBF\x80\x3E\x78\xBB\x65\x0D\x28\x3B\x0A\x37\xAC\x77\x3D\x00\x28" \
         "\x4B\xFA\x34\xD8"

if len(sys.argv) != 2:
    print("Usage: {} <path>".format(sys.argv[0]))
    sys.exit(1)

path = sys.argv[1]

if not os.path.exists(path):
    os.mkfifo(path)

with open(path, 'w') as f:
    while True:
        f.write(PACKET)
        print(PACKET)
        time.sleep(.01)
