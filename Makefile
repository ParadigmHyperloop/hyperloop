default:
	cd core && $(MAKE) clean all

clean:
	rm -f *.o *.csv *.log
