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
SDASZ80     ?= sdasz80
SDCC_LINK   ?= $(SDCC)
PYTHON      ?= python3

# --- SDCC compatibility ---
SDCC_PATH_DIR := $(dir $(shell command -v $(SDCC) 2>/dev/null))
SDCC_VERSION  := $(shell $(SDCC) --version 2>/dev/null)
HOST_SDCC     := $(shell which -a sdcc 2>/dev/null | grep -v '^$(SDCC_PATH_DIR)' | head -n 1)
SDCC_AUX_PATH := $(SDCC_PATH_DIR):$(SDCC_PATH_DIR)/bin:$(SDCC_PATH_DIR)/sdcc/bin:$(SDCC_PATH_DIR)/../bin:$(SDCC_PATH_DIR)/../sdcc/bin
export PATH   := $(SDCC_AUX_PATH):$(PATH)

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --opt-code-speed
ifneq (,$(findstring 2.9.0,$(SDCC_VERSION)))
else
SDCC_FLAGS += --max-allocs-per-node 5000
endif
INC         = -I$(SDK_DIR)include
SPRLIB      = $(SDK_DIR)build/sprinter.lib
DATA_LOC    ?=

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))
APP_OBJS    = $(APP_RELS:.rel=.o)
APP_LINK    = $(APP_RELS)
LINK_DRIVER = $(SDCC_LINK)
HOST_SDLDZ80 = $(dir $(LINK_DRIVER))sdldz80

ifneq (,$(findstring 2.9.0,$(SDCC_VERSION)))
ifeq ($(origin SDCC_LINK), file)
ifneq ($(HOST_SDCC),)
LINK_DRIVER = $(HOST_SDCC)
endif
endif
HOST_SDLDZ80 = $(dir $(LINK_DRIVER))sdldz80
endif

.PHONY: all clean

all: $(APP).bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -S -o $(basename $@).asm $<
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

ifeq (,$(findstring 2.9.0,$(SDCC_VERSION)))
# Link without CRT0, no standard libs
$(BUILD)/$(APP).ihx: $(APP_RELS)
	$(LINK_DRIVER) $(SDCC_TARGET) --no-std-crt0 --code-loc $(CODE_LOC) \
		--no-std-crt0 --nostdinc --nostdlib \
		$(SDCC_FLAGS) $(INC) \
		$(APP_LINK) \
		-l$(SPRLIB) \
		-o $@
else
$(BUILD)/$(APP).ihx: $(APP_RELS)
	printf '%s\n' '-mjwx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $@' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/$(APP).lk
	if [ -n "$(DATA_LOC)" ]; then printf '%s\n' '-b _DATA = $(DATA_LOC)' >> $(BUILD)/$(APP).lk; fi
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/$(APP).lk
	for rel in $(APP_LINK); do printf '%s\n' "$$rel" >> $(BUILD)/$(APP).lk; done
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(HOST_SDLDZ80) -nf $(BUILD)/$(APP).lk
endif

$(APP).bin: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2bin.py $< $@ --org $(CODE_LOC)
	@echo "Built: $@ (load at $(CODE_LOC))"

clean:
	rm -rf $(BUILD) $(APP).bin
