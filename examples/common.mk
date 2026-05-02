# common.mk — Shared build rules for SDK examples
#
# Configurable memory layout (override in your Makefile before include):
#   CODE_LOC  — code start address (default: 0x4100, like SOLID C)
#   STACK     — initial stack pointer (default: 0xBFFF)
#
# Example: compact layout (16KB code window only)
#   CODE_LOC = 0x8100
#   STACK    = 0xBFFF

SDK_DIR     ?= ../../
SDCC        ?= sdcc
PYTHON      ?= python3

# --- Memory layout defaults (32KB: WIN1+WIN2, like SOLID C) ---
ifeq ($(CRT0_PAGE2),1)
CODE_LOC    ?= 0x4200
STACK       ?= 0x40FF
CRT0_NAME   ?= crt0_page2
else
CODE_LOC    ?= 0x4100
STACK       ?= 0xBFFF
CRT0_NAME   ?= crt0
endif

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --max-allocs-per-node 5000 --opt-code-speed
INC         = -I$(SDK_DIR)include
CRT0        = $(SDK_DIR)build/$(CRT0_NAME).rel
SPRLIB      = $(SDK_DIR)build/sprinter.lib

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))

.PHONY: all clean

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(APP_RELS)
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --code-loc $(CODE_LOC) \
		--max-allocs-per-node 5000 --opt-code-speed \
		$(INC) \
		$(CRT0) $(APP_RELS) \
		-l$(SPRLIB) \
		-o $@

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe *.ihx *.lk *.map *.mem *.noi *.sym
