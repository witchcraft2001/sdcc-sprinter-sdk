# Phase 3: pointer marshalling (bounce) in the WIN0..WIN2 layout.
# Payload = crt0_win0 + win0_rt + win0_io (bounce wrappers) + main (demo),
# linked at 0x0300/0xA000; embedded into the stage-1 loader (@0x8100).

SDK_DIR     ?= ../../
include $(SDK_DIR)toolchain.mk

APP         = win0io
BUILD       = _build
INC         = -I$(SDK_DIR)include
SPRLIB      = $(SDK_DIR)build/sprinter.lib
CRT0        = $(SDK_DIR)build/crt0.rel

PAYLOAD_RELS = $(BUILD)/crt0_win0.rel $(BUILD)/win0_rt.rel \
               $(BUILD)/win0_io.rel $(BUILD)/main.rel

.PHONY: all clean
all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.s | $(BUILD)
	$(SDASZ80) -plosgff $@ $<
	cp $@ $(basename $@).o

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCPP) $(INC) $< > $(basename $@).i
	$(SDCC) $(SDCC_CFLAGS) --c1mode -o $(basename $@).asm < $(basename $@).i
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

# --- payload: code in WIN0 (0x0300), trampolines in WIN2 (0xA000) ---
$(BUILD)/payload.ihx: $(PAYLOAD_RELS) $(SPRLIB)
	printf '%s\n' '-mjx' > $(BUILD)/payload.lk
	printf '%s\n' '-i $(BUILD)/payload.ihx' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _CODE = 0x0300' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _WINRT = 0xA000' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _DATA = 0xA400' >> $(BUILD)/payload.lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/crt0_win0.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_rt.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_io.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/main.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '-e' >> $(BUILD)/payload.lk
	$(SDLDZ80) -n -f $(BUILD)/payload.lk

payload_blobs.h: $(BUILD)/payload.ihx $(SDK_DIR)tools/win0_pack.py
	$(PYTHON) $(SDK_DIR)tools/win0_pack.py $(BUILD)/payload.ihx payload_blobs.h

# --- loader: normal .EXE at 0x8100, embeds the packed payload ---
$(BUILD)/loader.rel: loader.c payload_blobs.h | $(BUILD)
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
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load 0x8100 --entry 0x8100 --stack 0xBFFF
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe payload_blobs.h
