import beaglebone_pru_adc as adc
import time
import mmap
import struct

with open("/dev/uio0", 'r+b') as f1:
	pru0_mem = mmap.mmap(f1.fileno(), 0x200, offset=0)

capture = adc.Capture()

capture.start()

time.sleep(1.0)
#struct.pack_into('L', pru0_mem, 0x0008, 1) # exit flag

capture.stop()

for off in range(0, 0x81, 4):
	read_back = struct.unpack("L", pru0_mem[off:off+4])[0]
	print hex(off), ':', hex(read_back)

print 'timer:', capture.timer
print 'ema_pow:', capture.ema_pow
print 'values:', capture.values
print 'enc pins:', capture.encoder0_pin, capture.encoder1_pin
print 'encoder0_values:', capture.encoder0_values
print 'encoder1_values:', capture.encoder1_values

print 'encoder0_threshold:', capture.encoder0_threshold
print 'encoder1_threshold:', capture.encoder1_threshold

capture.wait()

pru0_mem.close()
capture.close()
