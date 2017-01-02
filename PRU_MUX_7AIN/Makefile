CFLAGS+=-Wall -Werror
LDLIBS+= -lpthread -lprussdrv

all: ADCCollector.bin ADCCollector

clean:
	rm -f ADCCollector *.o *.bin
	find . -type f | xargs -n 5 touch
	rm -rf $(OBJS)
ADCCollector.bin: ADCCollector.p
	pasm -b $^

ADCCollector: ADCCollector.o
