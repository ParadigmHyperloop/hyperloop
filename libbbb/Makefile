include ../config.mk

RUN_ARGS := -i -

$(EXECUTABLE): $(OBJECTS)
		ar rcs $@ $^

$(OBJROOT)/%.o : $(SRCROOT)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: install_headers $(EXECUTABLE)

install:
	mkdir -p $(DSTROOT)/usr/local/bin
	install -m 755 $(EXECUTABLE) $(DSTROOT)/usr/local/lib
	zip -r $(DSTROOT).zip $(DSTROOT)

run: clean all
	$(EXECUTABLE) $(RUN_ARGS)

clean:
	rm -rf $(BLDROOT)

install_headers:
	@echo
	@echo $(SOURCES)
	@echo $(OBJECTS)
	@echo $(OBJROOT)
	@echo
	mkdir -p $(HDRROOT)$(HDR_INSTALL)
	install $(HEADERS) $(HDRROOT)$(HDR_INSTALL)
