#This script reads two ADC ports P9_39 & P9_40 which are connected to 2 muxes. 
#The script changes the selection signal on the mux and reads all 16 signals on each mux
#It can read a total of 32 signals
#Change numReadCycle to set how many reads on each mux selection signal
#----------------------------------------------------------------------------------------------------------------

import beaglebone_pru_adc as adc
import Adafruit_BBIO.GPIO as GPIO
import os

#path = "/tmp/my_program.fifo" #Shared pipe path
#os.mkfifo(path) #Setup shared pipe

#fifo = open(path, "w") #open pipe

#setup mux1 4 bit selction signal
GPIO.setup("P8_14", GPIO.OUT) 
GPIO.setup("P8_15", GPIO.OUT)
GPIO.setup("P8_16", GPIO.OUT)
GPIO.setup("P8_17", GPIO.OUT)

#setup mux2 4 bit selection signal
GPIO.setup("P8_18", GPIO.OUT) 
GPIO.setup("P8_19", GPIO.OUT)
GPIO.setup("P8_20", GPIO.OUT)
GPIO.setup("P8_21", GPIO.OUT)

#Setup adc
capture = adc.Capture()
capture.encoder0_pin = 0 # AIN0, aka P9_39
capture.encoder1_pin = 2 # AIN2, aka P9_37
capture.encoder0_threshold = 3000 # you will want to adjust this
capture.encoder1_thredhold = 3000 # and this...
capture.encoder0_delay = 100 # prevents "ringing", adjust if needed
capture.encoder1_delay = 100 # ... same

#first input mux1
def mux1(val): 
	if val == 0:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 0)
	elif val == 1:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 0)
	elif val == 2:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 0)
	elif val == 3:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 0)
	elif val == 4:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 0)
	elif val == 5:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 0)
	elif val == 6:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 0)
	elif val == 7:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 0)
	elif val == 8:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 1)
	elif val == 9:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 1)
	elif val == 10:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 1)
	elif val == 11:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 0)
		GPIO.output("P8_17", 1)
	elif val == 12:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 1)
	elif val == 13:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 0)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 1)
	elif val == 14:
		GPIO.output("P8_14", 0)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 1)
	elif val == 15:
		GPIO.output("P8_14", 1)
		GPIO.output("P8_15", 1)
		GPIO.output("P8_16", 1)
		GPIO.output("P8_17", 1)

#input mux2
def mux2(val): 
	if val == 0:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 0)
	elif val == 1:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 0)
	elif val == 2:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 0)
	elif val == 3:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 0)
	elif val == 4:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 0)
	elif val == 5:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 0)
	elif val == 6:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 0)
	elif val == 7:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 0)
	elif val == 8:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 1)
	elif val == 9:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 1)
	elif val == 10:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 1)
	elif val == 11:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 0)
		GPIO.output("P8_21", 1)
	elif val == 12:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 1)
	elif val == 13:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 0)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 1)
	elif val == 14:
		GPIO.output("P8_18", 0)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 1)
	elif val == 15:
		GPIO.output("P8_18", 1)
		GPIO.output("P8_19", 1)
		GPIO.output("P8_20", 1)
		GPIO.output("P8_21", 1)
	
capture.start()
numReadCycle = 10
#Loop over all mux inputs
for select in xrange(16):
	mux1(select)
	mux2(select)
	for _ in range(numReadCycle):
		print("Mux select = " + str(select), "Time capture:" ,capture.timer, "Mux 1:" ,capture.encoder0_values, "Mux2:", capture.encoder1_values)
	#print(output)
	#fifo.write(output)
capture.stop()
capture.wait()
capture.close()
#fifo.close()