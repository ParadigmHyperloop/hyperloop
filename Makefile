# Configure one common build root
export BLDROOT = ${CURDIR}/BUILD
export OBJROOT = $(BLDROOT)/obj
export DSTROOT = $(BLDROOT)/dst
export HDRROOT = $(BLDROOT)/hdr
export FMTROOT = $(BLDROOT)/fmt

# Delegate Build to Sub-Makefiles
all: install_headers
	@$(MAKE) -j -C libbbb
	@$(MAKE) -j -C libimu
	@$(MAKE) -j -C libhw
	@$(MAKE) -j -C liblog
	@$(MAKE) -j -C core

install:
	@$(MAKE) -j -C libbbb install
	@$(MAKE) -j -C libimu install
	@$(MAKE) -j -C libhw install
	@$(MAKE) -j -C liblog install
	@$(MAKE) -j -C core install

install_headers:
	@$(MAKE) -j -C libbbb install_headers
	@$(MAKE) -j -C libimu install_headers
	@$(MAKE) -j -C libhw install_headers
	@$(MAKE) -j -C liblog install_headers
	@$(MAKE) -j -C core install_headers

clean:
	@$(MAKE) -j -C libbbb clean
	@$(MAKE) -j -C libimu clean
	@$(MAKE) -j -C libhw clean
	@$(MAKE) -j -C liblog clean
	@$(MAKE) -j -C core clean
