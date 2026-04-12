# common.mk — Shared build rules for SDK examples
#
# Each example Makefile should define:
#   APP     = program name (without extension)
#   SRCS    = list of .c source files
#   SDK_DIR = path to SDK root (passed from parent Makefile)

SDK_DIR     ?= ../../
SDCC        ?= sdcc
SDASZ80     ?= sdasz80
PYTHON      ?= python3

BUILD       = _build
SDCC_TARGET = -mz80
SDCC_FLAGS  = $(SDCC_TARGET) --sdcccall 0 --max-allocs-per-node 5000 --opt-code-speed
INC         = -I$(SDK_DIR)include
CRT0        = $(SDK_DIR)build/crt0.rel
LIB_RELS    = $(wildcard $(SDK_DIR)build/dss.rel $(SDK_DIR)build/bios.rel \
               $(SDK_DIR)build/mouse.rel $(SDK_DIR)build/video.rel)

APP_RELS    = $(patsubst %.c,$(BUILD)/%.rel,$(SRCS))

.PHONY: all clean

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.rel: %.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

$(BUILD)/$(APP).ihx: $(CRT0) $(LIB_RELS) $(APP_RELS)
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --sdcccall 0 --code-loc 0x8100 --data-loc 0x4000 \
		--max-allocs-per-node 5000 --opt-code-speed \
		$(INC) \
		$(CRT0) $(LIB_RELS) $(APP_RELS) \
		-o $@

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe *.ihx *.lk *.map *.mem *.noi *.sym
