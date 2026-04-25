#
# toolchain.mk -- SDCC 2.9.0 toolchain selection
#
# Optional local override file:
#   config.local.mk
#
# Supported knobs:
#   SDCC290_BIN_DIR = /absolute/path/to/sdcc-2.9.0/bin
#   SDCC            = /absolute/path/to/sdcc-2.9.0/bin/sdcc
#   SDCPP           = /absolute/path/to/sdcc-2.9.0/bin/sdcpp-2.9.0
#   SDASZ80         = /absolute/path/to/sdcc-2.9.0/bin/as-z80-2.9.0
#   SDAR            = /absolute/path/to/sdcc-2.9.0/bin/sdcclib-2.9.0
#   SDLDZ80         = /absolute/path/to/sdcc-2.9.0/bin/link-z80-2.9.0
#   SDCC_OPT        = --opt-code-speed | --opt-code-size
#

-include $(SDK_DIR)config.local.mk

SDCC290_BIN_DIR ?=

ifeq ($(strip $(SDCC290_BIN_DIR)),)
SDCC ?= sdcc
SDCPP ?= sdcpp
SDASZ80 ?= sdasz80
SDAR    ?= sdar
SDLDZ80 ?= sdldz80
else
SDCC290_BIN_DIR := $(patsubst %/,%,$(abspath $(SDCC290_BIN_DIR)))

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcc-2.9.0),$(SDCC290_BIN_DIR)/sdcc-2.9.0)
SDCC ?= $(SDCC290_BIN_DIR)/sdcc-2.9.0
else
SDCC ?= $(SDCC290_BIN_DIR)/sdcc
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcpp-2.9.0),$(SDCC290_BIN_DIR)/sdcpp-2.9.0)
SDCPP ?= $(SDCC290_BIN_DIR)/sdcpp-2.9.0
else ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcpp),$(SDCC290_BIN_DIR)/sdcpp)
SDCPP ?= $(SDCC290_BIN_DIR)/sdcpp
else
SDCPP ?= sdcpp
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdasz80),$(SDCC290_BIN_DIR)/sdasz80)
SDASZ80 ?= $(SDCC290_BIN_DIR)/sdasz80
else ifeq ($(wildcard $(SDCC290_BIN_DIR)/as-z80-2.9.0),$(SDCC290_BIN_DIR)/as-z80-2.9.0)
SDASZ80 ?= $(SDCC290_BIN_DIR)/as-z80-2.9.0
else
SDASZ80 ?= sdasz80
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdar),$(SDCC290_BIN_DIR)/sdar)
SDAR ?= $(SDCC290_BIN_DIR)/sdar
else ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdcclib-2.9.0),$(SDCC290_BIN_DIR)/sdcclib-2.9.0)
SDAR ?= $(SDCC290_BIN_DIR)/sdcclib-2.9.0
else
SDAR ?= sdar
endif

ifeq ($(wildcard $(SDCC290_BIN_DIR)/sdldz80),$(SDCC290_BIN_DIR)/sdldz80)
SDLDZ80 ?= $(SDCC290_BIN_DIR)/sdldz80
else ifeq ($(wildcard $(SDCC290_BIN_DIR)/link-z80-2.9.0),$(SDCC290_BIN_DIR)/link-z80-2.9.0)
SDLDZ80 ?= $(SDCC290_BIN_DIR)/link-z80-2.9.0
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
