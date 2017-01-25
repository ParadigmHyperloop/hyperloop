#Copyright (C) 2013 Gavin Strunk
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

PASM := $(shell command -v pasm 2> /dev/null)

ifeq ($(OS),Windows_NT)
	echo "WARNING WARNING WARNING"
	echo "You are compiling on windows, this is an unsupported environment"
	echo "WARNING WARNING WARNING"
else
UNAME_R := $(shell uname -r)
UNAME_S := $(shell uname -s)

OBJROOT= ./BUILD/obj
DSTROOT= ./BUILD/dst
FMTROOT= ./BUILD/fmt
CC= gcc
SOURCES = main.c core.c pru.c imu_data.c braking.c lateral.c logging.c skates.c pod.c pod-helpers.c commands.c commander.c tests.c ring_buffer.c pod/states.c pod/states-internal.c pod/emergency.c pod/log.c

ifeq ($(UNAME_S),Linux)
	 LDFLAGS += -pthread
endif

ifneq (,$(findstring bone,$(UNAME_R)))
	LDFLAGS += -lprussdrv
	CFLAGS += "-DHAS_PRU -DBBB"
	SOURCES += libBBB.c
else
	SOURCES += shims.c
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += "-g"
endif
endif

OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
ROOTED_OBJECTS=$(patsubst %.c, $(OBJROOT)/%.o, $(SOURCES))
FILES := $(wildcard **/*.c **/*.h *.c *.h)

EXECUTABLE = $(OBJROOT)/core
CFLAGS += -pthread -std=gnu99 -DDEBUG -I../libimu/BUILD/dst/usr/local/include
LDFLAGS += -limu -L../libimu/BUILD/dst/usr/local/lib

$(EXECUTABLE): setup $(OBJECTS)
	$(CC) -o $@ $(ROOTED_OBJECTS) $(LDFLAGS)

$(OBJECTS): %.o : %.c
	mkdir -p $(shell dirname $(OBJROOT)/$<)
	$(CC) $(CFLAGS) -c $< -o $(OBJROOT)/$@


all: $(EXECUTABLE) ADCCollector.bin

setup:
	mkdir -p $(OBJROOT)
	mkdir -p $(DSTROOT)

install:
	mkdir -p $(DSTROOT)/usr/local/bin
	install -m 755 $(EXECUTABLE) $(DSTROOT)/usr/local/bin

run: clean all
	$(EXECUTABLE) -i -

style:
	mkdir -p $(FMTROOT)
	for f in $(FILES); do \
		mkdir -p $$(dirname $(FMTROOT)/$$f); \
		clang-format -style=llvm $$f > $(FMTROOT)/$$f; \
		(diff $(FMTROOT)/$$f $$f && echo "[OK] $$f") || echo "[WARNING] Bad Style in $$f"; \
	done

format:
	for f in $(FILES); do \
		echo "[Formating]: $$f"; \
		clang-format -style=llvm -i $$f; \
	done

ADCCollector.bin: ./pru/ADCCollector.p
ifdef PASM
	pasm -b $^
else
	@echo "WARNING WARNING WARNING"
	@echo "You are compiling on a machine without pasm, assuming this is a dev machine"
	@echo "WARNING WARNING WARNING"
endif

dist:
	zip -r $(DSTROOT).zip $(DSTROOT)

clean:
	rm -rf *.o pod/*.o *~ $(EXECUTABLE) BUILD
	rm -f ./pru/ADCCollector ./pru/*.o ./pru/*.bin
	find . -type f | xargs -n 5 touch
