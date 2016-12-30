#Copyright (C) 2013 Gavin Strunk
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

ifeq ($(OS),Windows_NT)
	echo "WARNING WARNING WARNING"
	echo "You are compiling on windows, this is an unsupported environment"
	echo "WARNING WARNING WARNING"
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	 LDFLAGS += -pthread
endif
ifeq ($(UNAME_S),Darwin)

endif
endif
CC= gcc
DEPS = core.c imu_data.c braking.c lateral.c logging.c skates.c pins.c pod.c pod-helpers.c commands.c commander.c mux.c tests.c libBBBShims.c ring_buffer.c pod/states.c pod/states-internal.c pod/emergency.c pod/log.c
SOURCES= main.c $(DEPS)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
FILES := $(wildcard **/*.c **/*.h *.c *.h)

EXECUTABLE = core
CFLAGS += -pthread -std=gnu99 -DDEBUG -I../libimu/BUILD/dst/usr/local/include
LDFLAGS += -limu -L../libimu/BUILD/dst/usr/local/lib
OBJROOT= ./BUILD/obj
DSTROOT= ./BUILD/dst
FMTROOT= ./BUILD/fmt

$(EXECUTABLE):  $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXECUTABLE)

core: main.c $(DEPS)

install:
	mkdir -p $(DSTROOT)/usr/local/bin
	mkdir -p $(OBJROOT)
	install -m 755 $(OBJECTS) $(OBJROOT)
	install -m 755 $(EXECUTABLE) $(DSTROOT)/usr/local/bin

run: clean all
	./$(EXECUTABLE) -r

style:
	mkdir -p $(FMTROOT)
	for f in $(FILES); do \
		echo "[Style Check]: $$f"; \
		mkdir -p $$(dirname $(FMTROOT)/$$f); \
		clang-format -style=llvm $$f > $(FMTROOT)/$$f; \
		diff $(FMTROOT)/$$f $$f || { echo "[WARNING] Bad Style in $$f"} \
	done

format:
	for f in $(FILES); do \
		echo "[Formating]: $$f"; \
		clang-format -style=llvm -i $$f; \
	done

clean:
	rm -rf *.o pod/*.o *~ $(EXECUTABLE) BUILD
