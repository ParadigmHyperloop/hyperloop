CFLAGS+=-Wall -Werror
LDLIBS+= -lpthread -lprussdrv

all: ADCCollector.bin ADCCollector

clean:
	rm -f ADCCollector *.o *.bin

ADCCollector.bin: ADCCollector.p
	pasm -b $^

ADCCollector: ADCCollector.o