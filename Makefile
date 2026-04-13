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

# --- SDCC flags ---
SDCC_TARGET  = -mz80
SDCC_OPT     = --max-allocs-per-node 5000 --opt-code-speed
SDCC_CFLAGS  = $(SDCC_TARGET) -I$(SDK_DIR)include $(SDCC_OPT)
SDASZ_FLAGS  = -plosgff

# --- Directories ---
LIB_SRC_DIR  = $(SDK_DIR)lib/src
LIB_DIR      = $(SDK_DIR)lib
BUILD_DIR    = $(SDK_DIR)build
INCLUDE_DIR  = $(SDK_DIR)include

# --- Source discovery (recursive, one function per file) ---
LIB_SUBDIRS  = dss bios video mouse stdio stdlib string ctype conio
LIB_C_SRCS   = $(foreach d,$(LIB_SUBDIRS),$(wildcard $(LIB_SRC_DIR)/$(d)/*.c))
LIB_ASM_SRCS = $(LIB_DIR)/crt0.s

# Flatten .rel names into BUILD_DIR (no subdirs in build/)
LIB_C_RELS   = $(patsubst %.c,$(BUILD_DIR)/%.rel,$(notdir $(LIB_C_SRCS)))
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

# --- Library .c → .rel (VPATH finds sources in subdirs) ---
$(BUILD_DIR)/%.rel: %.c | $(BUILD_DIR)
	$(SDCC) $(SDCC_CFLAGS) -c -o $@ $<

# --- Library archive (for selective linking) ---
SPRINTER_LIB = $(BUILD_DIR)/sprinter.lib

$(SPRINTER_LIB): $(LIB_C_RELS)
	sdar rc $@ $^

# --- Build library ---
lib: $(CRT0_REL) $(SPRINTER_LIB)
	@echo ""
	@echo "=== SDK Library Built ==="
	@echo "CRT0: $(CRT0_REL)"
	@echo "Library: $(SPRINTER_LIB) ($(words $(LIB_C_RELS)) modules)"

# --- Build all examples ---
examples: lib
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
