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

# Main Warning Flag Set: DO NOT CHANGE THESE!
CFLAGS += -Wall -Werror -pedantic

# Special Warnings
CFLAGS += -Wnon-modular-include-in-framework-module -Werror=non-modular-include-in-framework-module -Wno-trigraphs -fpascal-strings -O0 -fno-common -Werror -Werror=incompatible-pointer-types -Werror-implicit-function-declaration -Wno-missing-field-initializers -Wmissing-prototypes -Werror=return-type -Wdocumentation -Wunreachable-code -Werror=deprecated-objc-isa-usage -Werror=objc-root-class -Wmissing-braces -Wparentheses -Wswitch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wconditional-uninitialized -Wno-unknown-pragmas -Wshadow -Wfour-char-constants -Wconversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wassign-enum -Wsign-compare -Wshorten-64-to-32 -Wpointer-sign -Wnewline-eof -Wno-sign-conversion -Wno-gnu-zero-variadic-macro-arguments

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
