EXECUTABLE=$(OBJROOT)/imu_config

include ../config.mk

LDFLAGS += -L$(OBJROOT) -limu

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJROOT)/%.o : $(SRCROOT)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: install_headers $(EXECUTABLE)


install:
	mkdir -p $(DSTROOT)/usr/local/bin
	install -m 755 $(EXECUTABLE) $(DSTROOT)/usr/local/bin

run: clean all
	$(EXECUTABLE) $(RUN_ARGS)

clean:
	rm -rf $(BLDROOT)

install_headers:
