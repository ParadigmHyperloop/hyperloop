EXECUTABLE=$(OBJROOT)/core

include ../config.mk

RUN_ARGS := -i -
LDFLAGS += -L$(OBJROOT) -limu -llog -lhw -lbbb

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
	mkdir -p $(HDRROOT)$(HDR_INSTALL)
	install $(HEADERS) $(HDRROOT)$(HDR_INSTALL)
