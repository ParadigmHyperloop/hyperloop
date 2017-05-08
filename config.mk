define BANNER
######################################################################################################################
# .______      ___      .______          ___       _______   __    _______ .___  ___.                                #
# |   _  \    /   \     |   _  \        /   \     |       \ |  |  /  _____||   \/   |                                #
# |  |_)  |  /  ^  \    |  |_)  |      /  ^  \    |  .--.  ||  | |  |  __  |  \  /  |                                #
# |   ___/  /  /_\  \   |      /      /  /_\  \   |  |  |  ||  | |  | |_ | |  |\/|  |                                #
# |  |     /  _____  \  |  |\  \----./  _____  \  |  '--'  ||  | |  |__| | |  |  |  |                                #
# | _|    /__/     \__\ | _| `._____/__/     \__\ |_______/ |__|  \______| |__|  |__|                                #
#                                                                                                                    #
# .______    __    __   __   __       _______          _______.____    ____  _______.___________. _______ .___  ___. #
# |   _  \  |  |  |  | |  | |  |     |       \        /       |\   \  /   / /       |           ||   ____||   \/   | #
# |  |_)  | |  |  |  | |  | |  |     |  .--.  |      |   (----` \   \/   / |   (----`---|  |----`|  |__   |  \  /  | #
# |   _  <  |  |  |  | |  | |  |     |  |  |  |       \   \      \_    _/   \   \       |  |     |   __|  |  |\/|  | #
# |  |_)  | |  `--'  | |  | |  `----.|  '--'  |   .----)   |       |  | .----)   |      |  |     |  |____ |  |  |  | #
# |______/   \______/  |__| |_______||_______/    |_______/        |__| |_______/       |__|     |_______||__|  |__| #
#                                                                                                                    #
######################################################################################################################
endef

# Common Utilities
CC := $(shell which clang)
PASM := $(shell command -v pasm 2> /dev/null)
WARNING_BANNER := "WARNING WARNING WARNING"
AR := $(shell which ar)

# Root Setup
BLDROOT ?= ${CURDIR}/BUILD
OBJROOT ?= $(BLDROOT)/obj
DSTROOT ?= $(BLDROOT)/dst
HDRROOT ?= $(BLDROOT)/hdr
FMTROOT ?= $(BLDROOT)/fmt
SRCROOT ?= ${CURDIR}

HDR_INSTALL := /usr/local/include

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

#ifeq ($(UNAME_S),Darwin)
#	CFLAGS += -g
#	CFLAGS += -Wnon-modular-include-in-framework-module -Werror=non-modular-include-in-framework-module -Wno-trigraphs -fpascal-strings -fno-common -Werror -Werror=incompatible-pointer-types -Werror-implicit-function-declaration -Wno-missing-field-initializers -Wmissing-prototypes -Werror=return-type -Wdocumentation -Wunreachable-code -Werror=deprecated-objc-isa-usage -Werror=objc-root-class -Wmissing-braces -Wparentheses -Wswitch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wconditional-uninitialized -Wno-unknown-pragmas -Wshadow -Wfour-char-constants -Wconversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wassign-enum -Wsign-compare -Wshorten-64-to-32 -Wpointer-sign -Wnewline-eof -Wno-sign-conversion -Wno-gnu-zero-variadic-macro-arguments
#endif

ifeq ($(UNAME_S),Linux)
	 LDFLAGS += -pthread -lrt
endif

ifneq (,$(findstring bone,$(UNAME_R)))
	CFLAGS += -DBBB -DHAS_KILL_SWITCH
endif

# Compiler and Linker Flags
CFLAGS += -Os -pthread -std=gnu99 -I$(HDRROOT)$(HDR_INSTALL)

# Main Warning Flag Set
CFLAGS += -Wall -Werror -Wno-unknown-pragmas

# Helpers
MAKE_LIB = $(AR) rcs

GIT_SHA1 = $(shell git rev-parse --verify HEAD)
GIT_SHA1_SHORT = $(shell git rev-parse --verify --short HEAD)
GIT_BRANCH = $(shell git rev-parse --verify --abbrev-ref HEAD)
CFLAGS += -DPD_GIT_SHA1="$(GIT_SHA1)" -DPD_GIT_SHA1_SHORT="$(GIT_SHA1_SHORT)" -DPD_GIT_BRANCH="$(GIT_BRANCH)"
