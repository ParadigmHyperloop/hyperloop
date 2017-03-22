# Configure one common build root
export BLDROOT = ${CURDIR}/BUILD
export OBJROOT = $(BLDROOT)/obj
export DSTROOT = $(BLDROOT)/dst
export HDRROOT = $(BLDROOT)/hdr
export FMTROOT = $(BLDROOT)/fmt

# Delegate Build to Sub-Makefiles
all: install_headers
	@$(MAKE) -C libbbb
	@$(MAKE) -C libimu
	@$(MAKE) -C libhw
	@$(MAKE) -C liblog
	@$(MAKE) -C core

install:
	@$(MAKE) -C libbbb install
	@$(MAKE) -C libimu install
	@$(MAKE) -C libhw install
	@$(MAKE) -C liblog install
	@$(MAKE) -C core install

install_headers:
	@$(MAKE) -C libbbb install_headers
	@$(MAKE) -C libimu install_headers
	@$(MAKE) -C libhw install_headers
	@$(MAKE) -C liblog install_headers
	@$(MAKE) -C core install_headers

clean:
	@$(MAKE) -C libbbb clean
	@$(MAKE) -C libimu clean
	@$(MAKE) -C libhw clean
	@$(MAKE) -C liblog clean
	@$(MAKE) -C core clean
