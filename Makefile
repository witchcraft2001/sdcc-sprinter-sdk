# =========================================================================
#  Makefile — ZX Sprinter SDCC SDK
# =========================================================================
#  Usage:
#    make              - Build SDK library
#    make examples     - Build all examples
#    make clean        - Clean build artifacts
#    make install      - Install to PREFIX
#    make tools        - Build sjasmplus
# =========================================================================

PREFIX      ?= /usr/local
SDK_DIR     := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# --- Tools ---
SDCC        ?= sdcc
SDASZ80     ?= sdasz80
PYTHON      ?= python3
SJASMPLUS   ?= $(SDK_DIR)tools/bin/sjasmplus

# --- SDCC compatibility ---
SDCC_PATH_DIR := $(dir $(shell command -v $(SDCC) 2>/dev/null))
SDCC_VERSION  := $(shell $(SDCC) --version 2>/dev/null)
HOST_SDCC     := $(shell which -a sdcc 2>/dev/null | grep -v '^$(SDCC_PATH_DIR)' | head -n 1)
SDCC_AUX_PATH := $(SDCC_PATH_DIR):$(SDCC_PATH_DIR)/bin:$(SDCC_PATH_DIR)/sdcc/bin:$(SDCC_PATH_DIR)/../bin:$(SDCC_PATH_DIR)/../sdcc/bin
export PATH   := $(SDCC_AUX_PATH):$(PATH)

# --- SDCC flags ---
SDCC_TARGET  = -mz80
SDCC_OPT     = --opt-code-speed
ifneq (,$(findstring 2.9.0,$(SDCC_VERSION)))
else
SDCC_OPT    += --max-allocs-per-node 5000
endif
SDCC_CFLAGS  = $(SDCC_TARGET) -I$(SDK_DIR)include $(SDCC_OPT)
SDASZ_FLAGS  = -plosgff

# --- Directories ---
LIB_SRC_DIR  = $(SDK_DIR)lib/src
LIB_DIR      = $(SDK_DIR)lib
BUILD_DIR    = $(SDK_DIR)build
INCLUDE_DIR  = $(SDK_DIR)include

# --- Source discovery (recursive, one function per file) ---
LIB_SUBDIRS  = dss bios video mouse stdio stdlib string ctype conio dir
LIB_C_SRCS   = $(foreach d,$(LIB_SUBDIRS),$(wildcard $(LIB_SRC_DIR)/$(d)/*.c))
LIB_ASM_SRCS = $(LIB_DIR)/crt0.s
ifeq (,$(findstring 2.9.0,$(SDCC_VERSION)))
LIB_EXTRA_ASM_SRCS =
else
LIB_EXTRA_ASM_SRCS = \
	$(LIB_DIR)/sdcc290_compat.s \
	$(LIB_DIR)/sdcc290_crt0_rle.s \
	$(LIB_DIR)/sdcc290_div.s \
	$(LIB_DIR)/sdcc290_divsigned.s \
	$(LIB_DIR)/sdcc290_divulong.s \
	$(LIB_DIR)/sdcc290_mod.s \
	$(LIB_DIR)/sdcc290_modulong.s \
	$(LIB_DIR)/sdcc290_mul.s \
	$(LIB_DIR)/sdcc290_shift.s
endif

# Flatten .rel names into BUILD_DIR (no subdirs in build/)
LIB_C_RELS   = $(patsubst %.c,$(BUILD_DIR)/%.rel,$(notdir $(LIB_C_SRCS)))
LIB_C_OBJS   = $(LIB_C_RELS:.rel=.o)
LIB_ASM_RELS = $(patsubst $(LIB_DIR)/%.s,$(BUILD_DIR)/%.rel,$(LIB_EXTRA_ASM_SRCS))
LIB_ASM_OBJS = $(LIB_ASM_RELS:.rel=.o)
CRT0_REL     = $(BUILD_DIR)/crt0.rel

# VPATH: let make find .c files in subdirectories
VPATH = $(sort $(dir $(LIB_C_SRCS)))

# =========================================================================
.PHONY: all lib examples clean install tools help

all: lib

help:
	@echo "ZX Sprinter SDCC SDK"
	@echo ""
	@echo "  make              Build library"
	@echo "  make examples     Build all examples"
	@echo "  make clean        Clean"
	@echo "  make tools        Build sjasmplus"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- CRT0 ---
$(CRT0_REL): $(LIB_DIR)/crt0.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $(CRT0_REL) $<
	cp $(CRT0_REL) $(basename $(CRT0_REL)).o

$(BUILD_DIR)/%.rel: $(LIB_DIR)/%.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $@ $<
	cp $@ $(basename $@).o

# --- Library .c → .rel (VPATH finds sources in subdirs) ---
$(BUILD_DIR)/%.rel: %.c | $(BUILD_DIR)
	$(SDCC) $(SDCC_CFLAGS) -S -o $(basename $@).asm $<
	$(SDASZ80) $(SDASZ_FLAGS) $@ $(basename $@).asm
	cp $@ $(basename $@).o

# --- Library archive (for selective linking) ---
SPRINTER_LIB = $(BUILD_DIR)/sprinter.lib

ifneq (,$(findstring 2.9.0,$(SDCC_VERSION)))
SPRINTER_LIB_INPUTS = $(LIB_C_OBJS) $(LIB_ASM_OBJS)
else
SPRINTER_LIB_INPUTS = $(LIB_C_RELS) $(LIB_ASM_RELS)
endif

$(SPRINTER_LIB): $(LIB_C_RELS) $(LIB_ASM_RELS)
	sdar rc $@ $(SPRINTER_LIB_INPUTS)

# --- Build library ---
lib: $(CRT0_REL) $(SPRINTER_LIB)
	@echo ""
	@echo "=== SDK Library Built ==="
	@echo "CRT0: $(CRT0_REL)"
	@echo "Library: $(SPRINTER_LIB) ($(shell expr $(words $(LIB_C_RELS)) + $(words $(LIB_ASM_RELS))) modules)"

# --- Build all examples ---
examples: lib
	@for dir in $(SDK_DIR)examples/*/; do \
		echo "=== Building $$(basename $$dir) ==="; \
		$(MAKE) -C "$$dir" SDK_DIR=$(SDK_DIR) SDCC_LINK="$(HOST_SDCC)" || exit 1; \
	done

# --- Tools ---
tools:
	bash $(SDK_DIR)tools/build-sjasmplus.sh

# --- Clean ---
clean:
	rm -rf $(BUILD_DIR)
	@for dir in $(SDK_DIR)examples/*/; do \
		$(MAKE) -C "$$dir" clean SDK_DIR=$(SDK_DIR) 2>/dev/null || true; \
	done

# --- Install ---
install: lib
	mkdir -p $(PREFIX)/share/sprinter-sdk/{lib,include,tools}
	cp -r $(INCLUDE_DIR)/* $(PREFIX)/share/sprinter-sdk/include/
	cp $(BUILD_DIR)/*.rel $(PREFIX)/share/sprinter-sdk/lib/
	cp $(SDK_DIR)tools/ihx2exe.py $(PREFIX)/share/sprinter-sdk/tools/
	@echo "SDK installed to $(PREFIX)/share/sprinter-sdk/"
