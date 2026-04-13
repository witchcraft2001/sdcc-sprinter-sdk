# common_bin.mk — Build raw binary module (no EXE header, no CRT0)
#
# For resident code, overlays, and code fragments loaded at runtime.
#
# Variables (set before include):
#   APP      — output name (produces APP.bin)
#   SRCS     — source file(s)
#   CODE_LOC — load/run address (required, e.g., 0xC000)
#
# Example Makefile:
#   APP      = overlay
#   SRCS     = overlay.c
#   CODE_LOC = 0xC000
#   include $(SDK_DIR)examples/common_bin.mk

SDK_DIR     ?= ../../
SDCC        ?= sdcc
PYTHON      ?= python3

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --max-allocs-per-node 5000 --opt-code-speed
INC         = -I$(SDK_DIR)include
SPRLIB      = $(SDK_DIR)build/sprinter.lib

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))

.PHONY: all clean

all: $(APP).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

# Link without CRT0, no standard libs
$(BUILD)/$(APP).ihx: $(APP_RELS)
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --code-loc $(CODE_LOC) \
		--no-std-crt0 --nostdinc --nostdlib \
		$(SDCC_FLAGS) $(INC) \
		$(APP_RELS) \
		-l$(SPRLIB) \
		-o $@

$(APP).bin: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2bin.py $< $@ --org $(CODE_LOC)
	@echo "Built: $@ (load at $(CODE_LOC))"

clean:
	rm -rf $(BUILD) $(APP).bin
