# win0.mk - build an example in the extended WIN0..WIN2 layout (~47 KB).
#
# Example Makefile:
#     APP  = myapp
#     SRCS = main.c other.c
#     include ../win0.mk
#
# Produces a single PRELOAD $(APP).exe: a stage-1 loader streams the payload
# (your code @0x0180 in WIN0, optionally spilling into WIN1, plus the win0
# runtime trampolines @0x8000 in a private WIN2) from the .EXE file into GETMEM'd
# pages. Code+rodata may span WIN0+WIN1 (~31 KB); data/stack live in WIN2.
#
# Adapted for the sdcc45 SDK: single-pass sdcc (no toolchain.mk / --c1mode);
# the custom multi-area link is done with sdldz80 directly via a .lk script.

SDK_DIR     ?= ../../
SDCC        ?= sdcc
SDASZ80     ?= sdasz80
SDLDZ80     ?= sdldz80
PYTHON      ?= python3

BUILD       = _build
WIN0DIR     = $(SDK_DIR)lib/win0
INC         = -I$(SDK_DIR)include -I$(WIN0DIR)
SPRLIB      = $(SDK_DIR)build/sprinter.lib
CRT0        = $(SDK_DIR)build/crt0.rel

SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --max-allocs-per-node 5000 --opt-code-speed

# SDCC's own z80 runtime library, linked last so it only supplies helpers
# (e.g. __divuint / __moduint pulled in by printf) that sprinter.lib lacks.
# sprinter.lib is listed first, so its DSS-backed printf/puts/memcpy win and
# the matching z80.lib modules are never pulled (no duplicate-symbol warnings).
SDCC_Z80LIB := $(shell for d in $$($(SDCC) -mz80 --print-search-dirs 2>/dev/null | sed -n '/libdir:/,/libpath:/p' | grep '^/'); do if [ -f "$$d/z80.lib" ]; then echo "$$d/z80.lib"; break; fi; done)

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

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

# --- payload: your code in WIN0(+WIN1) + runtime trampolines in WIN2 ---
$(BUILD)/payload.ihx: $(PAYLOAD_RELS) $(SPRLIB)
	printf '%s\n' '-mjx' > $(BUILD)/payload.lk
	printf '%s\n' '-i $(BUILD)/payload.ihx' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _CODE = $(WIN0_CODE)' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _WINRT = $(WIN0_WINRT)' >> $(BUILD)/payload.lk
	printf '%s\n' '-b _DATA = $(WIN0_DATA)' >> $(BUILD)/payload.lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/payload.lk
	printf '%s\n' '-l $(SDCC_Z80LIB)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/crt0_win0.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_rt.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/win0_dss.rel)' >> $(BUILD)/payload.lk
	printf '%s\n' '$(abspath $(BUILD)/dss_raw.rel)' >> $(BUILD)/payload.lk
	for rel in $(APP_RELS); do printf '%s\n' "$(abspath $(BUILD))/$$(basename $$rel)" >> $(BUILD)/payload.lk; done
	printf '%s\n' '-e' >> $(BUILD)/payload.lk
	$(SDLDZ80) -n -f $(BUILD)/payload.lk

# --- stage-1 PRELOAD loader (@0x8100), independent of the payload ---
$(BUILD)/loader.ihx: $(CRT0) $(SPRLIB) $(BUILD)/loader.rel
	printf '%s\n' '-mjx' > $(BUILD)/loader.lk
	printf '%s\n' '-i $(BUILD)/loader.ihx' >> $(BUILD)/loader.lk
	printf '%s\n' '-b _CODE = 0x8100' >> $(BUILD)/loader.lk
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/loader.lk
	printf '%s\n' '-l $(SDCC_Z80LIB)' >> $(BUILD)/loader.lk
	printf '%s\n' '$(abspath $(CRT0))' >> $(BUILD)/loader.lk
	printf '%s\n' '$(abspath $(BUILD)/loader.rel)' >> $(BUILD)/loader.lk
	printf '%s\n' '-e' >> $(BUILD)/loader.lk
	$(SDLDZ80) -n -f $(BUILD)/loader.lk

$(APP).exe: $(BUILD)/loader.ihx $(BUILD)/payload.ihx $(SDK_DIR)tools/win0_exe.py
	$(PYTHON) $(SDK_DIR)tools/win0_exe.py $(BUILD)/loader.ihx $(BUILD)/payload.ihx $@
	@echo "Built (win0 layout): $@"

clean:
	rm -rf $(BUILD) $(APP).exe
