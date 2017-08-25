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
	@$(MAKE) -j -C libsked
	@$(MAKE) -j -C core
	@$(MAKE) -j -C imu_test
	@$(MAKE) -j -C imu_config

install:
	@$(MAKE) -j -C libbbb install
	@$(MAKE) -j -C libimu install
	@$(MAKE) -j -C libhw install
	@$(MAKE) -j -C liblog install
	@$(MAKE) -j -C libsked install
	@$(MAKE) -j -C core install
	@$(MAKE) -j -C imu_test install
	@$(MAKE) -j -C imu_config install

install_headers:
	$(BANNER)
	@$(MAKE) -j -C libbbb install_headers
	@$(MAKE) -j -C libimu install_headers
	@$(MAKE) -j -C libhw install_headers
	@$(MAKE) -j -C liblog install_headers
	@$(MAKE) -j -C libsked install_headers
	@$(MAKE) -j -C core install_headers
	@$(MAKE) -j -C imu_test install_headers
	@$(MAKE) -j -C imu_config install_headers

clean:
	@$(MAKE) -j -C libbbb clean
	@$(MAKE) -j -C libimu clean
	@$(MAKE) -j -C libhw clean
	@$(MAKE) -j -C liblog clean
	@$(MAKE) -j -C libsked clean
	@$(MAKE) -j -C core clean
	@$(MAKE) -j -C imu_test clean
	@$(MAKE) -j -C imu_config clean

style:
	@echo "Not Implemented"

test:
	$(DSTROOT)$(TEST_CMD)

run:
	killall -KILL core || true
	$(DSTROOT)/usr/local/bin/core

deb:
	mkdir -p $(DEBROOT)
	mkdir -p $(DEBROOT)/DEBIAN
	cp -a $(DSTROOT) $(DEBROOT)
	assets/make_deb.sh $(DEBROOT)/DEBIAN/control
	dpkg-deb --build $(DEBROOT) $(DEBFILE)

publish:
	curl -T $(DEBFILE) -u$(BINTRAY_USER):$(BINTRAY_TOKEN) https://api.bintray.com/content/paradigmtransportation/hyperloop-core/dev/$(VERSION)/$(PROJECT)-$(VERSION).deb\;deb_distribution=jessie\;deb_component=main\;deb_architecture=i386,amd64,arm64,armv7
	curl -X POST -u$(BINTRAY_USER):$(BINTRAY_TOKEN) https://api.bintray.com/content/paradigmtransportation/hyperloop-core/dev/$(VERSION)/publish
