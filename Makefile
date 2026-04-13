# =========================================================================
#  Makefile — ZX Sprinter SDCC SDK
# =========================================================================
#  Cross-platform build system for macOS / Linux / Windows (MSYS2)
#
#  Usage:
#    make              - Build SDK library
#    make examples     - Build all examples
#    make clean        - Clean build artifacts
#    make install      - Install to PREFIX (default: /usr/local)
#    make tools        - Build sjasmplus
# =========================================================================

# --- Configuration ---
PREFIX      ?= /usr/local
SDK_DIR     := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# --- Tools ---
SDCC        ?= sdcc
SDASZ80     ?= sdasz80
SDLDZ80     ?= sdldz80
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

# --- Library sources ---
LIB_C_SRCS   = $(wildcard $(LIB_SRC_DIR)/*.c)
LIB_ASM_SRCS = $(LIB_DIR)/crt0.s
LIB_C_RELS   = $(patsubst $(LIB_SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(LIB_C_SRCS))
CRT0_REL     = $(BUILD_DIR)/crt0.rel

# =========================================================================
#  Targets
# =========================================================================

.PHONY: all lib examples clean install tools help

all: lib

help:
	@echo "ZX Sprinter SDCC SDK - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make              Build SDK library"
	@echo "  make examples     Build all example programs"
	@echo "  make clean        Clean build artifacts"
	@echo "  make tools        Build sjasmplus assembler"
	@echo "  make install      Install SDK to PREFIX ($(PREFIX))"
	@echo ""
	@echo "Variables:"
	@echo "  SDCC=path         Path to SDCC compiler"
	@echo "  PREFIX=path       Install prefix (default: /usr/local)"
	@echo ""
	@echo "Building a project:"
	@echo "  make -f path/to/sdk/Makefile APP=myapp SRC=main.c"

# --- Build directory ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- CRT0 ---
$(CRT0_REL): $(LIB_DIR)/crt0.s | $(BUILD_DIR)
	$(SDASZ80) $(SDASZ_FLAGS) $(CRT0_REL) $<

# --- Library C sources -> .rel ---
$(BUILD_DIR)/%.rel: $(LIB_SRC_DIR)/%.c | $(BUILD_DIR)
	$(SDCC) $(SDCC_CFLAGS) -c -o $@ $<

# --- Build library ---
lib: $(CRT0_REL) $(LIB_C_RELS)
	@echo ""
	@echo "=== SDK Library Built ==="
	@echo "CRT0: $(CRT0_REL)"
	@echo "Modules: $(LIB_C_RELS)"
	@echo ""
	@echo "To compile a program:"
	@echo "  $(SDCC) -mz80 --no-std-crt0 --code-loc 0x8100 \\"
	@echo "    -I$(INCLUDE_DIR) \\"
	@echo "    $(CRT0_REL) $(LIB_C_RELS) \\"
	@echo "    main.c -o main.ihx"
	@echo "  $(PYTHON) $(SDK_DIR)tools/ihx2exe.py main.ihx main.exe"

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
	mkdir -p $(PREFIX)/share/sprinter-sdk/lib
	mkdir -p $(PREFIX)/share/sprinter-sdk/include
	mkdir -p $(PREFIX)/share/sprinter-sdk/tools
	cp -r $(INCLUDE_DIR)/* $(PREFIX)/share/sprinter-sdk/include/
	cp $(BUILD_DIR)/*.rel $(PREFIX)/share/sprinter-sdk/lib/
	cp $(SDK_DIR)tools/ihx2exe.py $(PREFIX)/share/sprinter-sdk/tools/
	@echo "SDK installed to $(PREFIX)/share/sprinter-sdk/"

# =========================================================================
#  APP build rule — for building user projects
#  Usage: make -f /path/to/sdk/Makefile APP=myapp SRC="main.c util.c"
# =========================================================================

ifdef APP
SRC         ?= main.c
APP_RELS    = $(patsubst %.c,$(BUILD_DIR)/app_%.rel,$(SRC))

$(BUILD_DIR)/app_%.rel: %.c | $(BUILD_DIR)
	$(SDCC) $(SDCC_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(APP).ihx: $(CRT0_REL) $(LIB_C_RELS) $(APP_RELS)
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --code-loc 0x8100 --data-loc 0x4000 \
		$(SDCC_OPT) -I$(INCLUDE_DIR) \
		$(CRT0_REL) $(LIB_C_RELS) $(APP_RELS) \
		-o $@

$(APP).exe: $(BUILD_DIR)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@

app: $(APP).exe
	@echo "Built: $(APP).exe"
endif
