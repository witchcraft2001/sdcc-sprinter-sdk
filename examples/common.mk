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
SDASZ80     ?= sdasz80
SDCC_LINK   ?= $(SDCC)
PYTHON      ?= python3

# --- SDCC compatibility ---
SDCC_PATH_DIR := $(dir $(shell command -v $(SDCC) 2>/dev/null))
SDCC_VERSION  := $(shell $(SDCC) --version 2>/dev/null)
HOST_SDCC     := $(shell which -a sdcc 2>/dev/null | grep -v '^$(SDCC_PATH_DIR)' | head -n 1)
SDCC_AUX_PATH := $(SDCC_PATH_DIR):$(SDCC_PATH_DIR)/bin:$(SDCC_PATH_DIR)/sdcc/bin:$(SDCC_PATH_DIR)/../bin:$(SDCC_PATH_DIR)/../sdcc/bin
export PATH   := $(SDCC_AUX_PATH):$(PATH)

# --- Memory layout defaults (32KB: WIN1+WIN2, like SOLID C) ---
CODE_LOC    ?= 0x4100
STACK       ?= 0xBFFF
DATA_LOC    ?=

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --opt-code-speed
ifneq (,$(findstring 2.9.0,$(SDCC_VERSION)))
else
SDCC_FLAGS += --max-allocs-per-node 5000
endif
INC         = -I$(SDK_DIR)include
CRT0        = $(SDK_DIR)build/crt0.rel
SPRLIB      = $(SDK_DIR)build/sprinter.lib

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))
APP_OBJS    = $(APP_RELS:.rel=.o)
CRT0_LINK   = $(CRT0)
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

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -S -o $(basename $@).asm $<
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

ifeq (,$(findstring 2.9.0,$(SDCC_VERSION)))
$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(APP_RELS)
	$(LINK_DRIVER) $(SDCC_TARGET) --no-std-crt0 --code-loc $(CODE_LOC) \
		$(filter-out $(SDCC_TARGET),$(SDCC_FLAGS)) \
		$(INC) \
		$(CRT0_LINK) $(APP_LINK) \
		-l$(SPRLIB) \
		-o $@
else
$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(APP_RELS)
	printf '%s\n' '-mjwx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $@' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/$(APP).lk
	if [ -n "$(DATA_LOC)" ]; then printf '%s\n' '-b _DATA = $(DATA_LOC)' >> $(BUILD)/$(APP).lk; fi
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/$(APP).lk
	printf '%s\n' '$(abspath $(CRT0_LINK))' >> $(BUILD)/$(APP).lk
	for rel in $(APP_LINK); do printf '%s\n' "$$rel" >> $(BUILD)/$(APP).lk; done
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(HOST_SDLDZ80) -nf $(BUILD)/$(APP).lk
endif

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe *.ihx *.lk *.map *.mem *.noi *.sym
