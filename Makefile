
CORE_MAKE= cd core && $(MAKE)

all:
	$(CORE_MAKE) all

run: run-core

run-core:
	$(CORE_MAKE) run

install:
	$(CORE_MAKE) install

test: all
	./test

style:
	$(CORE_MAKE) style
clean:
	$(CORE_MAKE) clean
	rm -f *.o *.csv *.log
