encodeMux.py reads two ADC ports P9_39 & P9_40 which are connected to 2 muxes. 
#The script changes the selection signal on the mux and reads all 16 signals on each mux
#It can read a total of 32 signals
#Change numReadCycle to set how many reads on each mux selection signal

To run the encodeMux.py script on the beagle bone the following packages need to be installed:
	adafruit-beaglebone-io-python-master
	beaglebone_pru_adc-master
#----------------------------------------------------------------------------------------------------------------