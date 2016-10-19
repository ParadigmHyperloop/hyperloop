import beaglebone_pru_adc as adc

capture = adc.Capture()
capture.encoder0_pin = 0 # AIN0, aka P9_39
capture.encoder1_pin = 2 # AIN2, aka P9_37
capture.encoder0_threshold = 3000 # you will want to adjust this
capture.encoder1_thredhold = 3000 # and this...
capture.encoder0_delay = 100 # prevents "ringing", adjust if needed
capture.encoder1_delay = 100 # ... same

capture.start()

for _ in range(1000):
    print capture.timer, capture.encoder0_values, capture.encoder1_values

capture.stop()
capture.wait()
capture.close()