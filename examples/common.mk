# common.mk — Shared build rules for SDK examples

SDK_DIR     ?= ../../
SDCC        ?= sdcc
PYTHON      ?= python3

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --max-allocs-per-node 5000 --opt-code-speed
INC         = -I$(SDK_DIR)include
CRT0        = $(SDK_DIR)build/crt0.rel
SPRLIB      = $(SDK_DIR)build/sprinter.lib

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))

.PHONY: all clean

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

$(BUILD)/$(APP).ihx: $(CRT0) $(SPRLIB) $(APP_RELS)
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --code-loc 0x8100 --data-loc 0x4000 \
		--max-allocs-per-node 5000 --opt-code-speed \
		$(INC) \
		$(CRT0) $(APP_RELS) \
		-l$(SPRLIB) \
		-o $@

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe *.ihx *.lk *.map *.mem *.noi *.sym
