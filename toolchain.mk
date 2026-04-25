#
# toolchain.mk -- SDCC 2.9.0 toolchain selection
#
# Optional local override file:
#   config.local.mk
#
# Supported knobs:
#   SDCC290_BIN_DIR = /absolute/path/to/bin
#   SDCC_OPT        = --opt-code-speed | --opt-code-size
#

-include $(SDK_DIR)config.local.mk

SDCC290_BIN_DIR ?=
SDCC290_WRAPPER ?= $(shell command -v sdcc290 2>/dev/null)

ifeq ($(strip $(SDCC290_BIN_DIR)),)
ifneq ($(strip $(SDCC290_WRAPPER)),)
SDCC ?= $(SDCC290_WRAPPER)
else
SDCC ?= sdcc290
endif

SDCC290_DEFAULT_ROOT := $(patsubst %/,%,$(dir $(abspath $(SDCC290_WRAPPER))))

ifeq ($(wildcard $(SDCC290_DEFAULT_ROOT)/opt/sdcc-2.9.0/bin/sdcpp-2.9.0),$(SDCC290_DEFAULT_ROOT)/opt/sdcc-2.9.0/bin/sdcpp-2.9.0)
SDCPP ?= $(SDCC290_DEFAULT_ROOT)/opt/sdcc-2.9.0/bin/sdcpp-2.9.0
else
SDCPP ?= sdcpp-2.9.0
endif

SDASZ80 ?= sdasz80
SDAR    ?= sdar
SDLDZ80 ?= sdldz80
else
SDCC290_BIN_DIR := $(patsubst %/,%,$(abspath $(SDCC290_BIN_DIR)))

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcc-2.9.0),$(SDCC290_BIN_DIR)/sdcc-2.9.0)
SDCC ?= $(SDCC290_BIN_DIR)/sdcc-2.9.0
else ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcc290),$(SDCC290_BIN_DIR)/sdcc290)
SDCC ?= $(SDCC290_BIN_DIR)/sdcc290
else
SDCC ?= $(SDCC290_BIN_DIR)/sdcc
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcpp-2.9.0),$(SDCC290_BIN_DIR)/sdcpp-2.9.0)
SDCPP ?= $(SDCC290_BIN_DIR)/sdcpp-2.9.0
else
SDCPP ?= sdcpp-2.9.0
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdasz80),$(SDCC290_BIN_DIR)/sdasz80)
SDASZ80 ?= $(SDCC290_BIN_DIR)/sdasz80
else
SDASZ80 ?= sdasz80
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdar),$(SDCC290_BIN_DIR)/sdar)
SDAR ?= $(SDCC290_BIN_DIR)/sdar
else
SDAR ?= sdar
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdldz80),$(SDCC290_BIN_DIR)/sdldz80)
SDLDZ80 ?= $(SDCC290_BIN_DIR)/sdldz80
else
SDLDZ80 ?= sdldz80
endif
endif

PYTHON       ?= python3
SDCC_TARGET  ?= -mz80
SDCC_OPT     ?= --opt-code-speed
SDCC_CFLAGS  ?= $(SDCC_TARGET) $(SDCC_OPT)
SDCPPFLAGS   ?=
SDASZ_FLAGS  ?= -plosgff
