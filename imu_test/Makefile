EXECUTABLE=$(OBJROOT)/imu_test

include ../config.mk

override LDFLAGS += -L$(OBJROOT) -limu

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJROOT)/%.o : $(SRCROOT)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXECUTABLE)


install:
	mkdir -p $(DSTROOT)/usr/local/bin
	install -m 755 $(EXECUTABLE) $(DSTROOT)/usr/local/bin

run: clean all
	$(EXECUTABLE) $(RUN_ARGS)

clean:
	rm -rf $(BLDROOT)

install_headers:
