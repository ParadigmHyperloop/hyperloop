default:
	cd core && $(MAKE) clean main

clean:
	rm -f *.o *.csv *.log
