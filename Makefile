include config.mk

TEST_CMD := /usr/local/bin/core -t -i -

export PROJECT = hyperloop-core
export VERSION = 1.0-$(shell git rev-parse --short HEAD)
export ARCH ?= $(shell arch)

# Configure one common build root
export BLDROOT = ${CURDIR}/BUILD
export OBJROOT = $(BLDROOT)/obj
export DSTROOT = $(BLDROOT)/dst
export HDRROOT = $(BLDROOT)/hdr
export FMTROOT = $(BLDROOT)/fmt
export DEBROOT = $(BLDROOT)/deb
export DEBFILE = $(BLDROOT)/$(PROJECT).deb

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
	$(BANNER)
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

test:
	$(DSTROOT)$(TEST_CMD)

deb:
	mkdir -p $(DEBROOT)
	mkdir -p $(DEBROOT)/DEBIAN
	cp -a $(DSTROOT) $(DEBROOT)
	assets/make_deb.sh $(DEBROOT)/DEBIAN/control
	dpkg-deb --build $(DEBROOT) $(DEBFILE)

publish:
	curl -T $(DEBFILE) -u$(BINTRAY_USER):$(BINTRAY_TOKEN) https://api.bintray.com/content/paradigmtransportation/hyperloop-core/dev/$(VERSION)/$(PROJECT)-$(VERSION).deb\;deb_distribution=jessie\;deb_component=main\;deb_architecture=i386,amd64,arm64,armv7
	curl -X POST -u$(BINTRAY_USER):$(BINTRAY_TOKEN) https://api.bintray.com/content/paradigmtransportation/hyperloop-core/dev/$(VERSION)/publish
