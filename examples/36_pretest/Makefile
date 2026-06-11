# PRELOAD verification spike (Phase 4b foundation).
SDK_DIR ?= ../../
include $(SDK_DIR)toolchain.mk

APP     = pretest
BUILD   = _build
INC     = -I$(SDK_DIR)include
SPRLIB  = $(SDK_DIR)build/sprinter.lib
CRT0    = $(SDK_DIR)build/crt0.rel
HDRSIZE ?= 512

.PHONY: all clean
all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/loader.rel: loader.c | $(BUILD)
	$(SDCPP) $(INC) loader.c > $(BUILD)/loader.i
	$(SDCC) $(SDCC_CFLAGS) --c1mode -o $(BUILD)/loader.asm < $(BUILD)/loader.i
	$(SDASZ80) -plosgff $@ $(BUILD)/loader.asm
	cp $@ $(basename $@).o

$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(BUILD)/loader.rel
	printf '%s\n' '-mjx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $(BUILD)/$(APP).ihx' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = 0x8100' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/$(APP).lk
	printf '%s\n' '$(abspath $(CRT0))' >> $(BUILD)/$(APP).lk
	printf '%s\n' '$(abspath $(BUILD)/loader.rel)' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(SDLDZ80) -n -f $(BUILD)/$(APP).lk

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/win0_preexe.py $< $@ $(HDRSIZE)
	@echo "Built (PRELOAD, hdr=$(HDRSIZE)): $@"

clean:
	rm -rf $(BUILD) $(APP).exe
