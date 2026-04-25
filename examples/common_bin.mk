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
include $(SDK_DIR)toolchain.mk

BUILD       = _build
INC         = -I$(SDK_DIR)include
SPRLIB      = $(SDK_DIR)build/sprinter.lib
DATA_LOC    ?=

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))
APP_OBJS    = $(APP_RELS:.rel=.o)
APP_LINK    = $(APP_RELS)
APP_CFLAGS   = $(SDCC_CFLAGS)
APP_CPPFLAGS = $(SDCPPFLAGS) $(INC)

.PHONY: all clean

all: $(APP).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCPP) $(APP_CPPFLAGS) $< > $(basename $@).i
	$(SDCC) $(APP_CFLAGS) --c1mode -o $(basename $@).asm < $(basename $@).i
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

$(BUILD)/$(APP).ihx: $(APP_RELS)
	printf '%s\n' '-mjx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $@' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/$(APP).lk
	if [ -n "$(DATA_LOC)" ]; then printf '%s\n' '-b _DATA = $(DATA_LOC)' >> $(BUILD)/$(APP).lk; fi
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/$(APP).lk
	for rel in $(APP_LINK); do printf '%s\n' "$$rel" >> $(BUILD)/$(APP).lk; done
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(SDLDZ80) -n -f $(BUILD)/$(APP).lk

$(APP).bin: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2bin.py $< $@ --org $(CODE_LOC)
	@echo "Built: $@ (load at $(CODE_LOC))"

clean:
	rm -rf $(BUILD) $(APP).bin
