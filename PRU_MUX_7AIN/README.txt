//Written by Upendra Naidoo
-This program reads an0-an7 input on the beaglebone
- GPIO selection 
	S0 - P8_11
	S1 - P8_12
	S2 - P9_25
	S3 - P9_27
-After all 7 analog inputs are read the mux input is switched to the next signal (0 - 15)

To set pru gpio pins:
   -dtc -O dtb -I dts -o /lib/firmware/PRU-GPIO-EXAMPLE-00A0.dtbo -b 0 -@ PRU-GPIO-EXAMPLE-00A0.dts  
   -reboot 
   -echo PRU-GPIO-EXAMPLE > /sys/devices/bone_capemgr.?/slots  
   -cat /sys/devices/bone_capemgr.?/slots
   
To compile and run:
   -bash init
   -make 
   -./ADCCollector <number_of_samples>