# Common Utilities
CC := gcc
PASM := $(shell command -v pasm 2> /dev/null)
WARNING_BANNER := "WARNING WARNING WARNING"

# Root Setup
BLDROOT ?= ${CURDIR}/BUILD
OBJROOT ?= $(BLDROOT)/obj
DSTROOT ?= $(BLDROOT)/dst
HDRROOT ?= $(BLDROOT)/hdr
FMTROOT ?= $(BLDROOT)/fmt
SRCROOT ?= ${CURDIR}

HDR_INSTALL := /usr/local/include

# Compiler and Linker Flags
CFLAGS += -pthread -std=gnu99 -I$(HDRROOT)$(HDR_INSTALL)

CFLAGS += -Wall -Werror

# Default Files, Sources, Objects, and Headers
FILES := $(wildcard ${CURDIR}/**/*.c ${CURDIR}/**/*.h ${CURDIR}/*.c ${CURDIR}/*.h)
SOURCES := $(wildcard ${CURDIR}/*.c ${CURDIR}/**/*.c)
HEADERS := $(wildcard ${CURDIR}/*.h ${CURDIR}/**/*.h)
OBJECTS := $(patsubst ${CURDIR}/%.c, $(OBJROOT)/%.o, $(SOURCES))
EXECUTABLE ?= $(OBJROOT)/$(notdir ${CURDIR}).a

# Platform Specific Tweaks
ifeq ($(OS),Windows_NT)
	@echo $(WARNING_BANNER)
	@echo "You are compiling on windows, this is an unsupported environment"
	@echo $(WARNING_BANNER)
	$(error Using Windows... Please install Linux instead)
endif

UNAME_R := $(shell uname -r)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CFLAGS += -g
endif

ifeq ($(UNAME_S),Linux)
	 LDFLAGS += -pthread
endif

# style:
# 	mkdir -p $(FMTROOT)
# 	for f in $(wildcard **/*.c **/*.h *.c *.h); do \
# 		mkdir -p $$(dirname $(FMTROOT)/$$f); \
# 		clang-format -style=llvm $$f > $(FMTROOT)/$$f; \
# 		(diff $(FMTROOT)/$$f $$f && echo "[OK] $$f") || echo "[WARNING] Bad Style in $$f"; \
# 	done
#
# format:
# 	for f in $(wildcard **/*.c **/*.h *.c *.h); do \
# 		echo "[Formating]: $$f"; \
# 		clang-format -style=llvm -i $$f; \
# 	done
