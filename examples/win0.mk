# win0.mk - build an example in the extended WIN0..WIN2 layout (~47 KB).
# (see docs/ru/11_extended_layout.md)
#
# Example Makefile:
#     APP  = myapp
#     SRCS = main.c other.c
#     include ../win0.mk
#
# Produces a single PRELOAD $(APP).exe: a stage-1 loader streams the payload
# (your code @0x0300 in WIN0, optionally spilling into WIN1, plus the win0
# runtime trampolines @0x8000 in a private WIN2) from the .EXE file into GETMEM'd
# pages. Code+rodata may span WIN0+WIN1 (~31 KB); data/stack live in WIN2.
# Use the w_* wrappers (win0_io.h) for pointers that may live in WIN0.

SDK_DIR     ?= ../../
include $(SDK_DIR)toolchain.mk

BUILD       = _build
WIN0DIR     = $(SDK_DIR)lib/win0
INC         = -I$(SDK_DIR)include -I$(WIN0DIR)
SPRLIB      = $(SDK_DIR)build/sprinter.lib
CRT0        = $(SDK_DIR)build/crt0.rel

# Payload link origins (WIN0 code / WIN2 trampolines / WIN2 data).
# Low WIN0: 0x0000-3F vectors, 0x40 boot params, 0x66 NMI, 0x80-17F cmd line.
WIN0_CODE   ?= 0x0180
WIN0_WINRT  ?= 0x8000
WIN0_DATA   ?= 0x8400

APP_RELS     = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))
PAYLOAD_RELS = $(BUILD)/crt0_win0.rel $(BUILD)/win0_rt.rel \
               $(BUILD)/win0_dss.rel $(BUILD)/dss_raw.rel $(APP_RELS)

VPATH = $(WIN0DIR)

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

# --- payload: your code in WIN0(+WIN1) + runtime trampolines in WIN2 ---
$(BUILD)/payload.ihx: $(PAYLOAD_RELS) $(SPRLIB)
	printf '%s\n' '-mjx' > $(BUILD)/payload.lk
	printf '%s\n' '-i $(BUILD)/payload.ihx' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _CODE = $(WIN0_CODE)' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _WINRT = $(WIN0_WINRT)' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _DATA = $(WIN0_DATA)' >> $(BUILD)/payload.lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/crt0_win0.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_rt.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_dss.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/dss_raw.rel)' >> $(BUILD)/payload.lk
	for rel in $(APP_RELS); do printf '%s\n' "$(abspath $(BUILD))/$$(basename $$rel)" >> $(BUILD)/payload.lk; done
	printf '%s\n' '-e' >> $(BUILD)/payload.lk
	$(SDLDZ80) -n -f $(BUILD)/payload.lk

# --- stage-1 PRELOAD loader (@0x8100), independent of the payload ---
$(BUILD)/loader.rel: $(WIN0DIR)/loader.c | $(BUILD)
	$(SDCPP) $(INC) $(WIN0DIR)/loader.c > $(BUILD)/loader.i
	$(SDCC) $(SDCC_CFLAGS) --c1mode -o $(BUILD)/loader.asm < $(BUILD)/loader.i
	$(SDASZ80) -plosgff $@ $(BUILD)/loader.asm
	cp $@ $(basename $@).o

$(BUILD)/loader.ihx: $(CRT0) $(SPRLIB) $(BUILD)/loader.rel
	printf '%s\n' '-mjx' > $(BUILD)/loader.lk
	printf '%s\n' '-i $(BUILD)/loader.ihx' >> $(BUILD)/loader.lk
	printf '%s\n' '-b _CODE = 0x8100' >> $(BUILD)/loader.lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/loader.lk
	printf '%s\n' '$(abspath $(CRT0))' >> $(BUILD)/loader.lk
	printf '%s\n' '$(abspath $(BUILD)/loader.rel)' >> $(BUILD)/loader.lk
	printf '%s\n' '-e' >> $(BUILD)/loader.lk
	$(SDLDZ80) -n -f $(BUILD)/loader.lk

$(APP).exe: $(BUILD)/loader.ihx $(BUILD)/payload.ihx $(SDK_DIR)tools/win0_exe.py
	$(PYTHON) $(SDK_DIR)tools/win0_exe.py $(BUILD)/loader.ihx $(BUILD)/payload.ihx $@
	@echo "Built (win0 layout): $@"

clean:
	rm -rf $(BUILD) $(APP).exe
