all:
	cd core && $(MAKE) clean main

run: all
	core/main

run-ods:
	ods/server.py

test: all
	./test

clean:
	rm -f *.o *.csv *.log
