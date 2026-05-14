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
SJASMPLUS   ?= $(SDK_DIR)tools/bin/sjasmplus
include $(SDK_DIR)toolchain.mk
SDK_CFLAGS   = $(SDCC_CFLAGS)
SDK_CPPFLAGS = $(SDCPPFLAGS) -I$(SDK_DIR)include

# --- Directories ---
LIB_SRC_DIR  = $(SDK_DIR)lib/src
GFX_SRC_DIR  = $(SDK_DIR)lib/gfx
LIB_DIR      = $(SDK_DIR)lib
BUILD_DIR    = $(SDK_DIR)build
INCLUDE_DIR  = $(SDK_DIR)include

# --- Source discovery (recursive, one function per file) ---
LIB_SUBDIRS  = dss bios video mouse keyb joystick assets audio stdio stdlib string ctype conio dir
LIB_WRAPPER_ASM_SRCS = $(foreach d,$(LIB_SUBDIRS),$(wildcard $(LIB_SRC_DIR)/$(d)/*.s))
LIB_WRAPPER_ASM_BASENAMES = $(notdir $(basename $(LIB_WRAPPER_ASM_SRCS)))
LIB_C_SRCS_ALL = $(foreach d,$(LIB_SUBDIRS),$(wildcard $(LIB_SRC_DIR)/$(d)/*.c))
LIB_C_SRCS   = $(filter-out $(foreach b,$(LIB_WRAPPER_ASM_BASENAMES),$(LIB_SRC_DIR)/%/$(b).c),$(LIB_C_SRCS_ALL))
LIB_ASM_SRCS = $(LIB_DIR)/crt0.s $(LIB_WRAPPER_ASM_SRCS)
LIB_EXTRA_ASM_SRCS = \
	$(LIB_DIR)/sdcc290_compat.s \
	$(LIB_DIR)/sdcc290_crt0_rle.s \
	$(LIB_DIR)/sdcc290_div.s \
	$(LIB_DIR)/sdcc290_divsigned.s \
	$(LIB_DIR)/sdcc290_divulong.s \
	$(LIB_DIR)/sdcc290_mod.s \
	$(LIB_DIR)/sdcc290_modulong.s \
	$(LIB_DIR)/sdcc290_mullong.s \
	$(LIB_DIR)/sdcc290_mul.s \
	$(LIB_DIR)/sdcc290_shift.s

# Flatten .rel names into BUILD_DIR (no subdirs in build/)
LIB_C_RELS   = $(patsubst %.c,$(BUILD_DIR)/%.rel,$(notdir $(LIB_C_SRCS)))
LIB_C_OBJS   = $(LIB_C_RELS:.rel=.o)
LIB_ASM_RELS = $(patsubst %.s,$(BUILD_DIR)/%.rel,$(notdir $(LIB_ASM_SRCS) $(LIB_EXTRA_ASM_SRCS)))
LIB_ASM_OBJS = $(LIB_ASM_RELS:.rel=.o)
CRT0_REL     = $(BUILD_DIR)/crt0.rel
CRT0_PAGE2_REL = $(BUILD_DIR)/crt0_page2.rel
SPRINTER_LIB = $(BUILD_DIR)/sprinter.lib

GFX_C_SRCS    = $(wildcard $(GFX_SRC_DIR)/*.c)
GFX_ASM_SRCS  = $(wildcard $(GFX_SRC_DIR)/*.s)
GFX_C_RELS    = $(patsubst %.c,$(BUILD_DIR)/%.rel,$(notdir $(GFX_C_SRCS)))
GFX_C_OBJS    = $(GFX_C_RELS:.rel=.o)
GFX_ASM_RELS  = $(patsubst %.s,$(BUILD_DIR)/%.rel,$(notdir $(GFX_ASM_SRCS)))
GFX_ASM_OBJS  = $(GFX_ASM_RELS:.rel=.o)
GFX_LIB       = $(BUILD_DIR)/gfx.lib

# VPATH: let make find .c files in subdirectories
VPATH = $(sort $(dir $(LIB_C_SRCS) $(LIB_ASM_SRCS) $(LIB_EXTRA_ASM_SRCS) $(GFX_C_SRCS) $(GFX_ASM_SRCS)))

# =========================================================================
.PHONY: all lib gfx examples clean install tools help

all: lib

help:
	@echo "ZX Sprinter SDCC SDK"
	@echo ""
	@echo "  make                  Build library with SDCC 2.9.0"
	@echo "  make gfx              Build optional graphics library"
	@echo "  make examples         Build all examples"
	@echo "  make examples PACK_ASSETS=1"
	@echo "                        Build examples with HRUST-packed resource files"
	@echo "  make clean            Clean"
	@echo "  make tools            Build sjasmplus"
	@echo "  make SDCC_OPT=...     Override optimization profile"
	@echo "  make SDCC290_BIN_DIR=/path/to/sdcc-2.9.0/bin"
	@echo "                        Use a specific SDCC 2.9.0 toolchain directory"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- CRT0 ---
$(CRT0_REL): $(LIB_DIR)/crt0.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $(CRT0_REL) $<
	cp $(CRT0_REL) $(basename $(CRT0_REL)).o

$(CRT0_PAGE2_REL): $(LIB_DIR)/crt0_page2.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $(CRT0_PAGE2_REL) $<
	cp $(CRT0_PAGE2_REL) $(basename $(CRT0_PAGE2_REL)).o

$(BUILD_DIR)/%.rel: %.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $@ $<
	cp $@ $(basename $@).o

# --- Library .c → .rel (VPATH finds sources in subdirs) ---
$(BUILD_DIR)/%.rel: %.c | $(BUILD_DIR)
	$(SDCPP) $(SDK_CPPFLAGS) $< > $(basename $@).i
	$(SDCC) $(SDK_CFLAGS) --c1mode -o $(basename $@).asm < $(basename $@).i
	$(SDASZ80) $(SDASZ_FLAGS) $@ $(basename $@).asm
	cp $@ $(basename $@).o

# --- Library archive (for selective linking) ---
SPRINTER_LIB_INPUTS = $(LIB_C_OBJS) $(LIB_ASM_OBJS)

$(SPRINTER_LIB): $(LIB_C_RELS) $(LIB_ASM_RELS)
	$(SDAR) r $@ $(SPRINTER_LIB_INPUTS)

GFX_LIB_INPUTS = $(GFX_C_OBJS) $(GFX_ASM_OBJS)

$(GFX_LIB): $(GFX_C_RELS) $(GFX_ASM_RELS)
	$(SDAR) r $@ $(GFX_LIB_INPUTS)

# --- Build library ---
lib: $(CRT0_REL) $(CRT0_PAGE2_REL) $(SPRINTER_LIB)
	@echo ""
	@echo "=== SDK Library Built ==="
	@echo "CRT0: $(CRT0_REL)"
	@echo "CRT0 page2: $(CRT0_PAGE2_REL)"
	@echo "Library: $(SPRINTER_LIB) ($(shell expr $(words $(LIB_C_RELS)) + $(words $(LIB_ASM_RELS))) modules)"

gfx: lib $(GFX_LIB)
	@echo ""
	@echo "=== GFX Library Built ==="
	@echo "Library: $(GFX_LIB) ($(shell expr $(words $(GFX_C_RELS)) + $(words $(GFX_ASM_RELS))) modules)"

# --- Build all examples ---
examples: lib gfx
	@for dir in $(SDK_DIR)examples/*/; do \
		echo "=== Building $$(basename $$dir) ==="; \
		$(MAKE) -C "$$dir" SDK_DIR=$(SDK_DIR) || exit 1; \
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
