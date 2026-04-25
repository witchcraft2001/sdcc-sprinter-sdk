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
include $(SDK_DIR)toolchain.mk

# --- Memory layout defaults (32KB: WIN1+WIN2, like SOLID C) ---
CODE_LOC    ?= 0x4100
STACK       ?= 0xBFFF
DATA_LOC    ?=

BUILD       = _build
INC         = -I$(SDK_DIR)include
CRT0        = $(SDK_DIR)build/crt0.rel
SPRLIB      = $(SDK_DIR)build/sprinter.lib
EXTRA_LIBS  ?=

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))
APP_OBJS    = $(APP_RELS:.rel=.o)
CRT0_LINK   = $(CRT0)
APP_LINK    = $(APP_RELS)

APP_CFLAGS   = $(SDCC_CFLAGS)
APP_CPPFLAGS = $(SDCPPFLAGS) $(INC)

.PHONY: all clean

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCPP) $(APP_CPPFLAGS) $< > $(basename $@).i
	$(SDCC) $(APP_CFLAGS) --c1mode -o $(basename $@).asm < $(basename $@).i
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(EXTRA_LIBS) $(APP_RELS)
	printf '%s\n' '-mjx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $@' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/$(APP).lk
	if [ -n "$(DATA_LOC)" ]; then printf '%s\n' '-b _DATA = $(DATA_LOC)' >> $(BUILD)/$(APP).lk; fi
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/$(APP).lk
	for lib in $(EXTRA_LIBS); do printf '%s\n' "-l $$(cd "$$(dirname "$$lib")" && pwd)/$$(basename "$$lib")" >> $(BUILD)/$(APP).lk; done
	printf '%s\n' '$(abspath $(CRT0_LINK))' >> $(BUILD)/$(APP).lk
	for rel in $(APP_LINK); do printf '%s\n' "$$rel" >> $(BUILD)/$(APP).lk; done
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(SDLDZ80) -n -f $(BUILD)/$(APP).lk

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe *.ihx *.lk *.map *.mem *.noi *.sym
