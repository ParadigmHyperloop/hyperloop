//Written by Upendra Naidoo
-This program reads an0 input on the beaglebone
- GPIO selection 
	S0 - P8_11
	S1 - P8_12
	S2 - P8_15
	S3 - P8_16
-After every read the mux input is switched to the next signal (0 - 15)
-To run enable the pru by running, "cat /sys/devices/bone_capemgr.?/slots"
-run make
-run ./ADCCollector <capturetimeSeconds>
