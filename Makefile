
CORE_MAKE= cd core && $(MAKE)
ODS_MAKE= cd ods && $(MAKE)

all:
	$(CORE_MAKE) all
	$(ODS_MAKE) all

run: run-core

run-core:
	$(CORE_MAKE) run

run-ods:
	$(ODS_MAKE) run

setup-ods:
	$(ODS_MAKE) setup-mac

test: all
	./test

clean:
	$(CORE_MAKE) clean
	$(ODS_MAKE) clean
	rm -f *.o *.csv *.log
